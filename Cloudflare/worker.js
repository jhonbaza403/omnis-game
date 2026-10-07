export default {
  async fetch(request, env) {
    const url = new URL(request.url);
    const environment = env.OMNIS_ENVIRONMENT ?? "unknown";

    if (url.pathname === "/health") {
      return Response.json({
        service: "omnis-cloudflare-worker",
        game: "Omnis: El Último Ciclo",
        environment,
        status: "ok"
      });
    }

    if (url.pathname === "/") {
      return Response.json( {
        name: "Omnis: El Último Ciclo",
        service: "Cloudflare Worker",
        environment,
        status: "online"
      });
    }

    return new Response("Omnis Worker: Not Found", {
      status: 404,
      headers: { "content-type": "text/plain; charset=utf-8" }
    });
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
