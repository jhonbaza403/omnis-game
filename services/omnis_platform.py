#!/usr/bin/env python3
import json
import os
import secrets
import socket
import time
from datetime import datetime, timezone
from http.client import HTTPConnection, HTTPSConnection
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from http import HTTPStatus
from urllib.parse import urlsplit

ROLE = os.getenv("SERVICE_ROLE", "gateway")
HOST = os.getenv("LISTEN_HOST", "0.0.0.0")
PORT = int(os.getenv("LISTEN_PORT", "8080"))
SERVICE_TOKEN = os.getenv("OMNIS_SERVICE_TOKEN", "").strip()
REDIS_ADDR = os.getenv("REDIS_ADDR", "redis:6379")
REDIS_PASSWORD = os.getenv("REDIS_PASSWORD", "")

def env(name, fallback):
    value = os.getenv(name, "").strip()
    return value or fallback

def now():
    return datetime.now(timezone.utc).isoformat()

def new_id():
    return secrets.token_hex(16)

def json_bytes(value):
    return json.dumps(value, separators=(",", ":")).encode()

class RedisError(RuntimeError):
    pass

class RedisClient:
    def __init__(self, address):
        host, port = address.rsplit(":", 1)
        self.host = host
        self.port = int(port)

    def command(self, *args):
        with socket.create_connection((self.host, self.port), timeout=3) as sock:
            if REDIS_PASSWORD:
                self._write(sock, "AUTH", REDIS_PASSWORD)
                self._read(sock)
            self._write(sock, *args)
            return self._read(sock)

    @staticmethod
    def _write(sock, *args):
        payload = [f"*{len(args)}\r\n".encode()]
        for arg in args:
            raw = str(arg).encode()
            payload.extend([b"$" + str(len(raw)).encode() + b"\r\n", raw, b"\r\n"])
        sock.sendall(b"".join(payload))

    @classmethod
    def _read(cls, sock):
        prefix = sock.recv(1)
        if not prefix:
            raise RedisError("redis connection closed")
        if prefix in (b"+", b"-", b":"):
            line = cls._readline(sock)
            if prefix == b"-":
                raise RedisError(line.decode())
            return int(line) if prefix == b":" else line.decode()
        if prefix == b"$":
            size = int(cls._readline(sock))
            if size < 0:
                return None
            data = cls._readn(sock, size)
            cls._readn(sock, 2)
            return data.decode()
        if prefix == b"*":
            size = int(cls._readline(sock))
            if size < 0:
                return None
            return [cls._read(sock) for _ in range(size)]
        raise RedisError(f"unknown redis response: {prefix!r}")

    @staticmethod
    def _readline(sock):
        out = bytearray()
        while True:
            ch = sock.recv(1)
            if not ch:
                raise RedisError("redis connection closed")
            if ch == b"\n":
                return bytes(out[:-1]) if out.endswith(b"\r") else bytes(out)
            out.extend(ch)

    @staticmethod
    def _readn(sock, size):
        out = bytearray()
        while len(out) < size:
            chunk = sock.recv(size - len(out))
            if not chunk:
                raise RedisError("redis connection closed")
            out.extend(chunk)
        return bytes(out)

    def ping(self):
        return self.command("PING")

    def get(self, key):
        return self.command("GET", key)

    def set(self, key, value, ttl=None):
        if ttl:
            return self.command("SET", key, value, "EX", str(ttl))
        return self.command("SET", key, value)

    def incr(self, key):
        return int(self.command("INCR", key))

    def sadd(self, key, value):
        return int(self.command("SADD", key, value))

    def srem(self, key, value):
        return int(self.command("SREM", key, value))

    def sismember(self, key, value):
        return int(self.command("SISMEMBER", key, value)) == 1

    def rpush(self, key, value):
        return int(self.command("RPUSH", key, value))

    def lrem(self, key, count, value):
        return int(self.command("LREM", key, count, value))

    def lpos(self, key, value):
        return self.command("LPOS", key, value)

    def lpop2(self, key):
        return self.command("LPOP", key, 2) or []

redis = RedisClient(REDIS_ADDR)

def require_service(handler):
    if SERVICE_TOKEN and handler.headers.get("X-Omnis-Service-Token", "") != SERVICE_TOKEN:
        handler.send_json(HTTPStatus.UNAUTHORIZED, {"error": "service authentication failed"})
        return False
    return True

def player_id(handler):
    value = handler.headers.get("X-Omnis-Player-Id", "").strip()
    if not value or len(value) > 128:
        return None
    return value

class Handler(BaseHTTPRequestHandler):
    server_version = "OmnisPlatform/1.0"

    def log_message(self, fmt, *args):
        print(json.dumps({"role": ROLE, "message": fmt % args}), flush=True)

    def send_json(self, status, value):
        body = json_bytes(value)
        self.send_response(int(status))
        self.send_header("Content-Type", "application/json")
        self.send_header("Content-Length", str(len(body)))
        self.send_header("X-Request-ID", self.headers.get("X-Request-ID", new_id()))
        self.end_headers()
        self.wfile.write(body)

    def body_json(self):
        length = int(self.headers.get("Content-Length", "0"))
        raw = self.rfile.read(length) if length else b"{}"
        return json.loads(raw.decode())

    def do_GET(self):
        self.route()

    def do_POST(self):
        self.route()

    def do_DELETE(self):
        self.route()

    def route(self):
        try:
            if self.path == "/healthz":
                self.send_json(200, {"status": "ok", "service": ROLE})
                return
            if self.path == "/readyz":
                if ROLE != "gateway":
                    redis.ping()
                self.send_json(200, {"status": "ready", "service": ROLE})
                return
            if ROLE == "gateway":
                self.gateway()
                return
            if not require_service(self):
                return
            if ROLE == "matchmaking":
                self.matchmaking()
                return
            if ROLE == "session":
                self.session()
                return
            if ROLE == "world":
                self.world()
                return
            self.send_json(500, {"error": "unknown service role"})
        except (RedisError, OSError, ValueError, json.JSONDecodeError) as exc:
            self.send_json(503, {"error": str(exc)})

    def gateway(self):
        path = self.path.split("?", 1)[0]
        service_authenticated = bool(
            SERVICE_TOKEN and
            self.headers.get("X-Omnis-Service-Token", "") == SERVICE_TOKEN
        )
        if (
            path.startswith("/api/matchmaking")
            or path.startswith("/api/sessions")
            or (path.startswith("/api/world") and self.command != "GET")
        ):
            if not service_authenticated and not player_id(self):
                self.send_json(401, {"error": "X-Omnis-Player-Id is required"})
                return
        routes = (
            ("/api/matchmaking", env("MATCHMAKING_URL", "http://matchmaking:8080"), "/v1/matchmaking"),
            ("/api/sessions", env("SESSION_URL", "http://session:8080"), "/v1/sessions"),
            ("/api/world", env("WORLD_URL", "http://world:8080"), "/v1/world"),
        )
        for prefix, target, target_prefix in routes:
            if path == prefix or path.startswith(prefix + "/"):
                self.proxy(target, target_prefix + path[len(prefix):])
                return
        self.send_json(404, {"error": "route not found"})

    def proxy(self, target, target_path):
        parsed = urlsplit(target)
        connection_cls = HTTPSConnection if parsed.scheme == "https" else HTTPConnection
        conn = connection_cls(
            parsed.hostname,
            parsed.port or (443 if parsed.scheme == "https" else 80),
            timeout=5,
        )
        headers = {
            k: v
            for k, v in self.headers.items()
            if k.lower() not in {"host", "content-length"}
        }
        if SERVICE_TOKEN:
            headers["X-Omnis-Service-Token"] = SERVICE_TOKEN
        length = int(self.headers.get("Content-Length", "0"))
        body = self.rfile.read(length) if length else None
        query = ("?" + self.path.split("?", 1)[1]) if "?" in self.path else ""
        conn.request(self.command, target_path + query, body=body, headers=headers)
        response = conn.getresponse()
        data = response.read()
        self.send_response(response.status)
        for key, value in response.getheaders():
            if key.lower() not in {"transfer-encoding", "connection"}:
                self.send_header(key, value)
        self.send_header("Content-Length", str(len(data)))
        self.end_headers()
        self.wfile.write(data)
        conn.close()

    def matchmaking(self):
        path = self.path.split("?", 1)[0]
        player = player_id(self)
        if path == "/v1/matchmaking/enqueue" and self.command == "POST":
            payload = self.body_json()
            playlist = str(payload.get("playlist", "")).strip()
            if not playlist or len(playlist) > 64 or "\n" in playlist or "\r" in playlist:
                self.send_json(400, {"error": "playlist is required"})
                return
            queue = f"omnis:mm:{playlist}:queue"
            members = f"omnis:mm:{playlist}:members"
            added = redis.sadd(members, player)
            if added:
                redis.rpush(queue, player)
            position = redis.lpos(queue, player)
            self.send_json(
                202,
                {
                    "queued": True,
                    "playlist": playlist,
                    "position": (int(position) + 1) if position is not None else 1,
                },
            )
            return
        if path == "/v1/matchmaking/queue" and self.command == "DELETE":
            playlist = self.query("playlist")
            if not playlist:
                self.send_json(400, {"error": "playlist is required"})
                return
            redis.lrem(f"omnis:mm:{playlist}:queue", 0, player)
            redis.srem(f"omnis:mm:{playlist}:members", player)
            self.send_json(200, {"queued": False, "playlist": playlist})
            return
        if path == "/v1/matchmaking/status" and self.command == "GET":
            playlist = self.query("playlist")
            if not playlist:
                self.send_json(400, {"error": "playlist is required"})
                return
            queued = redis.sismember(f"omnis:mm:{playlist}:members", player)
            self.send_json(200, {"queued": queued, "playlist": playlist})
            return
        if path == "/v1/matchmaking/claim" and self.command == "POST":
            playlist = self.query("playlist")
            if not playlist:
                self.send_json(400, {"error": "playlist is required"})
                return
            values = redis.lpop2(f"omnis:mm:{playlist}:queue")
            if len(values) < 2:
                for value in values:
                    redis.rpush(f"omnis:mm:{playlist}:queue", value)
                self.send_response(204)
                self.end_headers()
                return
            redis.srem(f"omnis:mm:{playlist}:members", values[0])
            redis.srem(f"omnis:mm:{playlist}:members", values[1])
            self.send_json(
                200,
                {
                    "matched": True,
                    "playlist": playlist,
                    "players": values,
                    "match_id": new_id(),
                },
            )
            return
        self.send_json(404, {"error": "matchmaking route not found"})

    def session(self):
        path = self.path.split("?", 1)[0]
        if path == "/v1/sessions" and self.command == "POST":
            payload = self.body_json()
            playlist = str(payload.get("playlist", "")).strip()
            players = payload.get("players") or [player_id(self)]
            if (
                not playlist
                or len(playlist) > 64
                or not isinstance(players, list)
                or len(players) > 64
            ):
                self.send_json(400, {"error": "valid playlist and players are required"})
                return
            created = now()
            session_id = new_id()
            state = {
                "id": session_id,
                "playlist": playlist,
                "world_revision": int(payload.get("world_revision", 0)),
                "players": players,
                "created_at": created,
                "expires_at": datetime.fromtimestamp(
                    time.time() + 86400, timezone.utc
                ).isoformat(),
                "status": "allocated",
            }
            redis.set(f"omnis:session:{session_id}", json.dumps(state), 86400)
            self.send_json(201, state)
            return
        if path.startswith("/v1/sessions/") and self.command == "GET":
            session_id = path.rsplit("/", 1)[-1]
            value = redis.get(f"omnis:session:{session_id}")
            if value is None:
                self.send_json(404, {"error": "session not found"})
                return
            self.send_json(200, json.loads(value))
            return
        self.send_json(404, {"error": "session route not found"})

    def world(self):
        path = self.path.split("?", 1)[0]
        if path == "/v1/world/cycle" and self.command == "GET":
            self.ensure_cycle()
            self.send_json(200, self.read_cycle())
            return
        if path == "/v1/world/cycle/advance" and self.command == "POST":
            payload = self.body_json()
            self.ensure_cycle()
            cycle_index = redis.incr("omnis:world:cycle-index")
            revision = redis.incr("omnis:world:revision")
            era = str(payload.get("era", "")).strip() or "Ciclo Renacido"
            updated = now()
            redis.set("omnis:world:cycle-era", era)
            redis.set("omnis:world:updated-at", updated)
            self.send_json(
                200,
                {
                    "cycle_index": cycle_index,
                    "era": era,
                    "world_revision": revision,
                    "reason": payload.get("reason", ""),
                },
            )
            return
        if path == "/v1/world/events" and self.command == "POST":
            payload = self.body_json()
            event_type = str(payload.get("event_type", "")).strip()
            if not event_type:
                self.send_json(400, {"error": "event_type is required"})
                return
            event_id = redis.incr("omnis:world:event-sequence")
            event = {
                "id": event_id,
                "event_type": event_type,
                "player_id": payload.get("player_id"),
                "payload": payload.get("payload") or {},
                "created_at": now(),
            }
            redis.set(f"omnis:world:event:{event_id}", json.dumps(event), 30 * 86400)
            self.send_json(201, {"id": event_id, "event_type": event_type})
            return
        if path == "/v1/world/events" and self.command == "GET":
            self.ensure_cycle()
            raw = redis.get("omnis:world:event-sequence")
            limit = min(max(int(self.query("limit") or 50), 1), 200)
            sequence = int(raw or 0)
            events = []
            for event_id in range(sequence, max(sequence - limit, 0), -1):
                event = redis.get(f"omnis:world:event:{event_id}")
                if event:
                    events.append(json.loads(event))
            self.send_json(200, {"events": events})
            return
        self.send_json(404, {"error": "world route not found"})

    def ensure_cycle(self):
        if redis.get("omnis:world:cycle-index") is not None:
            return
        redis.set("omnis:world:cycle-index", 0)
        redis.set("omnis:world:revision", 0)
        redis.set("omnis:world:cycle-era", "Ruinas de Piedra Ciénaga")
        redis.set("omnis:world:updated-at", now())

    def read_cycle(self):
        raw_idx = redis.get("omnis:world:cycle-index") or "0"
        raw_revision = redis.get("omnis:world:revision") or "0"
        era = redis.get("omnis:world:cycle-era") or "Ruinas de Piedra Ciénaga"
        updated = redis.get("omnis:world:updated-at") or now()
        return {
            "cycle_index": int(raw_idx),
            "era": era,
            "world_revision": int(raw_revision),
            "updated_at": updated,
        }

    def query(self, name):
        query = self.path.split("?", 1)[1] if "?" in self.path else ""
        for item in query.split("&"):
            if item.startswith(name + "="):
                return item.split("=", 1)[1]
        return ""

if __name__ == "__main__":
    print(json.dumps({"role": ROLE, "listen": f"{HOST}:{PORT}"}), flush=True)
    ThreadingHTTPServer((HOST, PORT), Handler).serve_forever()
