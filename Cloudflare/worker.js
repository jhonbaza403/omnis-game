const SECURITY_HEADERS = {
  "Content-Security-Policy": "default-src 'none'; frame-ancestors 'none'; base-uri 'none'",
  "Referrer-Policy": "no-referrer",
  "X-Content-Type-Options": "nosniff",
  "X-Frame-Options": "DENY",
  "Permissions-Policy": "camera=(), microphone=(), geolocation=()",
  "Cache-Control": "no-store"
};

function json(data, status = 200) {
  return Response.json(data, {
    status,
    headers: SECURITY_HEADERS
  });
}

function textResponse(message, status, allow) {
  return new Response(message, {
    status,
    headers: {
      ...SECURITY_HEADERS,
      Allow: allow,
      "content-type": "text/plain; charset=utf-8"
    }
  });
}

function notFound() {
  return textResponse("Not Found", 404, "GET, HEAD");
}

function getBearerToken(request) {
  const value = request.headers.get("authorization") || "";
  if (!value.startsWith("Bearer ")) {
    return null;
  }

  const token = value.slice(7).trim();
  return token || null;
}

async function parseJsonBody(request, maxBytes = 32768) {
  const raw = await request.text();
  if (new TextEncoder().encode(raw).byteLength > maxBytes) {
    const error = new Error("payload_too_large");
    error.status = 413;
    throw error;
  }

  try {
    return JSON.parse(raw);
  } catch {
    const error = new Error("invalid_json");
    error.status = 400;
    throw error;
  }
}

async function supabaseRequest(env, path, method, body = null, bearer = null) {
  const headers = {
    apikey: env.SUPABASE_PUBLISHABLE_KEY
  };

  if (bearer) {
    headers.authorization = "Bearer " + bearer;
  }

  if (body !== null) {
    headers["content-type"] = "application/json";
  }

  return fetch(new URL(path, env.SUPABASE_URL), {
    method,
    headers,
    body: body === null ? undefined : JSON.stringify(body)
  });
}

async function requireUser(env, request) {
  const bearer = getBearerToken(request);
  if (!bearer) {
    return { ok: false, response: json({ error: "authentication_required" }, 401) };
  }

  const response = await supabaseRequest(env, "/auth/v1/user", "GET", null, bearer);
  if (!response.ok) {
    return { ok: false, response: json({ error: "invalid_session" }, 401) };
  }

  const user = await response.json();
  if (!user || typeof user.id !== "string") {
    return { ok: false, response: json({ error: "invalid_identity" }, 401) };
  }

  return { ok: true, bearer, user };
}

async function relaySupabase(response) {
  const payload = await response.text();
  const contentType = response.headers.get("content-type") || "application/json; charset=utf-8";

  return new Response(payload, {
    status: response.status,
    headers: {
      ...SECURITY_HEADERS,
      "content-type": contentType
    }
  });
}

async function supabaseHealth(env) {
  if (!env.SUPABASE_URL || !env.SUPABASE_PUBLISHABLE_KEY) {
    return { ok: false, status: 503 };
  }

  const response = await fetch(new URL("/auth/v1/settings", env.SUPABASE_URL), {
    method: "GET",
    headers: {
      apikey: env.SUPABASE_PUBLISHABLE_KEY
    }
  });

  return { ok: response.ok, status: response.status };
}

async function handleApi(request, env, url) {
  const session = await requireUser(env, request);
  if (!session.ok) {
    return session.response;
  }

  const bearer = session.bearer;

  if (url.pathname === "/v1/profile") {
    if (request.method === "GET") {
      return relaySupabase(await supabaseRequest(
        env,
        "/rest/v1/game_profiles?select=user_id,display_name,created_at,updated_at",
        "GET",
        null,
        bearer
      ));
    }

    if (request.method === "PUT") {
      const body = await parseJsonBody(request);
      const displayName = typeof body.display_name === "string" ? body.display_name.trim() : "";
      if (displayName.length < 2 || displayName.length > 80) {
        return json({ error: "invalid_display_name" }, 422);
      }

      return relaySupabase(await supabaseRequest(
        env,
        "/rest/v1/game_profiles?on_conflict=user_id",
        "POST",
        { user_id: session.user.id, display_name: displayName },
        bearer
      ));
    }

    return textResponse("Method Not Allowed", 405, "GET, PUT");
  }

  if (url.pathname === "/v1/progression") {
    if (request.method !== "GET") {
      return textResponse("Method Not Allowed", 405, "GET");
    }

    return relaySupabase(await supabaseRequest(
      env,
      "/rest/v1/player_progression?select=user_id,level,experience,cycle_count,nexus_level,updated_at",
      "GET",
      null,
      bearer
    ));
  }

  const saveMatch = url.pathname.match(/^\/v1\/saves\/([0-9]+)$/);
  if (saveMatch) {
    const slot = Number(saveMatch[1]);
    if (!Number.isInteger(slot) || slot < 1 || slot > 10) {
      return json({ error: "invalid_slot" }, 422);
    }

    if (request.method === "GET") {
      return relaySupabase(await supabaseRequest(
        env,
        "/rest/v1/player_saves?select=id,slot,save_version,state,created_at,updated_at&slot=eq." + slot,
        "GET",
        null,
        bearer
      ));
    }

    if (request.method === "PUT") {
      const body = await parseJsonBody(request);
      if (!body.state || typeof body.state !== "object" || Array.isArray(body.state)) {
        return json({ error: "invalid_state" }, 422);
      }

      const saveVersion = Number(body.save_version ?? 1);
      if (!Number.isInteger(saveVersion) || saveVersion < 1) {
        return json({ error: "invalid_save_version" }, 422);
      }

      return relaySupabase(await supabaseRequest(
        env,
        "/rest/v1/player_saves?on_conflict=user_id,slot",
        "POST",
        {
          user_id: session.user.id,
          slot,
          save_version: saveVersion,
          state: body.state
        },
        bearer
      ));
    }

    if (request.method === "DELETE") {
      return relaySupabase(await supabaseRequest(
        env,
        "/rest/v1/player_saves?slot=eq." + slot,
        "DELETE",
        null,
        bearer
      ));
    }

    return textResponse("Method Not Allowed", 405, "GET, PUT, DELETE");
  }

  if (url.pathname === "/v1/events") {
    if (request.method !== "POST") {
      return textResponse("Method Not Allowed", 405, "POST");
    }

    const body = await parseJsonBody(request, 16384);
    const eventType = typeof body.event_type === "string" ? body.event_type.trim() : "";
    const eventVersion = Number(body.event_version ?? 1);

    if (eventType.length < 1 || eventType.length > 120) {
      return json({ error: "invalid_event_type" }, 422);
    }

    if (!Number.isInteger(eventVersion) || eventVersion < 1) {
      return json({ error: "invalid_event_version" }, 422);
    }

    if (!body.payload || typeof body.payload !== "object" || Array.isArray(body.payload)) {
      return json({ error: "invalid_event_payload" }, 422);
    }

    return relaySupabase(await supabaseRequest(
      env,
      "/rest/v1/game_events",
      "POST",
      {
        user_id: session.user.id,
        event_type: eventType,
        event_version: eventVersion,
        payload: body.payload
      },
      bearer
    ));
  }

  return notFound();
}

export default {
  async fetch(request, env) {
    const url = new URL(request.url);

    if (url.pathname === "/health") {
      if (request.method !== "GET" && request.method !== "HEAD") {
        return textResponse("Method Not Allowed", 405, "GET, HEAD");
      }

      const db = await supabaseHealth(env);
      return json({
        service: "omnis-cloudflare-worker",
        game: "Omnis: El Último Ciclo",
        status: db.ok ? "ok" : "degraded",
        supabase: db.ok ? "ok" : "unavailable"
      }, db.ok ? 200 : 503);
    }

    if (url.pathname === "/") {
      if (request.method !== "GET" && request.method !== "HEAD") {
        return textResponse("Method Not Allowed", 405, "GET, HEAD");
      }

      return json({
        name: "Omnis: El Último Ciclo",
        service: "Cloudflare Worker",
        api: "/v1",
        status: "online"
      });
    }

    if (url.pathname.startsWith("/v1/")) {
      try {
        return await handleApi(request, env, url);
      } catch (error) {
        const status = Number(error?.status) || 500;
        if (status === 413) {
          return json({ error: "payload_too_large" }, 413);
        }
        if (status === 400) {
          return json({ error: "invalid_json" }, 400);
        }
        console.error(JSON.stringify({ event: "api_error", status }));
        return json({ error: "internal_error" }, 500);
      }
    }

    return notFound();
  },

  async queue(batch) {
    for (const message of batch.messages) {
      console.log(JSON.stringify({
        event: "queue_message",
        queue: batch.queue,
        messageId: message.id
      }));
    }
  }
};
