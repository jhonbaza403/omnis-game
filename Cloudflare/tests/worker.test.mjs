import test from "node:test";
import assert from "node:assert/strict";
import { readFile } from "node:fs/promises";

const workerSource = await readFile(new URL("../worker.js", import.meta.url), "utf8");
const moduleUrl = "data:text/javascript;base64," + Buffer.from(workerSource).toString("base64");
const { default: worker } = await import(moduleUrl);

class MemoryKV {
  constructor() {
    this.values = new Map();
    this.options = new Map();
  }

  async get(key, type) {
    const value = this.values.get(key);
    if (value === undefined) return null;
    return type === "json" ? JSON.parse(value) : value;
  }

  async put(key, value, options) {
    this.values.set(key, String(value));
    if (options) this.options.set(key, options);
  }

  async delete(key) {
    this.values.delete(key);
  }
}

function makeEnv(overrides = {}) {
  return {
    GITLAB_OAUTH_ENABLED: "true",
    GITLAB_OAUTH_CLIENT_ID: "test-client-id",
    GITLAB_OAUTH_CLIENT_SECRET: "test-client-secret",
    GITLAB_OAUTH_ALLOWED_USER_IDS: "12345",
    GITLAB_OAUTH_REDIRECT_URI: "https://omnis-game.example/oauth/callback",
    GITLAB_OAUTH_SCOPES: "create_runner manage_runner",
    OMNIS_OAUTH_ENCRYPTION_KEY: Buffer.alloc(32, 7).toString("base64"),
    OMNIS_RUNNER_DELIVERY_SECRET: "test-delivery-secret-with-more-than-32-characters",
    OMNIS_ENVIRONMENT: "test",
    OMNIS_OAUTH_KV: new MemoryKV(),
    ...overrides
  };
}

async function encryptForTest(env, value) {
  const rawKey = Buffer.from(env.OMNIS_OAUTH_ENCRYPTION_KEY, "base64");
  const key = await crypto.subtle.importKey("raw", rawKey, "AES-GCM", false, ["encrypt"]);
  const iv = crypto.getRandomValues(new Uint8Array(12));
  const plaintext = new TextEncoder().encode(JSON.stringify(value));
  const ciphertext = new Uint8Array(
    await crypto.subtle.encrypt({ name: "AES-GCM", iv }, key, plaintext)
  );
  const packed = new Uint8Array(iv.length + ciphertext.length);
  packed.set(iv, 0);
  packed.set(ciphertext, iv.length);
  return Buffer.from(packed).toString("base64");
}

function provisionRequest(sessionId, origin = "https://omnis-game.example") {
  return new Request(origin + "/runner/provision", {
    method: "POST",
    headers: {
      origin,
      "sec-fetch-site": "same-origin",
      cookie: "omnis_oauth_session=" + encodeURIComponent(sessionId),
      "content-type": "application/json"
    },
    body: "{}"
  });
}

test("a forged legacy cookie and Origin header cannot provision a runner", async () => {
  const env = makeEnv();
  const response = await worker.fetch(
    new Request("https://omnis-game.example/runner/provision", {
      method: "POST",
      headers: {
        origin: "https://omnis-game.example",
        cookie: "omnis_oauth_connected=1",
        "content-type": "application/json"
      },
      body: "{}"
    }),
    env
  );

  assert.equal(response.status, 401);
  assert.equal(await env.OMNIS_OAUTH_KV.get("gitlab:runner:credential"), null);
});

test("a GitLab user outside the allowlist cannot use a stored session", async () => {
  const env = makeEnv();
  await env.OMNIS_OAUTH_KV.put(
    "oauth:session:forged-session",
    JSON.stringify({ gitlab_user_id: "99999" }),
    { expirationTtl: 1800 }
  );

  const response = await worker.fetch(provisionRequest("forged-session"), env);
  assert.equal(response.status, 401);
  assert.equal(await env.OMNIS_OAUTH_KV.get("oauth:session:forged-session"), null);
});

test("a successful browser provisioning response never contains the runner token", async () => {
  const env = makeEnv();
  const sessionId = "allowlisted-session";
  await env.OMNIS_OAUTH_KV.put(
    "oauth:session:" + sessionId,
    JSON.stringify({ gitlab_user_id: "12345", username: "trusted-operator" }),
    { expirationTtl: 1800 }
  );
  await env.OMNIS_OAUTH_KV.put(
    "gitlab:oauth:credential",
    await encryptForTest(env, {
      access_token: "test-gitlab-access-token",
      refresh_token: null,
      expires_at: Math.floor(Date.now() / 1000) + 3600
    })
  );

  const previousFetch = globalThis.fetch;
  globalThis.fetch = async (url) => {
    assert.match(String(url), /\/user\/runners$/);
    return new Response(
      JSON.stringify({ id: 733, token: "glrt-never-return-this-to-browser" }),
      { status: 201, headers: { "content-type": "application/json" } }
    );
  };

  try {
    const response = await worker.fetch(provisionRequest(sessionId), env);
    const body = await response.json();
    assert.equal(response.status, 200);
    assert.equal(body.status, "provisioned");
    assert.equal(body.runner.id, 733);
    assert.equal(Object.hasOwn(body, "registration_token"), false);
    assert.equal(JSON.stringify(body).includes("glrt-never-return-this-to-browser"), false);

    const stored = await env.OMNIS_OAUTH_KV.get("gitlab:runner:credential");
    assert.ok(stored);
    assert.equal(
      env.OMNIS_OAUTH_KV.options.get("gitlab:runner:credential").expirationTtl,
      900
    );
  } finally {
    globalThis.fetch = previousFetch;
  }
});

test("runner token delivery is rejected from browser requests", async () => {
  const env = makeEnv();
  const response = await worker.fetch(
    new Request("https://omnis-game.example/runner/registration-token", {
      method: "POST",
      headers: {
        origin: "https://omnis-game.example",
        authorization: "Bearer " + env.OMNIS_RUNNER_DELIVERY_SECRET
      }
    }),
    env
  );
  assert.equal(response.status, 403);
});

test("runner token delivery requires the separate high-entropy delivery secret", async () => {
  const env = makeEnv();
  const response = await worker.fetch(
    new Request("https://omnis-game.example/runner/registration-token", {
      method: "POST",
      headers: { authorization: "Bearer wrong-delivery-secret" }
    }),
    env
  );
  assert.equal(response.status, 401);
});

test("runner status is not available without a verified operator session", async () => {
  const env = makeEnv();
  const response = await worker.fetch(
    new Request("https://omnis-game.example/runner/status", {
      headers: { cookie: "omnis_oauth_connected=1" }
    }),
    env
  );
  assert.equal(response.status, 401);
});

test("OAuth callback refuses identities outside the GitLab operator allowlist", async () => {
  const env = makeEnv();
  const state = "state-for-untrusted-account";
  await env.OMNIS_OAUTH_KV.put("oauth:state:" + state, "1", { expirationTtl: 600 });

  const previousFetch = globalThis.fetch;
  let call = 0;
  globalThis.fetch = async () => {
    call += 1;
    if (call === 1) {
      return new Response(
        JSON.stringify({
          access_token: "untrusted-account-access-token",
          refresh_token: "untrusted-account-refresh-token",
          expires_in: 7200
        }),
        { status: 200, headers: { "content-type": "application/json" } }
      );
    }
    return new Response(
      JSON.stringify({ id: 99999, username: "not-allowed" }),
      { status: 200, headers: { "content-type": "application/json" } }
    );
  };

  try {
    const response = await worker.fetch(
      new Request(
        "https://omnis-game.example/oauth/callback?state=" + state + "&code=test-code",
        { headers: { cookie: "omnis_oauth_state=" + state } }
      ),
      env
    );
    assert.equal(response.status, 403);
    assert.equal(await env.OMNIS_OAUTH_KV.get("gitlab:oauth:credential"), null);
    assert.equal(await env.OMNIS_OAUTH_KV.get("oauth:session:ignored"), null);
  } finally {
    globalThis.fetch = previousFetch;
  }
});


test("runner token delivery stops when Runner Control is disabled", async () => {
  const env = makeEnv({ GITLAB_OAUTH_ENABLED: "false" });
  const response = await worker.fetch(
    new Request("https://omnis-game.example/runner/registration-token", {
      method: "POST",
      headers: {
        authorization: "Bearer " + env.OMNIS_RUNNER_DELIVERY_SECRET
      }
    }),
    env
  );
  assert.equal(response.status, 503);
});

test("queue logs contain identifiers only and never serialize message bodies", async () => {
  const env = makeEnv();
  const previousLog = console.log;
  const captured = [];
  console.log = (value) => captured.push(String(value));

  try {
    await worker.queue({
      queue: "omnis-events",
      messages: [{
        id: "message-42",
        attempts: 2,
        timestamp: new Date("2026-11-03T12:00:00.000Z"),
        body: {
          email: "private@example.invalid",
          access_token: "body-secret-token",
          order_total: 999
        }
      }]
    });
  } finally {
    console.log = previousLog;
  }

  assert.equal(captured.length, 1);
  assert.match(captured[0], /message-42/);
  assert.equal(captured[0].includes("private@example.invalid"), false);
  assert.equal(captured[0].includes("body-secret-token"), false);
  assert.equal(captured[0].includes("order_total"), false);
});
