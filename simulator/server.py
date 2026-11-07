"""RVC simulator web server (Python standard library only).

  python simulator/server.py [--port 8000]
  then open http://localhost:8000

Routes
  GET  /                      simulator/web/index.html (+ app.js, style.css)
  GET  /traces/<ID>.json      pre-generated trace
  GET  /api/scenarios         built-in + custom scenarios
  POST /run                   body = scenario JSON -> trace JSON from the real C
                              controller (sim_runner, fresh process = reset)
  POST /api/scenarios         save a custom map as a new scenario
  GET  /api/evaluations       recorded human evaluations
  POST /api/evaluations       record a human evaluation (+ its trace)
"""
import argparse
import json
import os
import re
import subprocess
import sys
import tempfile
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer

SIM_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, SIM_DIR)

import build_runner  # noqa: E402
import results_report  # noqa: E402

WEB_DIR = os.path.join(SIM_DIR, "web")
SCEN_DIR = os.path.join(SIM_DIR, "scenarios")
CUSTOM_DIR = os.path.join(SCEN_DIR, "custom")
TRACE_DIR = os.path.join(SIM_DIR, "traces")
MAX_BODY = 2 * 1024 * 1024
ID_RE = re.compile(r"^[A-Za-z0-9_-]{1,40}$")
MIME = {".html": "text/html; charset=utf-8", ".js": "text/javascript; charset=utf-8",
        ".css": "text/css; charset=utf-8", ".json": "application/json; charset=utf-8"}

RUNNER = None


def run_controller(scenario):
    """Runs sim_runner on the map part of scenario. Returns (status, dict)."""
    data = {k: scenario.get(k) for k in ("start", "obstacles", "dust", "max_ticks")}
    fd, path = tempfile.mkstemp(suffix=".json")
    try:
        with os.fdopen(fd, "w", encoding="utf-8") as f:
            json.dump(data, f)
        res = subprocess.run([RUNNER, path], capture_output=True, text=True, timeout=30)
    finally:
        os.remove(path)
    try:
        out = json.loads(res.stdout)
    except ValueError:
        return 500, {"error": "runner produced no JSON: " + res.stderr.strip()}
    return (200 if res.returncode == 0 else 400), out


def list_scenarios():
    items = []
    for d, custom in ((SCEN_DIR, False), (CUSTOM_DIR, True)):
        if not os.path.isdir(d):
            continue
        for name in sorted(os.listdir(d)):
            if name.endswith(".json"):
                with open(os.path.join(d, name), encoding="utf-8") as f:
                    s = json.load(f)
                s["custom"] = custom
                s["has_trace"] = (not custom) and os.path.exists(os.path.join(TRACE_DIR, s["id"] + ".json"))
                items.append(s)
    return items


class Handler(BaseHTTPRequestHandler):
    server_version = "RVCSim/1.0"

    def log_message(self, fmt, *args):
        sys.stderr.write("[%s] %s\n" % (self.log_date_time_string(), fmt % args))

    def send_json(self, status, obj):
        body = json.dumps(obj).encode("utf-8")
        self.send_response(status)
        self.send_header("Content-Type", MIME[".json"])
        self.send_header("Content-Length", str(len(body)))
        self.send_header("Cache-Control", "no-store")
        self.end_headers()
        self.wfile.write(body)

    def send_file(self, path):
        if not os.path.isfile(path):
            self.send_json(404, {"error": "not found"})
            return
        with open(path, "rb") as f:
            body = f.read()
        self.send_response(200)
        self.send_header("Content-Type", MIME.get(os.path.splitext(path)[1], "application/octet-stream"))
        self.send_header("Content-Length", str(len(body)))
        self.send_header("Cache-Control", "no-store")
        self.end_headers()
        self.wfile.write(body)

    def read_json(self):
        n = int(self.headers.get("Content-Length", "0"))
        if n <= 0 or n > MAX_BODY:
            raise ValueError("missing or too large request body")
        return json.loads(self.rfile.read(n).decode("utf-8"))

    def do_GET(self):
        p = self.path.split("?", 1)[0]
        if p in ("/", "/index.html"):
            return self.send_file(os.path.join(WEB_DIR, "index.html"))
        if p in ("/app.js", "/style.css"):
            return self.send_file(os.path.join(WEB_DIR, p[1:]))
        m = re.match(r"^/traces/([A-Za-z0-9_-]+)\.json$", p)
        if m:
            return self.send_file(os.path.join(TRACE_DIR, m.group(1) + ".json"))
        if p == "/api/scenarios":
            return self.send_json(200, list_scenarios())
        if p == "/api/evaluations":
            return self.send_json(200, results_report.load_evaluations())
        self.send_json(404, {"error": "not found"})

    def do_POST(self):
        p = self.path.split("?", 1)[0]
        try:
            body = self.read_json()
        except ValueError as e:
            return self.send_json(400, {"error": str(e)})
        if p == "/run":
            status, out = run_controller(body)
            return self.send_json(status, out)
        if p == "/api/scenarios":
            return self.save_scenario(body)
        if p == "/api/evaluations":
            try:
                stored = results_report.save_evaluation(body.get("record", {}), body.get("trace"))
            except ValueError as e:
                return self.send_json(400, {"error": str(e)})
            return self.send_json(200, stored)
        self.send_json(404, {"error": "not found"})

    def save_scenario(self, s):
        sid = str(s.get("id", ""))
        if not ID_RE.match(sid):
            return self.send_json(400, {"error": "id must be 1-40 letters, digits, '-' or '_'"})
        if os.path.exists(os.path.join(SCEN_DIR, sid + ".json")):
            return self.send_json(400, {"error": f"{sid} is a built-in scenario id; choose another id"})
        target = os.path.join(CUSTOM_DIR, sid + ".json")
        if os.path.exists(target) and not s.get("overwrite"):
            return self.send_json(409, {"error": f"custom scenario {sid} exists", "exists": True})
        status, out = run_controller(s)  # validates the map with the same rules as a run
        if status != 200:
            return self.send_json(400, out)
        keep = {k: s.get(k) for k in ("id", "name", "start", "obstacles", "dust", "max_ticks",
                                      "functions", "requirements", "expected", "notes")}
        keep["name"] = str(keep.get("name") or sid)[:200]
        os.makedirs(CUSTOM_DIR, exist_ok=True)
        with open(target, "w", encoding="utf-8") as f:
            json.dump(keep, f, indent=2)
        self.send_json(200, {"saved": "simulator/scenarios/custom/" + sid + ".json"})


def main():
    global RUNNER
    ap = argparse.ArgumentParser()
    ap.add_argument("--port", type=int, default=8000)
    args = ap.parse_args()
    RUNNER = build_runner.build()
    httpd = ThreadingHTTPServer(("127.0.0.1", args.port), Handler)
    print(f"RVC simulator: http://localhost:{args.port}  (runner: {os.path.relpath(RUNNER, SIM_DIR)})")
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        pass


if __name__ == "__main__":
    main()
