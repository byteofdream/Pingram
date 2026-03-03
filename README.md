# Pingram Client

C++ desktop client for Pingram messenger (Qt6 Widgets + Qt6 Network).

## Requirements

- C++20 compiler (`g++` recommended)
- `cmake` (3.20+)
- Qt6 development packages (`qt6-base-dev`)
- `nlohmann_json` (`nlohmann-json3-dev`)

Example (Ubuntu/Debian):

```bash
sudo apt update
sudo apt install -y qt6-base-dev nlohmann-json3-dev cmake g++
```

## Build

From project root:

```bash
cmake -S . -B build
cmake --build build -j
```

## Run

```bash
./build/pingram_client
```

## Backend Requirement

The client expects backend at:

- `http://127.0.0.1:8000`

Run server in another terminal:

```bash
cd server
source .venv/bin/activate
uvicorn app.main:app --reload --host 127.0.0.1 --port 8000
```

## Login / Registration

- You can log in with demo users (see `server/README.md`).
- You can register a new account from login screen (`Create new account`).
- Custom phone format with `+888...` is supported.

## Features in Client UI

- Login and registration by phone number
- Chat send flow (JSON API)
- Localization (RU / EN / UK)
- Profile and app settings tabs
- Premium settings section
- Country/region detection by phone prefix

## Troubleshooting

### App starts but login fails (401)

- Make sure server is running on `127.0.0.1:8000`.
- If using freshly registered account, avoid server restart (in-memory storage).
- Re-register and log in again if needed.

### Build fails with missing Qt6

Install Qt6 dev packages:

```bash
sudo apt install -y qt6-base-dev
```

### Build fails after major changes

Clean and rebuild:

```bash
cd client
rm -rf build
cmake -S . -B build
cmake --build build -j
```
