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

function notFound() {
  return new Response("Not Found", {
    status: 404,
    headers: {
      ...SECURITY_HEADERS,
      "content-type": "text/plain; charset=utf-8"
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

export default {
  async fetch(request, env) {
    const url = new URL(request.url);

    if (request.method !== "GET" && request.method !== "HEAD") {
      return new Response("Method Not Allowed", {
        status: 405,
        headers: {
          ...SECURITY_HEADERS,
          Allow: "GET, HEAD",
          "content-type": "text/plain; charset=utf-8"
        }
      });
    }

    if (url.pathname === "/health") {
      const db = await supabaseHealth(env);
      return json({
        service: "omnis-cloudflare-worker",
        game: "Omnis: El Último Ciclo",
        status: db.ok ? "ok" : "degraded",
        supabase: db.ok ? "ok" : "unavailable"
      }, db.ok ? 200 : 503);
    }

    if (url.pathname === "/") {
      return json({
        name: "Omnis: El Último Ciclo",
        service: "Cloudflare Worker",
        status: "online"
      });
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
