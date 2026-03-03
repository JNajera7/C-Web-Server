# C Web Server

## Overview

This project is a C-based web server implementation developed as part of a Network Systems assignment.  
It demonstrates basic HTTP server functionality while using `fork()` to handle multiple client connections concurrently.

The server supports loading static content and handling concurrent requests with proper error handling and connection management.

Final Grade: **100%**

---

## Features

### Loading HTML Pages
- Serves static `.html` files.
- Parses HTTP GET requests.
- Returns requested HTML content to the client.

### Loading Binary Files Such as Images
- Supports serving binary files (e.g., `.png`, `.jpg`, `.jpeg`).
- Sends files using proper binary transmission to prevent corruption.

### Handling Multiple Connections
- Uses `fork()` to create child processes.
- Supports concurrent client connections.
- Each client request is handled independently.

### Error Handling
- Handles invalid requests.
- Returns proper HTTP error responses (e.g., 404 Not Found, 400 Bad Request, 405 Method Not Allowed, 505 HTTP Version Not Supported).
- Prevents server crashes from malformed input.

---

## Compilation

```bash
make
```

## Execution

```bash
./server 8888
```
