export default {
  async fetch(request) {
    const url = new URL(request.url);

    if (url.pathname === "/health") {
      return Response.json({
        service: "omnis-cloudflare-worker",
        game: "Omnis: El Último Ciclo",
        status: "ok"
      });
    }

    if (url.pathname === "/") {
      return Response.json({
        name: "Omnis: El Último Ciclo",
        service: "Cloudflare Worker",
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
