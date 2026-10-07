const GITLAB_AUTHORIZE_URL = "https://gitlab.com/oauth/authorize";
const GITLAB_TOKEN_URL = "https://gitlab.com/oauth/token";
const GITLAB_API_BASE = "https://gitlab.com/api/v4";
const OAUTH_STATE_TTL_SECONDS = 600;
const TOKEN_REFRESH_SKEW_SECONDS = 300;
const OAUTH_STATE_COOKIE = "omnis_oauth_state";
const OAUTH_CONNECTED_COOKIE = "omnis_oauth_connected";
const TOKEN_KV_KEY = "gitlab:oauth:credential";
const META_KV_KEY = "gitlab:oauth:metadata";

function json(data, status = 200, extra = {}) {
  return new Response(JSON.stringify(data, null, 2), {
    status,
    headers: {
      "content-type": "application/json; charset=utf-8",
      "cache-control": "no-store",
      "x-content-type-options": "nosniff",
      "referrer-policy": "no-referrer",
      ...extra
    }
  });
}

function html(title, body, status = 200) {
  return new Response(`<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>${title}</title>
</head>
<body>
<main style="font-family:system-ui,sans-serif;max-width:720px;margin:10vh auto;padding:24px">
${body}
</main>
</body>
</html>`, {
    status,
    headers: {
      "content-type": "text/html; charset=utf-8",
      "cache-control": "no-store",
      "content-security-policy": "default-src 'none'; style-src 'unsafe-inline'",
      "x-content-type-options": "nosniff",
      "referrer-policy": "no-referrer"
    }
  });
}

function requireConfig(env) {
  const missing = [];
  if (!env.GITLAB_OAUTH_CLIENT_ID) missing.push("GITLAB_OAUTH_CLIENT_ID");
  if (!env.GITLAB_OAUTH_CLIENT_SECRET) missing.push("GITLAB_OAUTH_CLIENT_SECRET");
  if (!env.OMNIS_OAUTH_ENCRYPTION_KEY) missing.push("OMNIS_OAUTH_ENCRYPTION_KEY");
  if (!env.OMNIS_OAUTH_KV) missing.push("OMNIS_OAUTH_KV");
  if (!env.GITLAB_OAUTH_REDIRECT_URI) missing.push("GITLAB_OAUTH_REDIRECT_URI");
  if (missing.length) throw new Error(`OAuth configuration incomplete: ${missing.join(", ")}`);
}

function cookieMap(request) {
  const out = {};
  for (const part of (request.headers.get("cookie") ?? "").split(";")) {
    const i = part.indexOf("=");
    if (i === -1) continue;
    out[part.slice(0, i).trim()] = decodeURIComponent(part.slice(i + 1).trim());
  }
  return out;
}

function cookie(name, value, maxAgeSeconds) {
  return `${name}=${encodeURIComponent(value)}; Max-Age=${maxAgeSeconds}; Path=/; HttpOnly; Secure; SameSite=Lax`;
}

function deleteCookie(name) {
  return `${name}=; Max-Age=0; Path=/; HttpOnly; Secure; SameSite=Lax`;
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
  if (raw.byteLength !== 32) throw new Error("OMNIS_OAUTH_ENCRYPTION_KEY must decode to exactly 32 bytes");
  return crypto.subtle.importKey("raw", raw, "AES-GCM", false, ["encrypt", "decrypt"]);
}

async function encryptJson(env, value) {
  const key = await encryptionKey(env);
  const iv = crypto.getRandomValues(new Uint8Array(12));
  const plaintext = new TextEncoder().encode(JSON.stringify(value));
  const ciphertext = new Uint8Array(await crypto.subtle.encrypt({ name: "AES-GCM", iv }, key, plaintext));
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

async function startOAuth(env) {
  requireConfig(env);
  const state = crypto.randomUUID();
  await env.OMNIS_OAUTH_KV.put(`oauth:state:${state}`, "1", { expirationTtl: OAUTH_STATE_TTL_SECONDS });
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
      location: `${GITLAB_AUTHORIZE_URL}?${params.toString()}`,
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
    headers: { "content-type": "application/x-www-form-urlencoded", accept: "application/json" },
    body: form
  });
  const payload = await response.json().catch(() => null);
  if (!response.ok || !payload?.access_token) {
    throw new Error(`GitLab token exchange failed (${response.status})`);
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
    headers: { "content-type": "application/x-www-form-urlencoded", accept: "application/json" },
    body: form
  });
  const payload = await response.json().catch(() => null);
  if (!response.ok || !payload?.access_token) {
    throw new Error(`GitLab token refresh failed (${response.status})`);
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
  if (error) return html("Omnis OAuth", `<h1>GitLab authorization not completed</h1><p>${errorDescription ?? error}</p>`, 400);

  const state = url.searchParams.get("state");
  const code = url.searchParams.get("code");
  const cookies = cookieMap(request);
  if (!state || !code || !cookies[OAUTH_STATE_COOKIE] || state !== cookies[OAUTH_STATE_COOKIE]) {
    return html("Omnis OAuth", "<h1>Invalid OAuth state</h1><p>The authorization request could not be verified.</p>", 400);
  }

  const stateExists = await env.OMNIS_OAUTH_KV.get(`oauth:state:${state}`);
  await env.OMNIS_OAUTH_KV.delete(`oauth:state:${state}`);
  if (!stateExists) {
    return html("Omnis OAuth", "<h1>Expired OAuth state</h1><p>Start the GitLab authorization again.</p>", 400);
  }

  const credential = await exchangeCode(env, code);
  const profileResponse = await fetch(`${GITLAB_API_BASE}/user`, {
    headers: { authorization: `Bearer ${credential.access_token}`, accept: "application/json" }
  });
  const profile = await profileResponse.json().catch(() => null);
  if (!profileResponse.ok || !profile?.id) {
    return html("Omnis OAuth", "<h1>GitLab profile verification failed</h1>", 502);
  }

  await env.OMNIS_OAUTH_KV.put(TOKEN_KV_KEY, await encryptJson(env, credential));
  await env.OMNIS_OAUTH_KV.put(META_KV_KEY, JSON.stringify({
    gitlab_user_id: profile.id,
    username: profile.username ?? null,
    name: profile.name ?? null,
    connected_at: new Date().toISOString(),
    scopes: scopes(env)
  }));

  return new Response(null, {
    status: 303,
    headers: {
      location: "/oauth/status",
      "cache-control": "no-store",
      "set-cookie": [
        deleteCookie(OAUTH_STATE_COOKIE),
        cookie(OAUTH_CONNECTED_COOKIE, "1", 3600)
      ]
    }
  });
}

async function oauthStatus(env) {
  requireConfig(env);
  const metadata = await env.OMNIS_OAUTH_KV.get(META_KV_KEY, "json");
  const credential = await env.OMNIS_OAUTH_KV.get(TOKEN_KV_KEY);
  return json({
    service: "omnis-gitlab-oauth",
    configured: true,
    connected: Boolean(credential),
    user: metadata
      ? { id: metadata.gitlab_user_id, username: metadata.username, name: metadata.name }
      : null,
    scopes: scopes(env),
    redirect_uri: env.GITLAB_OAUTH_REDIRECT_URI
  });
}

export default {
  async fetch(request, env) {
    const url = new URL(request.url);
    const environment = env.OMNIS_ENVIRONMENT ?? "unknown";

    try {
      if (url.pathname === "/health") {
        return json({
          service: "omnis-cloudflare-worker",
          game: "Omnis: El Último Ciclo",
          environment,
          status: "ok",
          oauth: {
            configured: Boolean(
              env.GITLAB_OAUTH_CLIENT_ID &&
              env.GITLAB_OAUTH_CLIENT_SECRET &&
              env.OMNIS_OAUTH_ENCRYPTION_KEY &&
              env.OMNIS_OAUTH_KV &&
              env.GITLAB_OAUTH_REDIRECT_URI
            )
          }
        });
      }

      if (url.pathname === "/oauth/start" && request.method === "GET") {
        return await startOAuth(env);
      }

      if (url.pathname === "/oauth/callback" && request.method === "GET") {
        return await handleCallback(request, env);
      }

      if (url.pathname === "/oauth/status" && request.method === "GET") {
        return await oauthStatus(env);
      }

      if (url.pathname === "/") {
        return json({
          name: "Omnis: El Último Ciclo",
          service: "Cloudflare Worker",
          environment,
          status: "online",
          oauth: {
            authorize: "/oauth/start",
            callback: "/oauth/callback",
            status: "/oauth/status"
          }
        });
      }

      return new Response("Omnis Worker: Not Found", {
        status: 404,
        headers: { "content-type": "text/plain; charset=utf-8", "cache-control": "no-store" }
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
        event: "queue_message",
        queue: batch.queue,
        messageId: message.id,
        body: message.body
      }));
    }
  }
};
