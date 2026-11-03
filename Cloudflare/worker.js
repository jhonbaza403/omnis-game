const GITLAB_AUTHORIZE_URL = "https://gitlab.com/oauth/authorize";
const GITLAB_TOKEN_URL = "https://gitlab.com/oauth/token";
const GITLAB_API_BASE = "https://gitlab.com/api/v4";
const OAUTH_STATE_TTL_SECONDS = 600;
const OAUTH_SESSION_TTL_SECONDS = 1800;
const RUNNER_TOKEN_TTL_SECONDS = 900;
const TOKEN_REFRESH_SKEW_SECONDS = 300;
const OAUTH_STATE_COOKIE = "omnis_oauth_state";
const OAUTH_SESSION_COOKIE = "omnis_oauth_session";
const OAUTH_SESSION_PREFIX = "oauth:session:";
const TOKEN_KV_KEY = "gitlab:oauth:credential";
const META_KV_KEY = "gitlab:oauth:metadata";
const RUNNER_CREDENTIAL_KV_KEY = "gitlab:runner:credential";
const RUNNER_META_KV_KEY = "gitlab:runner:metadata";
const OMNIS_PROJECT_ID = "87302401";

function json(data, status = 200, extra = {}) {
  return new Response(JSON.stringify(data, null, 2), {
    status,
    headers: {
      "content-type": "application/json; charset=utf-8",
      "cache-control": "no-store, max-age=0",
      "pragma": "no-cache",
      "x-content-type-options": "nosniff",
      "referrer-policy": "no-referrer",
      ...extra
    }
  });
}

function escapeHtml(value) {
  return String(value)
    .replaceAll("&", "&amp;")
    .replaceAll("<", "&lt;")
    .replaceAll(">", "&gt;")
    .replaceAll('"', "&quot;")
    .replaceAll("'", "&#39;");
}

function html(title, body, status = 200, extraHeaders = {}) {
  return new Response(
    "<!doctype html>\n" +
    '<html lang="en">\n<head>\n' +
    '<meta charset="utf-8">\n' +
    '<meta name="viewport" content="width=device-width,initial-scale=1">\n' +
    "<title>" + escapeHtml(title) + "</title>\n" +
    "</head>\n<body>\n" +
    '<main style="font-family:system-ui,sans-serif;max-width:720px;margin:10vh auto;padding:24px">\n' +
    body +
    "\n</main>\n</body>\n</html>",
    {
      status,
      headers: {
        "content-type": "text/html; charset=utf-8",
        "cache-control": "no-store, max-age=0",
        "content-security-policy": "default-src 'none'; script-src 'unsafe-inline'; style-src 'unsafe-inline'; connect-src 'self'",
        "x-content-type-options": "nosniff",
        "referrer-policy": "no-referrer",
        ...extraHeaders
      }
    }
  );
}

function oauthEnabled(env) {
  return env.GITLAB_OAUTH_ENABLED === "true";
}

function allowedGitLabUserIds(env) {
  return new Set(
    String(env.GITLAB_OAUTH_ALLOWED_USER_IDS ?? "")
      .split(/[,\s]+/)
      .map((value) => value.trim())
      .filter((value) => /^\d+$/.test(value))
  );
}

function missingOAuthConfig(env) {
  const missing = [];
  if (!env.GITLAB_OAUTH_CLIENT_ID) missing.push("GITLAB_OAUTH_CLIENT_ID");
  if (!env.GITLAB_OAUTH_CLIENT_SECRET) missing.push("GITLAB_OAUTH_CLIENT_SECRET");
  if (!env.OMNIS_OAUTH_ENCRYPTION_KEY) missing.push("OMNIS_OAUTH_ENCRYPTION_KEY");
  if (!env.OMNIS_OAUTH_KV) missing.push("OMNIS_OAUTH_KV");
  if (!env.GITLAB_OAUTH_REDIRECT_URI) missing.push("GITLAB_OAUTH_REDIRECT_URI");
  if (allowedGitLabUserIds(env).size === 0) missing.push("GITLAB_OAUTH_ALLOWED_USER_IDS");
  return missing;
}

function requireConfig(env) {
  const missing = missingOAuthConfig(env);
  if (missing.length) {
    throw new Error("OAuth configuration incomplete: " + missing.join(", "));
  }
}

function cookieMap(request) {
  const out = {};
  for (const part of (request.headers.get("cookie") ?? "").split(";")) {
    const i = part.indexOf("=");
    if (i === -1) continue;
    const name = part.slice(0, i).trim();
    try {
      out[name] = decodeURIComponent(part.slice(i + 1).trim());
    } catch {
      // Ignore malformed cookie values; never let a bad cookie create a 500.
    }
  }
  return out;
}

function cookie(name, value, maxAgeSeconds, sameSite = "Lax") {
  return name + "=" + encodeURIComponent(value) +
    "; Max-Age=" + maxAgeSeconds +
    "; Path=/; HttpOnly; Secure; SameSite=" + sameSite;
}

function deleteCookie(name, sameSite = "Lax") {
  return name + "=; Max-Age=0; Path=/; HttpOnly; Secure; SameSite=" + sameSite;
}

function isSameOriginBrowserRequest(request) {
  const origin = request.headers.get("origin");
  const requestOrigin = new URL(request.url).origin;
  if (origin !== requestOrigin) return false;
  const fetchSite = request.headers.get("sec-fetch-site");
  return !fetchSite || fetchSite === "same-origin";
}

function constantTimeEqual(actual, expected) {
  const left = new TextEncoder().encode(String(actual ?? ""));
  const right = new TextEncoder().encode(String(expected ?? ""));
  if (left.length === 0 || right.length === 0) return false;
  let mismatch = left.length ^ right.length;
  const length = Math.max(left.length, right.length);
  for (let index = 0; index < length; index += 1) {
    mismatch |= (left[index] ?? 0) ^ (right[index] ?? 0);
  }
  return mismatch === 0;
}

function base64FromBytes(bytes) {
  let binary = "";
  const chunkSize = 0x8000;
  for (let i = 0; i < bytes.length; i += chunkSize) {
    binary += String.fromCharCode(...bytes.subarray(i, i + chunkSize));
  }
  return btoa(binary);
}

function bytesFromBase64(value) {
  const binary = atob(value);
  const bytes = new Uint8Array(binary.length);
  for (let i = 0; i < binary.length; i++) bytes[i] = binary.charCodeAt(i);
  return bytes;
}

async function encryptionKey(env) {
  const raw = bytesFromBase64(env.OMNIS_OAUTH_ENCRYPTION_KEY);
  if (raw.byteLength !== 32) {
    throw new Error("OMNIS_OAUTH_ENCRYPTION_KEY must decode to exactly 32 bytes");
  }
  return crypto.subtle.importKey("raw", raw, "AES-GCM", false, ["encrypt", "decrypt"]);
}

async function encryptJson(env, value) {
  const key = await encryptionKey(env);
  const iv = crypto.getRandomValues(new Uint8Array(12));
  const plaintext = new TextEncoder().encode(JSON.stringify(value));
  const ciphertext = new Uint8Array(
    await crypto.subtle.encrypt({ name: "AES-GCM", iv }, key, plaintext)
  );
  const packed = new Uint8Array(iv.length + ciphertext.length);
  packed.set(iv, 0);
  packed.set(ciphertext, iv.length);
  return base64FromBytes(packed);
}

async function decryptJson(env, packedValue) {
  const packed = bytesFromBase64(packedValue);
  if (packed.length <= 12) throw new Error("Invalid encrypted credential");
  const iv = packed.slice(0, 12);
  const ciphertext = packed.slice(12);
  const key = await encryptionKey(env);
  const plaintext = await crypto.subtle.decrypt({ name: "AES-GCM", iv }, key, ciphertext);
  return JSON.parse(new TextDecoder().decode(plaintext));
}

function scopes(env) {
  return (env.GITLAB_OAUTH_SCOPES ?? "create_runner manage_runner").trim();
}

async function getRunnerSession(request, env) {
  const sessionId = cookieMap(request)[OAUTH_SESSION_COOKIE];
  if (!sessionId || sessionId.length > 128 || !env.OMNIS_OAUTH_KV) return null;

  const session = await env.OMNIS_OAUTH_KV.get(OAUTH_SESSION_PREFIX + sessionId, "json");
  if (!session || !session.gitlab_user_id) return null;

  const userId = String(session.gitlab_user_id);
  if (!allowedGitLabUserIds(env).has(userId)) {
    await env.OMNIS_OAUTH_KV.delete(OAUTH_SESSION_PREFIX + sessionId);
    return null;
  }

  return { sessionId, ...session };
}

async function startOAuth(env) {
  requireConfig(env);
  const state = crypto.randomUUID();
  await env.OMNIS_OAUTH_KV.put(
    "oauth:state:" + state,
    "1",
    { expirationTtl: OAUTH_STATE_TTL_SECONDS }
  );
  const params = new URLSearchParams({
    client_id: env.GITLAB_OAUTH_CLIENT_ID,
    redirect_uri: env.GITLAB_OAUTH_REDIRECT_URI,
    response_type: "code",
    state,
    scope: scopes(env)
  });
  return new Response(null, {
    status: 302,
    headers: {
      location: GITLAB_AUTHORIZE_URL + "?" + params.toString(),
      "cache-control": "no-store",
      "set-cookie": cookie(OAUTH_STATE_COOKIE, state, OAUTH_STATE_TTL_SECONDS)
    }
  });
}

async function exchangeCode(env, code) {
  const form = new URLSearchParams({
    client_id: env.GITLAB_OAUTH_CLIENT_ID,
    client_secret: env.GITLAB_OAUTH_CLIENT_SECRET,
    code,
    grant_type: "authorization_code",
    redirect_uri: env.GITLAB_OAUTH_REDIRECT_URI
  });
  const response = await fetch(GITLAB_TOKEN_URL, {
    method: "POST",
    headers: {
      "content-type": "application/x-www-form-urlencoded",
      accept: "application/json"
    },
    body: form
  });
  const payload = await response.json().catch(() => null);
  if (!response.ok || !payload?.access_token) {
    throw new Error("GitLab token exchange failed (" + response.status + ")");
  }
  return {
    access_token: payload.access_token,
    refresh_token: payload.refresh_token ?? null,
    token_type: payload.token_type ?? "bearer",
    created_at: Number(payload.created_at ?? Math.floor(Date.now() / 1000)),
    expires_at: Math.floor(Date.now() / 1000) + Number(payload.expires_in ?? 7200)
  };
}

async function refreshCredential(env, credential) {
  if (!credential.refresh_token) return credential;
  const form = new URLSearchParams({
    client_id: env.GITLAB_OAUTH_CLIENT_ID,
    client_secret: env.GITLAB_OAUTH_CLIENT_SECRET,
    refresh_token: credential.refresh_token,
    grant_type: "refresh_token",
    redirect_uri: env.GITLAB_OAUTH_REDIRECT_URI
  });
  const response = await fetch(GITLAB_TOKEN_URL, {
    method: "POST",
    headers: {
      "content-type": "application/x-www-form-urlencoded",
      accept: "application/json"
    },
    body: form
  });
  const payload = await response.json().catch(() => null);
  if (!response.ok || !payload?.access_token) {
    throw new Error("GitLab token refresh failed (" + response.status + ")");
  }
  return {
    access_token: payload.access_token,
    refresh_token: payload.refresh_token ?? null,
    token_type: payload.token_type ?? "bearer",
    created_at: Number(payload.created_at ?? Math.floor(Date.now() / 1000)),
    expires_at: Math.floor(Date.now() / 1000) + Number(payload.expires_in ?? 7200)
  };
}

async function getCredential(env) {
  const encrypted = await env.OMNIS_OAUTH_KV.get(TOKEN_KV_KEY);
  if (!encrypted) return null;
  const credential = await decryptJson(env, encrypted);
  const now = Math.floor(Date.now() / 1000);
  if (credential.expires_at && credential.expires_at - now <= TOKEN_REFRESH_SKEW_SECONDS) {
    const refreshed = await refreshCredential(env, credential);
    await env.OMNIS_OAUTH_KV.put(TOKEN_KV_KEY, await encryptJson(env, refreshed));
    return refreshed;
  }
  return credential;
}

async function handleCallback(request, env) {
  requireConfig(env);
  const url = new URL(request.url);
  const error = url.searchParams.get("error");
  const errorDescription = url.searchParams.get("error_description");
  if (error) {
    return html(
      "Omnis OAuth",
      "<h1>GitLab authorization not completed</h1><p>" +
        escapeHtml(errorDescription ?? error) +
        "</p>",
      400,
      { "set-cookie": deleteCookie(OAUTH_STATE_COOKIE) }
    );
  }

  const state = url.searchParams.get("state");
  const code = url.searchParams.get("code");
  const cookies = cookieMap(request);
  if (!state || !code || !cookies[OAUTH_STATE_COOKIE] || state !== cookies[OAUTH_STATE_COOKIE]) {
    return html(
      "Omnis OAuth",
      "<h1>Invalid OAuth state</h1><p>The authorization request could not be verified.</p>",
      400,
      { "set-cookie": deleteCookie(OAUTH_STATE_COOKIE) }
    );
  }

  const stateExists = await env.OMNIS_OAUTH_KV.get("oauth:state:" + state);
  await env.OMNIS_OAUTH_KV.delete("oauth:state:" + state);
  if (!stateExists) {
    return html(
      "Omnis OAuth",
      "<h1>Expired OAuth state</h1><p>Start the GitLab authorization again.</p>",
      400,
      { "set-cookie": deleteCookie(OAUTH_STATE_COOKIE) }
    );
  }

  const credential = await exchangeCode(env, code);
  const profileResponse = await fetch(GITLAB_API_BASE + "/user", {
    headers: {
      authorization: "Bearer " + credential.access_token,
      accept: "application/json"
    }
  });
  const profile = await profileResponse.json().catch(() => null);
  if (!profileResponse.ok || !profile?.id) {
    return html("Omnis OAuth", "<h1>GitLab profile verification failed</h1>", 502);
  }

  const gitLabUserId = String(profile.id);
  if (!allowedGitLabUserIds(env).has(gitLabUserId)) {
    return html(
      "Omnis OAuth",
      "<h1>This GitLab account is not authorized for runner administration.</h1>",
      403,
      { "set-cookie": deleteCookie(OAUTH_STATE_COOKIE) }
    );
  }

  const sessionId = crypto.randomUUID();
  await env.OMNIS_OAUTH_KV.put(TOKEN_KV_KEY, await encryptJson(env, credential));
  await env.OMNIS_OAUTH_KV.put(META_KV_KEY, JSON.stringify({
    gitlab_user_id: profile.id,
    username: profile.username ?? null,
    name: profile.name ?? null,
    connected_at: new Date().toISOString(),
    scopes: scopes(env)
  }));
  await env.OMNIS_OAUTH_KV.put(
    OAUTH_SESSION_PREFIX + sessionId,
    JSON.stringify({
      gitlab_user_id: gitLabUserId,
      username: profile.username ?? null,
      created_at: new Date().toISOString()
    }),
    { expirationTtl: OAUTH_SESSION_TTL_SECONDS }
  );

  const callbackHeaders = new Headers({
    location: "/runner",
    "cache-control": "no-store"
  });
  callbackHeaders.append("set-cookie", deleteCookie(OAUTH_STATE_COOKIE));
  callbackHeaders.append(
    "set-cookie",
    cookie(OAUTH_SESSION_COOKIE, sessionId, OAUTH_SESSION_TTL_SECONDS, "Lax")
  );
  return new Response(null, {
    status: 303,
    headers: callbackHeaders
  });
}

async function oauthStatus(env) {
  const missing = missingOAuthConfig(env);
  const credential = env.OMNIS_OAUTH_KV
    ? await env.OMNIS_OAUTH_KV.get(TOKEN_KV_KEY)
    : null;
  return json({
    service: "omnis-gitlab-oauth",
    enabled: true,
    configured: missing.length === 0,
    connected: Boolean(credential),
    scopes: scopes(env),
    redirect_uri: env.GITLAB_OAUTH_REDIRECT_URI ?? null
  }, missing.length === 0 ? 200 : 503);
}

async function runnerPage(request, env) {
  const session = await getRunnerSession(request, env);
  if (!session) {
    return html(
      "Omnis Runner",
      '<h1>GitLab authorization required</h1><p>Sign in with an allowlisted GitLab account before provisioning a runner.</p><p><a href="/oauth/start">Authorize GitLab</a></p>',
      401
    );
  }

  return new Response(
    '<!doctype html>\n<html lang="en">\n<head>\n' +
    '<meta charset="utf-8">\n<meta name="viewport" content="width=device-width,initial-scale=1">\n' +
    "<title>Omnis Runner Control</title>\n</head>\n<body>\n" +
    '<main style="font-family:system-ui,sans-serif;max-width:820px;margin:8vh auto;padding:24px">\n' +
    "<h1>Omnis Runner Control</h1>\n" +
    "<p>Project runner: <strong>omnis-ue58-linux</strong></p>\n" +
    "<p>Tag: <strong>omnis-ci-linux</strong></p>\n" +
    "<p>Target: Unreal Engine 5.8 / Linux x86_64</p>\n" +
    "<p>Registration credentials are not returned to this browser.</p>\n" +
    '<button id="provision" style="padding:12px 18px">Create project runner</button>\n' +
    '<button id="logout" style="padding:12px 18px;margin-left:8px">Sign out</button>\n' +
    '<pre id="output" style="white-space:pre-wrap;background:#111;color:#eee;padding:16px;margin-top:20px;border-radius:8px"></pre>\n' +
    "<script>\n" +
    'const button=document.getElementById("provision");\n' +
    'const logout=document.getElementById("logout");\n' +
    'const output=document.getElementById("output");\n' +
    'button.addEventListener("click",async()=>{\n' +
    '  button.disabled=true; output.textContent="Creating runner...";\n' +
    '  try{\n' +
    '    const response=await fetch("/runner/provision",{method:"POST",headers:{"Origin":location.origin,"Content-Type":"application/json"},body:"{}"});\n' +
    '    const data=await response.json();\n' +
    '    output.textContent=JSON.stringify(data,null,2);\n' +
    '  }catch(error){output.textContent="Runner provisioning request failed.";}\n' +
    '  finally{button.disabled=false;}\n' +
    '});\n' +
    'logout.addEventListener("click",async()=>{\n' +
    '  logout.disabled=true;\n' +
    '  try{await fetch("/runner/logout",{method:"POST",headers:{"Origin":location.origin,"Content-Type":"application/json"},body:"{}"});location.assign("/");}\n' +
    '  catch(error){output.textContent="Sign-out request failed.";logout.disabled=false;}\n' +
    '});\n' +
    "</script>\n" +
    "</main>\n</body>\n</html>",
    {
      status: 200,
      headers: {
        "content-type": "text/html; charset=utf-8",
        "cache-control": "no-store, max-age=0",
        "content-security-policy": "default-src 'none'; script-src 'unsafe-inline'; style-src 'unsafe-inline'; connect-src 'self'",
        "x-content-type-options": "nosniff",
        "referrer-policy": "no-referrer"
      }
    }
  );
}

async function provisionProjectRunner(request, env) {
  requireConfig(env);
  if (request.method !== "POST") return json({ error: "Method not allowed" }, 405);
  if (!isSameOriginBrowserRequest(request)) {
    return json({ error: "Same-origin request required" }, 403);
  }

  const session = await getRunnerSession(request, env);
  if (!session) return json({ error: "Authorized GitLab session required" }, 401);

  const existingCredential = await env.OMNIS_OAUTH_KV.get(RUNNER_CREDENTIAL_KV_KEY);
  const existingMeta = await env.OMNIS_OAUTH_KV.get(RUNNER_META_KV_KEY, "json");
  if (existingMeta?.id || existingCredential) {
    return json({
      service: "omnis-gitlab-runner",
      status: existingCredential ? "already_provisioned_pending_secure_delivery" : "runner_exists_token_unavailable",
      runner: existingMeta ?? null,
      token_delivery: existingCredential
        ? "Use the authenticated out-of-band delivery endpoint from a trusted terminal."
        : "Do not create a duplicate runner. Revoke or recover the existing runner through GitLab administration."
    });
  }

  const credential = await getCredential(env);
  if (!credential) {
    return json({ error: "GitLab OAuth is not connected" }, 401);
  }

  const form = new URLSearchParams({
    runner_type: "project_type",
    project_id: OMNIS_PROJECT_ID,
    description: "omnis-ue58-linux",
    tag_list: "omnis-ci-linux",
    run_untagged: "false",
    locked: "true",
    access_level: "ref_protected",
    paused: "false",
    maintenance_note: "Omnis Unreal Engine 5.8 Linux runner"
  });

  const response = await fetch(GITLAB_API_BASE + "/user/runners", {
    method: "POST",
    headers: {
      authorization: "Bearer " + credential.access_token,
      "content-type": "application/x-www-form-urlencoded",
      accept: "application/json"
    },
    body: form
  });

  const payload = await response.json().catch(() => null);
  if (!response.ok || !payload?.token || !payload?.id) {
    return json({
      error: "GitLab runner provisioning failed",
      upstream_status: response.status
    }, 502);
  }

  const createdAt = new Date().toISOString();
  await env.OMNIS_OAUTH_KV.put(
    RUNNER_CREDENTIAL_KV_KEY,
    await encryptJson(env, { id: payload.id, token: payload.token }),
    { expirationTtl: RUNNER_TOKEN_TTL_SECONDS }
  );
  const runnerMeta = {
    id: payload.id,
    description: "omnis-ue58-linux",
    tags: ["omnis-ci-linux"],
    project_id: Number(OMNIS_PROJECT_ID),
    requested_by_gitlab_user_id: String(session.gitlab_user_id),
    created_at: createdAt,
    token_delivery_expires_at: new Date(Date.now() + RUNNER_TOKEN_TTL_SECONDS * 1000).toISOString()
  };
  await env.OMNIS_OAUTH_KV.put(RUNNER_META_KV_KEY, JSON.stringify(runnerMeta));

  return json({
    service: "omnis-gitlab-runner",
    status: "provisioned",
    runner: runnerMeta,
    token_delivery: "A short-lived authenticated terminal request is required. The browser response never contains the runner token."
  });
}

async function runnerStatus(request, env) {
  requireConfig(env);
  const session = await getRunnerSession(request, env);
  if (!session) return json({ error: "Authorized GitLab session required" }, 401);

  const credential = await getCredential(env);
  if (!credential) return json({ service: "omnis-gitlab-runner", connected: false }, 401);

  const response = await fetch(
    GITLAB_API_BASE + "/projects/" + OMNIS_PROJECT_ID + "/runners?type=project_type",
    {
      headers: {
        authorization: "Bearer " + credential.access_token,
        accept: "application/json"
      }
    }
  );
  const payload = await response.json().catch(() => null);
  if (!response.ok) {
    return json({ error: "GitLab runner status failed", upstream_status: response.status }, 502);
  }

  return json({
    service: "omnis-gitlab-runner",
    connected: true,
    project_id: Number(OMNIS_PROJECT_ID),
    runners: Array.isArray(payload)
      ? payload.map((runner) => ({
          id: runner.id,
          description: runner.description,
          status: runner.status,
          paused: runner.paused,
          online: runner.status === "online",
          tags: runner.tag_list ?? []
        }))
      : []
  });
}

async function logoutRunnerSession(request, env) {
  if (request.method !== "POST") return json({ error: "Method not allowed" }, 405);
  if (!isSameOriginBrowserRequest(request)) {
    return json({ error: "Same-origin request required" }, 403);
  }
  const session = await getRunnerSession(request, env);
  if (session) {
    await env.OMNIS_OAUTH_KV.delete(OAUTH_SESSION_PREFIX + session.sessionId);
  }
  return json(
    { service: "omnis-gitlab-oauth", signed_out: true },
    200,
    { "set-cookie": deleteCookie(OAUTH_SESSION_COOKIE, "Strict") }
  );
}

async function deliverRunnerRegistrationToken(request, env) {
  if (!oauthEnabled(env)) return json({ error: "Runner Control is disabled" }, 503);
  if (request.method !== "POST") return json({ error: "Method not allowed" }, 405);
  if (request.headers.has("origin") || request.headers.has("cookie")) {
    return json({ error: "Machine-to-machine delivery only" }, 403);
  }

  const deliverySecret = env.OMNIS_RUNNER_DELIVERY_SECRET;
  if (!deliverySecret || deliverySecret.length < 32) {
    return json({ error: "Runner token delivery is not configured" }, 503);
  }
  const authorization = request.headers.get("authorization") ?? "";
  if (!constantTimeEqual(authorization, "Bearer " + deliverySecret)) {
    return json({ error: "Runner delivery authorization failed" }, 401);
  }

  if (!env.OMNIS_OAUTH_KV) {
    return json({ error: "Runner credential storage is unavailable" }, 503);
  }
  const encrypted = await env.OMNIS_OAUTH_KV.get(RUNNER_CREDENTIAL_KV_KEY);
  if (!encrypted) {
    return json({ error: "No unexpired runner registration token is available" }, 404);
  }

  const runnerCredential = await decryptJson(env, encrypted);
  if (!runnerCredential?.id || !runnerCredential?.token) {
    return json({ error: "Stored runner credential is invalid" }, 500);
  }

  return json({
    service: "omnis-gitlab-runner",
    runner_id: runnerCredential.id,
    registration_token: runnerCredential.token,
    expires_in_seconds: RUNNER_TOKEN_TTL_SECONDS,
    warning: "Store this response only in a protected secret store. Do not print it or include it in logs, tickets, screenshots, or shell history."
  });
}

export default {
  async fetch(request, env) {
    const url = new URL(request.url);
    const environment = env.OMNIS_ENVIRONMENT ?? "unknown";

    try {
      if (url.pathname === "/health") {
        const missing = missingOAuthConfig(env);
        return json({
          service: "omnis-cloudflare-worker",
          game: "Omnis: El Último Ciclo",
          environment,
          status: "ok",
          oauth: {
            enabled: oauthEnabled(env),
            configured: missing.length === 0,
            runner_token_delivery_ready: Boolean(
              env.OMNIS_RUNNER_DELIVERY_SECRET &&
              env.OMNIS_RUNNER_DELIVERY_SECRET.length >= 32
            )
          }
        });
      }

      if (url.pathname === "/runner/registration-token" && request.method === "POST") {
        return await deliverRunnerRegistrationToken(request, env);
      }

      if (oauthEnabled(env) && url.pathname === "/oauth/start" && request.method === "GET") {
        return await startOAuth(env);
      }

      if (oauthEnabled(env) && url.pathname === "/oauth/callback" && request.method === "GET") {
        return await handleCallback(request, env);
      }

      if (url.pathname === "/oauth/status" && request.method === "GET") {
        if (!oauthEnabled(env)) {
          return json({ service: "omnis-gitlab-oauth", enabled: false, environment });
        }
        return await oauthStatus(env);
      }

      if (oauthEnabled(env) && url.pathname === "/runner" && request.method === "GET") {
        return await runnerPage(request, env);
      }

      if (oauthEnabled(env) && url.pathname === "/runner/provision" && request.method === "POST") {
        return await provisionProjectRunner(request, env);
      }

      if (oauthEnabled(env) && url.pathname === "/runner/status" && request.method === "GET") {
        return await runnerStatus(request, env);
      }

      if (oauthEnabled(env) && url.pathname === "/runner/logout" && request.method === "POST") {
        return await logoutRunnerSession(request, env);
      }

      if (url.pathname === "/") {
        return json({
          name: "Omnis: El Último Ciclo",
          service: "Cloudflare Worker",
          environment,
          status: "online",
          oauth: oauthEnabled(env)
            ? {
                authorize: "/oauth/start",
                callback: "/oauth/callback",
                status: "/oauth/status"
              }
            : { enabled: false }
        });
      }

      return new Response("Omnis Worker: Not Found", {
        status: 404,
        headers: {
          "content-type": "text/plain; charset=utf-8",
          "cache-control": "no-store"
        }
      });
    } catch (error) {
      console.error(JSON.stringify({
        event: "worker_error",
        path: url.pathname,
        message: error instanceof Error ? error.message : "Unknown error"
      }));
      return json({ error: "Internal server error" }, 500);
    }
  },

  async queue(batch) {
    for (const message of batch.messages) {
      console.log(JSON.stringify({
        event: "queue_message_processed",
        queue: batch.queue,
        messageId: message.id,
        attempts: Number.isInteger(message.attempts) ? message.attempts : null,
        timestamp: message.timestamp instanceof Date
          ? message.timestamp.toISOString()
          : null
      }));
    }
  }
};
