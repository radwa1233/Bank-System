#!/usr/bin/env python3
"""
NOVA BANK - simple backend server (standard library only, no pip installs).

Serves the static HTML/CSS/images AND a small JSON API that reads/writes the
same #//#-separated text files used by the C++ console app, so both share data.

Run:  python server.py      (then open http://localhost:8000/loginPage.html)
"""

import json
import os
import re
from datetime import datetime
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from urllib.parse import urlparse, parse_qs

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
DATA_DIR = os.path.join(BASE_DIR, "data")
SEP = "#//#"

CLIENTS_FILE = os.path.join(DATA_DIR, "Clients.txt")
USERS_FILE = os.path.join(DATA_DIR, "Users.txt")
CURRENCIES_FILE = os.path.join(DATA_DIR, "Currencies.txt")
LOGIN_LOG_FILE = os.path.join(DATA_DIR, "LoginRegister.txt")
TRANSFER_LOG_FILE = os.path.join(DATA_DIR, "TransferLog.txt")


# ---------------------------------------------------------------- file helpers
def read_lines(path):
    if not os.path.exists(path):
        return []
    with open(path, "r", encoding="utf-8", errors="replace") as f:
        return [ln.rstrip("\n").rstrip("\r") for ln in f if ln.strip() != ""]


def write_lines(path, lines):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w", encoding="utf-8") as f:
        for ln in lines:
            f.write(ln + "\n")


def append_line(path, line):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "a", encoding="utf-8") as f:
        f.write(line + "\n")


# ---------------------------------------------------------------- model parsers
def safe_float(s):
    """Tolerant float parse: handles stray spaces, trailing dots, empty values."""
    try:
        return float(str(s).strip().rstrip("."))
    except (ValueError, AttributeError):
        return 0.0


def client_to_dict(line):
    # FirstName#//#LastName#//#Email#//#Phone#//#AccountNumber#//#PinCode#//#Balance
    p = line.split(SEP)
    if len(p) < 7:
        return None
    return {
        "firstName": p[0], "lastName": p[1], "email": p[2], "phone": p[3],
        "accountNumber": p[4], "pinCode": p[5],
        "balance": safe_float(p[6]),
    }


def client_to_line(c):
    return SEP.join([
        c["firstName"], c["lastName"], c["email"], c["phone"],
        c["accountNumber"], c["pinCode"], f'{float(c["balance"]):.6f}',
    ])


def user_to_dict(line):
    # FirstName#//#LastName#//#Email#//#Phone#//#UserName#//#Password#//#Permissions
    p = line.split(SEP)
    if len(p) < 7:
        return None
    return {
        "firstName": p[0], "lastName": p[1], "email": p[2], "phone": p[3],
        "userName": p[4], "password": p[5],
        "permissions": int(p[6]) if p[6].lstrip("-").isdigit() else 0,
    }


def user_to_line(u):
    return SEP.join([
        u["firstName"], u["lastName"], u["email"], u["phone"],
        u["userName"], u["password"], str(u["permissions"]),
    ])


def currency_to_dict(line):
    # Country#//#Code#//#Name#//#Rate
    p = line.split(SEP)
    if len(p) < 4:
        return None
    return {
        "country": p[0], "code": p[1], "name": p[2],
        "rate": safe_float(p[3]),
    }


# ---------------------------------------------------------------- data access
def load_clients():
    return [c for c in (client_to_dict(l) for l in read_lines(CLIENTS_FILE)) if c]


def save_clients(clients):
    write_lines(CLIENTS_FILE, [client_to_line(c) for c in clients])


def load_users():
    return [u for u in (user_to_dict(l) for l in read_lines(USERS_FILE)) if u]


def load_currencies():
    return [c for c in (currency_to_dict(l) for l in read_lines(CURRENCIES_FILE)) if c]


def now_string():
    n = datetime.now()
    return f"{n.day}/{n.month}/{n.year} - {n.hour}:{n.minute}:{n.second}"


# ---------------------------------------------------------------- API handlers
def api_login(body):
    username = body.get("username", "")
    password = body.get("password", "")
    for u in load_users():
        if u["userName"] == username and u["password"] == password:
            append_line(LOGIN_LOG_FILE,
                        SEP.join([now_string(), username, password, str(u["permissions"])]))
            safe = {k: v for k, v in u.items() if k != "password"}
            return {"ok": True, "user": safe}
    return {"ok": False, "error": "Invalid username or password"}


def api_clients_list():
    clients = load_clients()
    total = sum(c["balance"] for c in clients)
    return {"ok": True, "clients": clients, "count": len(clients), "totalBalance": total}


def api_client_add(body):
    clients = load_clients()
    acc = body.get("accountNumber", "").strip()
    if not acc:
        return {"ok": False, "error": "Account number required"}
    if any(c["accountNumber"] == acc for c in clients):
        return {"ok": False, "error": "Account number already exists"}
    new = {
        "firstName": body.get("firstName", ""), "lastName": body.get("lastName", ""),
        "email": body.get("email", ""), "phone": body.get("phone", ""),
        "accountNumber": acc, "pinCode": body.get("pinCode", ""),
        "balance": float(body.get("balance", 0) or 0),
    }
    clients.append(new)
    save_clients(clients)
    return {"ok": True, "client": new}


def api_client_update(body):
    clients = load_clients()
    acc = body.get("accountNumber", "")
    for c in clients:
        if c["accountNumber"] == acc:
            for k in ("firstName", "lastName", "email", "phone", "pinCode"):
                if k in body:
                    c[k] = body[k]
            if "balance" in body:
                c["balance"] = float(body["balance"] or 0)
            save_clients(clients)
            return {"ok": True, "client": c}
    return {"ok": False, "error": "Client not found"}


def api_client_delete(body):
    clients = load_clients()
    acc = body.get("accountNumber", "")
    new = [c for c in clients if c["accountNumber"] != acc]
    if len(new) == len(clients):
        return {"ok": False, "error": "Client not found"}
    save_clients(new)
    return {"ok": True}


def _find_client(clients, acc):
    for c in clients:
        if c["accountNumber"] == acc:
            return c
    return None


def api_transaction(body):
    kind = body.get("type")           # deposit | withdraw | transfer
    clients = load_clients()
    try:
        amount = float(body.get("amount", 0) or 0)
    except ValueError:
        return {"ok": False, "error": "Invalid amount"}
    if amount <= 0:
        return {"ok": False, "error": "Amount must be greater than zero"}

    src = _find_client(clients, body.get("accountNumber", ""))
    if not src:
        return {"ok": False, "error": "Account not found"}

    if kind == "deposit":
        src["balance"] += amount
    elif kind == "withdraw":
        if src["balance"] < amount:
            return {"ok": False, "error": "Insufficient balance"}
        src["balance"] -= amount
    elif kind == "transfer":
        dst = _find_client(clients, body.get("toAccount", ""))
        if not dst:
            return {"ok": False, "error": "Destination account not found"}
        if src["balance"] < amount:
            return {"ok": False, "error": "Insufficient balance"}
        src["balance"] -= amount
        dst["balance"] += amount
        log = (f"{now_string()} | From: {src['accountNumber']} | To: {dst['accountNumber']}"
               f" | Amount: {int(amount)} | Balance1: {src['balance']:.6f}"
               f" | Balance2: {dst['balance']:.6f} | User: {body.get('user', 'web')}")
        append_line(TRANSFER_LOG_FILE, log)
    else:
        return {"ok": False, "error": "Unknown transaction type"}

    save_clients(clients)
    return {"ok": True, "client": src}


def api_users_list():
    users = [{k: v for k, v in u.items() if k != "password"} for u in load_users()]
    return {"ok": True, "users": users, "count": len(users)}


def api_currencies_list():
    cur = load_currencies()
    return {"ok": True, "currencies": cur, "count": len(cur)}


def api_transfers_list():
    # Parse the human-readable transfer log lines into structured records.
    records = []
    for line in read_lines(TRANSFER_LOG_FILE):
        rec = {"dateTime": "", "from": "", "to": "", "amount": "", "user": ""}
        for part in line.split("|"):
            part = part.strip()
            if part.startswith("From:"):
                rec["from"] = part[5:].strip()
            elif part.startswith("To:"):
                rec["to"] = part[3:].strip()
            elif part.startswith("Amount:"):
                rec["amount"] = part[7:].strip()
            elif part.startswith("User:"):
                rec["user"] = part[5:].strip()
            elif part.startswith("Balance"):
                continue
            elif rec["dateTime"] == "":
                rec["dateTime"] = part
        records.append(rec)
    records.reverse()  # newest first
    return {"ok": True, "transfers": records, "count": len(records)}


def api_dashboard():
    clients = load_clients()
    users = load_users()
    currencies = load_currencies()
    transfers = read_lines(TRANSFER_LOG_FILE)
    return {
        "ok": True,
        "totalClients": len(clients),
        "totalBalance": sum(c["balance"] for c in clients),
        "totalUsers": len(users),
        "totalCurrencies": len(currencies),
        "totalTransfers": len(transfers),
        "topClients": sorted(clients, key=lambda c: c["balance"], reverse=True)[:5],
    }


ROUTES = {
    ("POST", "/api/login"): lambda b: api_login(b),
    ("GET", "/api/clients"): lambda b: api_clients_list(),
    ("POST", "/api/clients/add"): lambda b: api_client_add(b),
    ("POST", "/api/clients/update"): lambda b: api_client_update(b),
    ("POST", "/api/clients/delete"): lambda b: api_client_delete(b),
    ("POST", "/api/transaction"): lambda b: api_transaction(b),
    ("GET", "/api/users"): lambda b: api_users_list(),
    ("GET", "/api/currencies"): lambda b: api_currencies_list(),
    ("GET", "/api/transfers"): lambda b: api_transfers_list(),
    ("GET", "/api/dashboard"): lambda b: api_dashboard(),
}


# ---------------------------------------------------------------- HTTP server
class Handler(BaseHTTPRequestHandler):
    def _send_json(self, obj, status=200):
        data = json.dumps(obj).encode("utf-8")
        self.send_response(status)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(data)))
        self.send_header("Access-Control-Allow-Origin", "*")
        self.end_headers()
        self.wfile.write(data)

    def _serve_static(self, path):
        rel = path.lstrip("/")
        if rel == "":
            rel = "loginPage.html"
        # prevent path traversal
        full = os.path.normpath(os.path.join(BASE_DIR, rel))
        if not full.startswith(BASE_DIR) or not os.path.isfile(full):
            self.send_error(404, "Not found")
            return
        ctypes = {
            ".html": "text/html; charset=utf-8", ".css": "text/css; charset=utf-8",
            ".js": "application/javascript; charset=utf-8", ".png": "image/png",
            ".jpg": "image/jpeg", ".jpeg": "image/jpeg", ".svg": "image/svg+xml",
            ".ico": "image/x-icon",
        }
        ext = os.path.splitext(full)[1].lower()
        with open(full, "rb") as f:
            body = f.read()
        self.send_response(200)
        self.send_header("Content-Type", ctypes.get(ext, "application/octet-stream"))
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def do_GET(self):
        parsed = urlparse(self.path)
        route = ROUTES.get(("GET", parsed.path))
        if route:
            try:
                self._send_json(route(parse_qs(parsed.query)))
            except Exception as e:
                self._send_json({"ok": False, "error": str(e)}, 500)
            return
        self._serve_static(parsed.path)

    def do_POST(self):
        parsed = urlparse(self.path)
        route = ROUTES.get(("POST", parsed.path))
        if not route:
            self.send_error(404, "Not found")
            return
        length = int(self.headers.get("Content-Length", 0) or 0)
        raw = self.rfile.read(length) if length else b"{}"
        try:
            body = json.loads(raw.decode("utf-8") or "{}")
        except json.JSONDecodeError:
            body = {}
        try:
            self._send_json(route(body))
        except Exception as e:
            self._send_json({"ok": False, "error": str(e)}, 500)

    def log_message(self, fmt, *args):
        pass  # quiet console


def main():
    port = 8000
    server = ThreadingHTTPServer(("0.0.0.0", port), Handler)
    print("=" * 52)
    print("  NOVA BANK server is running")
    print(f"  Open:  http://localhost:{port}/loginPage.html")
    print("  Press Ctrl+C to stop")
    print("=" * 52)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\nServer stopped.")


if __name__ == "__main__":
    main()
