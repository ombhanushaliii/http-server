# A cool HTTP server in C++ (Winsock)

This is a minimal HTTP server built in **C++** using **Winsock2**.  
It supports both **GET** and **POST** requests, and logs the client request to the console.

---

## How it works
- **HTTP protocol** is system-independent.  
- What differs is how the OS exposes sockets:
  - **Unix/Linux/macOS** → use **POSIX sockets** (file descriptors: `int` values).
  - **Windows** → use **Winsock**, which returns a `SOCKET` type.  

This server:
1. Initializes Winsock (`WSAStartup`).
2. Creates a TCP socket (`socket(AF_INET, SOCK_STREAM, 0)`).
3. Binds it to `PORT 8080` (or any port you set).
4. Listens for connections (`listen`).
5. Accepts clients (`accept`).
6. Reads the request (`recv`).
7. Responds with:
   - **GET** → returns `"heyo!"`.
   - **POST** → echoes the POST body back in the response.

---

## Requirements
- Windows (tested on **MSYS2 / MinGW**).
- Compiler supporting C++11 or higher.

---

## Build
```bash
g++ server.cpp -o server.exe -lws2_32
