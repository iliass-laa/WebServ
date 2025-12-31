# Webserv Unit Tests & Explanations

## Table of Contents
1. [Core Requirements Tests](#core-requirements-tests)
2. [HTTP Protocol Tests](#http-protocol-tests)
3. [Configuration File Tests](#configuration-file-tests)
4. [CGI Tests](#cgi-tests)
5. [Performance & Stress Tests](#performance--stress-tests)
6. [Error Handling Tests](#error-handling-tests)
7. [Bonus Features Tests](#bonus-features-tests)

---

## Core Requirements Tests

### 1. Non-Blocking I/O Tests

#### Test 1.1: Server Must Never Block
**Purpose**: Verify that the server uses non-blocking I/O for all socket operations.

**Test Procedure**:
```bash
# Start the server
./webserv config.conf

# Use multiple concurrent connections
for i in {1..50}; do
    (echo -e "GET /slow-resource HTTP/1.1\r\nHost: localhost\r\n\r\n" | nc localhost 8080 &)
done

# Server should handle all connections without blocking
```

**Expected Result**: All connections are accepted and processed concurrently without any connection being refused or timing out.

**Explanation**: Non-blocking I/O ensures that a slow client or a long-running operation doesn't prevent the server from handling other clients. This is fundamental to web server scalability.

---

#### Test 1.2: Single poll() for All I/O
**Purpose**: Verify that the server uses exactly one poll() (or equivalent) call for all I/O operations.

**Test Procedure**:
1. Review the code to ensure only one poll()/select()/epoll()/kqueue() call exists in the main event loop
2. Use `strace` to verify:
```bash
strace -e poll,select,epoll_wait,kevent ./webserv config.conf 2>&1 | grep -E "poll|select|epoll|kevent"
```

**Expected Result**: Only one multiplexing system call is used repeatedly in the main loop.

**Explanation**: Using a single multiplexing call simplifies the architecture and ensures consistent handling of all file descriptors.

---

#### Test 1.3: Read/Write Only After poll() Readiness
**Purpose**: Ensure that read/write operations only occur after poll() indicates readiness.

**Test Procedure**:
1. Code review to verify that all read/recv and write/send calls are preceded by poll() readiness checks
2. Test with multiple slow clients:
```python
import socket
import time

sock = socket.socket()
sock.connect(('localhost', 8080))
# Send request very slowly, byte by byte
request = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"
for char in request:
    sock.send(char.encode())
    time.sleep(0.1)  # Slow send
response = sock.recv(4096)
print(response.decode())
```

**Expected Result**: Server waits for readiness signals from poll() before attempting I/O operations.

**Explanation**: Attempting I/O without readiness would cause the server to block or fail, violating the non-blocking requirement.

---

### 2. Request Timeout Tests

#### Test 2.1: Request Must Not Hang Indefinitely
**Purpose**: Verify that incomplete or stalled requests eventually timeout.

**Test Procedure**:
```python
import socket

# Connect but never complete the request
sock = socket.socket()
sock.connect(('localhost', 8080))
sock.send(b"GET / HTTP/1.1\r\n")  # Incomplete request
# Wait to see if server closes connection
time.sleep(60)
```

**Expected Result**: Server closes the connection after a reasonable timeout (typically 30-60 seconds).

**Explanation**: Servers must implement timeouts to prevent resource exhaustion from clients that connect but never complete requests or disconnect properly.

---

### 3. Memory Management Tests

#### Test 3.1: No Memory Leaks
**Purpose**: Verify that all allocated memory is properly freed.

**Test Procedure**:
```bash
# Run server with valgrind
valgrind --leak-check=full --show-leak-kinds=all ./webserv config.conf

# In another terminal, send requests
for i in {1..100}; do
    curl http://localhost:8080/
done

# Stop server (Ctrl+C) and check valgrind output
```

**Expected Result**: Valgrind reports "All heap blocks were freed" with 0 bytes definitely lost.

**Explanation**: Memory leaks are unacceptable in long-running servers as they will eventually exhaust system resources.

---

#### Test 3.2: Memory Under Heavy Load
**Purpose**: Verify memory doesn't grow unbounded under sustained load.

**Test Procedure**:
```bash
# Monitor memory usage
watch -n 1 'ps aux | grep webserv'

# Generate sustained load
ab -n 10000 -c 100 http://localhost:8080/
```

**Expected Result**: Memory usage stabilizes and doesn't continuously grow.

**Explanation**: Proper resource cleanup ensures the server can run indefinitely without memory exhaustion.

---

## HTTP Protocol Tests

### 4. HTTP Methods Tests

#### Test 4.1: GET Method
**Purpose**: Verify correct handling of GET requests.

**Test Procedure**:
```bash
# Test basic GET
curl -v http://localhost:8080/index.html

# Test GET with query parameters
curl -v http://localhost:8080/page?param1=value1&param2=value2

# Test GET for non-existent resource
curl -v http://localhost:8080/nonexistent.html
```

**Expected Results**:
- 200 OK for existing resources with correct content
- 404 Not Found for non-existent resources
- Query parameters properly handled (if applicable)

**Explanation**: GET is the most common HTTP method, used to retrieve resources. It should never modify server state.

---

#### Test 4.2: POST Method
**Purpose**: Verify correct handling of POST requests with body data.

**Test Procedure**:
```bash
# Test simple POST
curl -X POST -d "name=John&age=30" http://localhost:8080/form

# Test POST with JSON
curl -X POST -H "Content-Type: application/json" \
     -d '{"name":"John","age":30}' \
     http://localhost:8080/api

# Test POST with large body
dd if=/dev/urandom of=testfile.bin bs=1M count=10
curl -X POST --data-binary @testfile.bin http://localhost:8080/upload
```

**Expected Results**:
- 200/201 for successful posts
- Request body fully received and processed
- Proper Content-Length handling

**Explanation**: POST is used to submit data to the server. The server must correctly receive and process the entire request body.

---

#### Test 4.3: DELETE Method
**Purpose**: Verify DELETE method implementation.

**Test Procedure**:
```bash
# Create a file to delete
curl -X POST --data-binary @test.txt http://localhost:8080/upload/test.txt

# Delete the file
curl -X DELETE http://localhost:8080/upload/test.txt

# Verify deletion
curl -v http://localhost:8080/upload/test.txt
```

**Expected Results**:
- 200/204 for successful deletion
- 404 when trying to access deleted resource
- 404 for non-existent resources

**Explanation**: DELETE allows clients to remove resources. The server must verify permissions and handle the deletion safely.

---

#### Test 4.4: Unsupported Methods
**Purpose**: Verify proper rejection of unsupported HTTP methods.

**Test Procedure**:
```bash
curl -X PUT http://localhost:8080/resource
curl -X PATCH http://localhost:8080/resource
curl -X OPTIONS http://localhost:8080/resource
```

**Expected Result**: 405 Method Not Allowed with appropriate Allow header.

**Explanation**: Servers should explicitly reject unsupported methods rather than failing silently.

---

### 5. HTTP Status Codes Tests

#### Test 5.1: Success Codes (2xx)
**Purpose**: Verify correct 2xx status codes.

**Test Scenarios**:
```bash
# 200 OK - Successful GET
curl -w "%{http_code}" http://localhost:8080/

# 201 Created - Successful resource creation
curl -w "%{http_code}" -X POST -d "data" http://localhost:8080/resource

# 204 No Content - Successful DELETE
curl -w "%{http_code}" -X DELETE http://localhost:8080/resource
```

**Expected Result**: Appropriate 2xx codes for successful operations.

---

#### Test 5.2: Redirection Codes (3xx)
**Purpose**: Verify redirect handling if configured.

**Test Procedure**:
```bash
# Test configured redirect
curl -L -v http://localhost:8080/old-page

# Test without following redirects
curl -v http://localhost:8080/old-page
```

**Expected Results**:
- 301/302 status code
- Location header pointing to new URL
- Redirect followed when using -L flag

**Explanation**: Redirects inform clients that a resource has moved, either temporarily or permanently.

---

#### Test 5.3: Client Error Codes (4xx)
**Purpose**: Verify correct 4xx error responses.

**Test Scenarios**:
```bash
# 400 Bad Request - Malformed request
echo -e "INVALID REQUEST\r\n\r\n" | nc localhost 8080

# 404 Not Found
curl -w "%{http_code}" http://localhost:8080/nonexistent

# 405 Method Not Allowed
curl -w "%{http_code}" -X PUT http://localhost:8080/

# 413 Payload Too Large
dd if=/dev/zero bs=1M count=100 | curl -X POST --data-binary @- http://localhost:8080/upload

# 414 URI Too Long
curl -w "%{http_code}" http://localhost:8080/$(python3 -c "print('a'*10000)")
```

**Expected Result**: Appropriate 4xx status codes for each error condition.

**Explanation**: 4xx codes indicate client errors. The server must detect and report these correctly.

---

#### Test 5.4: Server Error Codes (5xx)
**Purpose**: Verify 5xx error responses when appropriate.

**Test Scenarios**:
```bash
# 500 Internal Server Error - CGI script fails
curl http://localhost:8080/broken-cgi.php

# 501 Not Implemented - Unsupported feature
curl -X CONNECT http://localhost:8080/

# 503 Service Unavailable - Under extreme load
# Run heavy load test and verify graceful degradation
```

**Expected Result**: 5xx codes when server encounters errors it can't handle.

**Explanation**: 5xx codes indicate server-side problems, distinguishing them from client errors.

---

### 6. HTTP Headers Tests

#### Test 6.1: Required Request Headers
**Purpose**: Verify server handles essential request headers.

**Test Procedure**:
```bash
# Test Host header (required in HTTP/1.1)
curl -H "Host: example.com" http://localhost:8080/

# Test without Host header (should fail or default)
telnet localhost 8080
GET / HTTP/1.1

# Test Content-Length
curl -X POST -H "Content-Length: 5" -d "hello" http://localhost:8080/

# Test Content-Type
curl -X POST -H "Content-Type: application/json" -d '{"key":"value"}' http://localhost:8080/
```

**Expected Result**: Proper handling and validation of headers.

---

#### Test 6.2: Response Headers
**Purpose**: Verify server sends correct response headers.

**Test Procedure**:
```bash
curl -I http://localhost:8080/
```

**Expected Headers**:
- `HTTP/1.1 200 OK` (or appropriate status)
- `Content-Type: text/html` (or appropriate type)
- `Content-Length: <size>` (when known)
- `Date: <current date>`
- `Server: webserv/1.0` (or similar)

**Explanation**: Response headers provide essential metadata about the response.

---

#### Test 6.3: Connection Management Headers
**Purpose**: Verify Connection header handling.

**Test Procedure**:
```bash
# Test Connection: keep-alive
curl -H "Connection: keep-alive" -v http://localhost:8080/

# Test Connection: close
curl -H "Connection: close" -v http://localhost:8080/
```

**Expected Result**: Server respects connection preferences when possible.

---

### 7. Request Body Tests

#### Test 7.1: Content-Length Based Bodies
**Purpose**: Verify proper handling of requests with Content-Length.

**Test Procedure**:
```python
import socket

sock = socket.socket()
sock.connect(('localhost', 8080))

request = (
    "POST /upload HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "Content-Length: 13\r\n"
    "\r\n"
    "Hello, World!"
)
sock.send(request.encode())
response = sock.recv(4096)
print(response.decode())
```

**Expected Result**: Server reads exactly Content-Length bytes from body.

---

#### Test 7.2: Chunked Transfer Encoding
**Purpose**: Verify handling of chunked requests (important for CGI).

**Test Procedure**:
```python
import socket

sock = socket.socket()
sock.connect(('localhost', 8080))

request = (
    "POST /upload HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "Transfer-Encoding: chunked\r\n"
    "\r\n"
    "5\r\n"
    "Hello\r\n"
    "7\r\n"
    ", World\r\n"
    "0\r\n"
    "\r\n"
)
sock.send(request.encode())
response = sock.recv(4096)
print(response.decode())
```

**Expected Result**: Server properly decodes chunks and reconstructs body.

**Explanation**: The subject explicitly mentions that chunked requests must be unchunked before passing to CGI.

---

#### Test 7.3: Body Size Limits
**Purpose**: Verify enforcement of maximum body size from config.

**Test Procedure**:
```bash
# If max body size is 10MB
dd if=/dev/zero bs=1M count=11 | curl -X POST --data-binary @- http://localhost:8080/upload
```

**Expected Result**: 413 Payload Too Large when exceeding configured limit.

**Explanation**: Servers must protect against resource exhaustion from excessive request bodies.

---

## Configuration File Tests

### 8. Server Configuration Tests

#### Test 8.1: Multiple Port Bindings
**Purpose**: Verify server can listen on multiple ports simultaneously.

**Configuration**:
```nginx
server {
    listen 8080;
    server_name localhost;
}

server {
    listen 8081;
    server_name localhost;
}
```

**Test Procedure**:
```bash
curl http://localhost:8080/
curl http://localhost:8081/
```

**Expected Result**: Both ports respond correctly with potentially different content.

---

#### Test 8.2: Multiple Server Blocks (Virtual Hosts)
**Purpose**: Verify different configurations for different ports/hosts.

**Configuration**:
```nginx
server {
    listen 8080;
    root /var/www/site1;
}

server {
    listen 8081;
    root /var/www/site2;
}
```

**Test Procedure**:
```bash
curl http://localhost:8080/  # Should serve from site1
curl http://localhost:8081/  # Should serve from site2
```

**Expected Result**: Each port serves content from its configured root.

---

#### Test 8.3: Default Error Pages
**Purpose**: Verify custom error pages from configuration.

**Configuration**:
```nginx
server {
    error_page 404 /custom_404.html;
    error_page 500 502 503 504 /custom_50x.html;
}
```

**Test Procedure**:
```bash
curl http://localhost:8080/nonexistent  # Should return custom 404
```

**Expected Result**: Custom error page served instead of default.

---

#### Test 8.4: Client Body Size Limit
**Purpose**: Verify client_max_body_size configuration.

**Configuration**:
```nginx
server {
    client_max_body_size 1M;
}
```

**Test Procedure**:
```bash
# Try to upload 2MB (should fail)
dd if=/dev/zero bs=1M count=2 | curl -X POST --data-binary @- http://localhost:8080/upload
```

**Expected Result**: 413 Payload Too Large.

---

### 9. Location/Route Configuration Tests

#### Test 9.1: Root Directory Configuration
**Purpose**: Verify root directive for locating files.

**Configuration**:
```nginx
location /docs {
    root /var/www/documentation;
}
```

**Test Procedure**:
```bash
# Request /docs/guide.html
# Should look for /var/www/documentation/docs/guide.html
curl http://localhost:8080/docs/guide.html
```

**Expected Result**: File served from correct filesystem location.

**Explanation**: The root directive prepends the path, while alias would replace it.

---

#### Test 9.2: Directory Listing
**Purpose**: Verify autoindex on/off functionality.

**Configuration**:
```nginx
location /public {
    root /var/www;
    autoindex on;
}

location /private {
    root /var/www;
    autoindex off;
}
```

**Test Procedure**:
```bash
curl http://localhost:8080/public/   # Should show file list
curl http://localhost:8080/private/  # Should show 403 or default file
```

**Expected Result**: Directory listing shown only when enabled.

---

#### Test 9.3: Default Index Files
**Purpose**: Verify index file configuration.

**Configuration**:
```nginx
location / {
    index index.html index.htm default.html;
}
```

**Test Procedure**:
```bash
# Request directory, should try files in order
curl http://localhost:8080/somedir/
```

**Expected Result**: First existing index file is served.

---

#### Test 9.4: HTTP Method Restrictions
**Purpose**: Verify limiting allowed methods per location.

**Configuration**:
```nginx
location /api {
    limit_except GET POST {
        deny all;
    }
}
```

**Test Procedure**:
```bash
curl -X GET http://localhost:8080/api    # Should work
curl -X POST http://localhost:8080/api   # Should work
curl -X DELETE http://localhost:8080/api # Should return 405
```

**Expected Result**: Only configured methods are allowed.

---

#### Test 9.5: HTTP Redirects
**Purpose**: Verify redirect configuration.

**Configuration**:
```nginx
location /old-page {
    return 301 /new-page;
}
```

**Test Procedure**:
```bash
curl -L -v http://localhost:8080/old-page
```

**Expected Result**: 301 redirect with Location header, curl follows to new page.

---

#### Test 9.6: Upload Location Configuration
**Purpose**: Verify file upload destination configuration.

**Configuration**:
```nginx
location /upload {
    upload_store /var/www/uploads;
}
```

**Test Procedure**:
```bash
curl -X POST -F "file=@test.txt" http://localhost:8080/upload
ls /var/www/uploads/  # Should contain uploaded file
```

**Expected Result**: File uploaded to configured location.

---

## CGI Tests

### 10. Basic CGI Tests

#### Test 10.1: CGI Execution Based on Extension
**Purpose**: Verify CGI scripts execute for configured extensions.

**Configuration**:
```nginx
location ~ \.php$ {
    fastcgi_pass 127.0.0.1:9000;  # Or execute php-cgi directly
}

location ~ \.py$ {
    cgi_pass /usr/bin/python3;
}
```

**Test Files**:

`test.php`:
```php
<?php
header("Content-Type: text/plain");
echo "Hello from PHP CGI\n";
echo "Query: " . $_SERVER['QUERY_STRING'] . "\n";
?>
```

`test.py`:
```python
#!/usr/bin/env python3
print("Content-Type: text/plain\n")
print("Hello from Python CGI")
import os
print(f"Query: {os.environ.get('QUERY_STRING', '')}")
```

**Test Procedure**:
```bash
curl http://localhost:8080/test.php?name=John
curl http://localhost:8080/test.py?name=Jane
```

**Expected Result**: CGI scripts execute and return proper output.

---

#### Test 10.2: CGI Environment Variables
**Purpose**: Verify correct environment variables passed to CGI.

**Test Script** (`env.php`):
```php
<?php
header("Content-Type: text/plain");
foreach ($_SERVER as $key => $value) {
    echo "$key = $value\n";
}
?>
```

**Test Procedure**:
```bash
curl "http://localhost:8080/env.php?param=value" -H "X-Custom: header"
```

**Expected Environment Variables**:
- `REQUEST_METHOD` = GET
- `QUERY_STRING` = param=value
- `SCRIPT_FILENAME` = /path/to/env.php
- `SERVER_PROTOCOL` = HTTP/1.1
- `HTTP_X_CUSTOM` = header
- `CONTENT_LENGTH` (for POST)
- `CONTENT_TYPE` (for POST)
- `PATH_INFO`, `SERVER_NAME`, `SERVER_PORT`, etc.

**Explanation**: CGI relies on environment variables to receive request metadata. All relevant HTTP headers and request information must be passed.

---

#### Test 10.3: CGI POST Request Handling
**Purpose**: Verify request body passed to CGI via stdin.

**Test Script** (`post.php`):
```php
<?php
header("Content-Type: text/plain");
$input = file_get_contents('php://input');
echo "Received: " . $input . "\n";
echo "Length: " . strlen($input) . "\n";
?>
```

**Test Procedure**:
```bash
echo "test data" | curl -X POST --data-binary @- http://localhost:8080/post.php
```

**Expected Result**: CGI receives full request body via stdin.

---

#### Test 10.4: CGI Chunked Request Handling
**Purpose**: Verify chunked requests are unchunked before passing to CGI.

**Test Procedure**:
```python
import socket

sock = socket.socket()
sock.connect(('localhost', 8080))

request = (
    "POST /post.php HTTP/1.1\r\n"
    "Host: localhost\r\n"
    "Transfer-Encoding: chunked\r\n"
    "\r\n"
    "4\r\n"
    "test\r\n"
    "5\r\n"
    " data\r\n"
    "0\r\n"
    "\r\n"
)
sock.send(request.encode())
response = sock.recv(4096)
print(response.decode())
```

**Expected Result**: CGI receives "test data" (unchunked) via stdin, not the raw chunks.

**Explanation**: The subject explicitly requires: "for chunked requests, your server needs to unchunk them, the CGI will expect EOF as the end of the body."

---

#### Test 10.5: CGI Output Without Content-Length
**Purpose**: Verify handling of CGI output without Content-Length.

**Test Script** (`stream.py`):
```python
#!/usr/bin/env python3
print("Content-Type: text/plain\n")
for i in range(5):
    print(f"Line {i}")
```

**Test Procedure**:
```bash
curl http://localhost:8080/stream.py
```

**Expected Result**: All output received, EOF marks end of response.

**Explanation**: The subject states: "If no content_length is returned from the CGI, EOF will mark the end of the returned data."

---

#### Test 10.6: CGI Working Directory
**Purpose**: Verify CGI executes in correct directory for relative paths.

**Test Script** (`path.php`):
```php
<?php
header("Content-Type: text/plain");
echo "CWD: " . getcwd() . "\n";
echo "Script: " . $_SERVER['SCRIPT_FILENAME'] . "\n";
// Try to access a file in the same directory
if (file_exists('./data.txt')) {
    echo "Found: " . file_get_contents('./data.txt');
}
?>
```

**Test Procedure**:
```bash
curl http://localhost:8080/cgi-bin/path.php
```

**Expected Result**: CGI runs in the directory containing the script, can access relative files.

**Explanation**: The subject requires: "The CGI should be run in the correct directory for relative path file access."

---

#### Test 10.7: Multiple CGI Support
**Purpose**: Verify support for multiple CGI interpreters.

**Configuration**:
```nginx
location ~ \.php$ {
    cgi_pass /usr/bin/php-cgi;
}

location ~ \.py$ {
    cgi_pass /usr/bin/python3;
}

location ~ \.pl$ {
    cgi_pass /usr/bin/perl;
}
```

**Test Procedure**:
```bash
curl http://localhost:8080/test.php
curl http://localhost:8080/test.py
curl http://localhost:8080/test.pl
```

**Expected Result**: Each extension uses its appropriate interpreter.

---

#### Test 10.8: CGI Error Handling
**Purpose**: Verify proper handling when CGI fails.

**Test Script** (`error.php`):
```php
<?php
exit(1);  // Non-zero exit code
?>
```

**Test Procedure**:
```bash
curl http://localhost:8080/error.php
```

**Expected Result**: 500 Internal Server Error returned to client.

---

## Performance & Stress Tests

### 11. Concurrent Connection Tests

#### Test 11.1: Multiple Simultaneous Connections
**Purpose**: Verify server handles many concurrent connections.

**Test Procedure**:
```bash
# Apache Bench - 1000 requests, 100 concurrent
ab -n 1000 -c 100 http://localhost:8080/

# Or with curl in parallel
seq 1 100 | xargs -P 100 -I {} curl http://localhost:8080/
```

**Expected Result**: All requests succeed without errors or timeouts.

---

#### Test 11.2: Slow Client Handling
**Purpose**: Verify slow clients don't block other clients.

**Test Procedure**:
```python
import socket
import time
import threading

def slow_client():
    sock = socket.socket()
    sock.connect(('localhost', 8080))
    request = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"
    for char in request:
        sock.send(char.encode())
        time.sleep(0.5)  # Very slow
    response = sock.recv(4096)
    sock.close()

def fast_client():
    sock = socket.socket()
    sock.connect(('localhost', 8080))
    sock.send(b"GET / HTTP/1.1\r\nHost: localhost\r\n\r\n")
    response = sock.recv(4096)
    sock.close()
    return time.time()

# Start slow client
threading.Thread(target=slow_client).start()
time.sleep(1)

# Fast clients should still be fast
start = time.time()
fast_client()
duration = time.time() - start

print(f"Fast client took {duration}s (should be < 1s)")
```

**Expected Result**: Fast clients complete quickly despite slow client.

---

#### Test 11.3: Keep-Alive Connection Tests
**Purpose**: Verify proper handling of persistent connections.

**Test Procedure**:
```python
import socket

sock = socket.socket()
sock.connect(('localhost', 8080))

# Send multiple requests on same connection
for i in range(5):
    request = f"GET /page{i}.html HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\n\r\n"
    sock.send(request.encode())
    response = sock.recv(4096)
    print(f"Request {i}: {len(response)} bytes")

sock.close()
```

**Expected Result**: All requests served on same connection (if keep-alive supported).

---

### 12. Large File Tests

#### Test 12.1: Large File Download
**Purpose**: Verify server can serve large files efficiently.

**Test Procedure**:
```bash
# Create large file
dd if=/dev/zero of=/var/www/html/large.bin bs=1M count=100

# Download it
time curl -o /dev/null http://localhost:8080/large.bin
```

**Expected Result**: File downloads successfully without errors or excessive memory use.

---

#### Test 12.2: Large File Upload
**Purpose**: Verify server can receive large uploads (within configured limits).

**Test Procedure**:
```bash
# Create large file (but under max body size)
dd if=/dev/zero of=upload.bin bs=1M count=50

# Upload it
time curl -X POST --data-binary @upload.bin http://localhost:8080/upload
```

**Expected Result**: Upload completes successfully.

---

#### Test 12.3: Streaming Large Responses
**Purpose**: Verify server doesn't buffer entire large responses in memory.

**Test Procedure**:
```bash
# Monitor memory while serving large file
watch -n 1 'ps aux | grep webserv'

# In another terminal, download large file
curl http://localhost:8080/large-file.bin > /dev/null
```

**Expected Result**: Memory usage doesn't spike to file size (should stream in chunks).

---

### 13. Stress and Resilience Tests

#### Test 13.1: Rapid Connection/Disconnection
**Purpose**: Verify server remains stable with rapid connects/disconnects.

**Test Procedure**:
```python
import socket
import time

for i in range(1000):
    try:
        sock = socket.socket()
        sock.connect(('localhost', 8080))
        sock.close()  # Immediate close
    except Exception as e:
        print(f"Error on iteration {i}: {e}")
    
    if i % 100 == 0:
        print(f"Completed {i} iterations")
```

**Expected Result**: Server remains responsive, no crashes or resource leaks.

---

#### Test 13.2: Malformed Request Flood
**Purpose**: Verify server handles malformed requests gracefully.

**Test Procedure**:
```python
import socket

malformed_requests = [
    b"INVALID HTTP REQUEST\r\n\r\n",
    b"GET\r\n\r\n",
    b"GET / HTTP/9.9\r\n\r\n",
    b"GET / HTTP/1.1\r\n" + b"X" * 100000 + b"\r\n\r\n",  # Huge header
    b"\x00\x01\x02\x03",  # Binary garbage
]

for req in malformed_requests * 100:
    try:
        sock = socket.socket()
        sock.settimeout(5)
        sock.connect(('localhost', 8080))
        sock.send(req)
        sock.recv(1024)
        sock.close()
    except:
        pass
```

**Expected Result**: Server returns 400 Bad Request or closes connection, never crashes.

---

#### Test 13.3: Resource Exhaustion Attempts
**Purpose**: Verify server degrades gracefully under extreme load.

**Test Procedure**:
```bash
# Try to exhaust file descriptors
ulimit -n 1024  # Limit to 1024 FDs
./webserv config.conf &

# Open many connections
for i in {1..2000}; do
    (sleep 100 | telnet localhost 8080 &)
done

# Server should still respond to new requests (maybe with 503)
curl http://localhost:8080/
```

**Expected Result**: Server returns 503 Service Unavailable when overloaded but doesn't crash.

---

#### Test 13.4: Long-Running Stability
**Purpose**: Verify server runs stably for extended periods.

**Test Procedure**:
```bash
./webserv config.conf &

# Run continuous load for hours
while true; do
    curl http://localhost:8080/ > /dev/null
    sleep 1
done

# Monitor for memory leaks, crashes, or degradation
watch -n 60 'ps aux | grep webserv'
```

**Expected Result**: Server remains stable, memory doesn't grow, no crashes.

---

## Error Handling Tests

### 14. Client Error Handling

#### Test 14.1: Malformed Requests
**Purpose**: Verify proper handling of various malformed requests.

**Test Cases**:
```python
import socket

test_cases = [
    # Missing HTTP version
    (b"GET /\r\n\r\n", "Missing HTTP version"),
    
    # Invalid HTTP version
    (b"GET / HTTP/2.0\r\n\r\n", "Unsupported HTTP version"),
    
    # No Host header (required in HTTP/1.1)
    (b"GET / HTTP/1.1\r\n\r\n", "Missing Host header"),
    
    # Invalid header format
    (b"GET / HTTP/1.1\r\nInvalid-Header\r\n\r\n", "Malformed header"),
    
    # Invalid method
    (b"INVALID / HTTP/1.1\r\nHost: localhost\r\n\r\n", "Unknown method"),
]

for request, description in test_cases:
    sock = socket.socket()
    sock.connect(('localhost', 8080))
    sock.send(request)
    response = sock.recv(4096).decode()
    print(f"{description}: {response.split()[1]}")  # Print status code
    sock.close()
```

**Expected Results**: 400 Bad Request for most cases, possibly 505 for version issues.

---

#### Test 14.2: Content-Length Mismatch
**Purpose**: Verify handling when Content-Length doesn't match body.

**Test Procedure**:
```python
import socket

# Content-Length says 10, but send 5
sock = socket.socket()
sock.connect(('localhost', 8080))
request = (
    b"POST / HTTP/1.1\r\n"
    b"Host: localhost\r\n"
    b"Content-Length: 10\r\n"
    b"\r\n"
    b"hello"  # Only 5 bytes
)
sock.send(request)
time.sleep(5)  # Server should timeout waiting for rest
response = sock.recv(4096)
print(response.decode())
```

**Expected Result**: Server either times out and closes connection, or returns 400.

---

#### Test 14.3: Oversized Headers
**Purpose**: Verify protection against huge headers.

**Test Procedure**:
```python
import socket

sock = socket.socket()
sock.connect(('localhost', 8080))

# Send request with very large header
huge_header = "X-Large-Header: " + ("A" * 100000) + "\r\n"
request = (
    "GET / HTTP/1.1\r\n"
    "Host: localhost\r\n" +
    huge_header +
    "\r\n"
)
sock.send(request.encode())
response = sock.recv(4096)
print(response.decode())
```

**Expected Result**: 400 Bad Request or connection closed.

---

### 15. File System Error Handling

#### Test 15.1: Permission Denied
**Purpose**: Verify handling of files without read permissions.

**Test Procedure**:
```bash
# Create file without read permission
touch /var/www/html/forbidden.txt
chmod 000 /var/www/html/forbidden.txt

# Try to access it
curl -v http://localhost:8080/forbidden.txt
```

**Expected Result**: 403 Forbidden.

---

#### Test 15.2: Symbolic Link Handling
**Purpose**: Verify safe handling of symbolic links.

**Test Procedure**:
```bash
# Create symlink outside document root
ln -s /etc/passwd /var/www/html/passwd

# Try to access it
curl -v http://localhost:8080/passwd
```

**Expected Result**: Either follow safely (if configured) or return 403/404.

---

#### Test 15.3: Directory Traversal Attempts
**Purpose**: Verify protection against directory traversal attacks.

**Test Procedure**:
```bash
curl http://localhost:8080/../../../etc/passwd
curl http://localhost:8080/....//....//....//etc/passwd
curl http://localhost:8080/%2e%2e%2f%2e%2e%2f%2e%2e%2fetc%2fpasswd
```

**Expected Result**: 400/403/404, never serve files outside document root.

---

### 16. CGI Error Handling

#### Test 16.1: Non-Existent CGI Script
**Purpose**: Verify handling when CGI script doesn't exist.

**Test Procedure**:
```bash
curl http://localhost:8080/nonexistent.php
```

**Expected Result**: 404 Not Found.

---

#### Test 16.2: Non-Executable CGI Script
**Purpose**: Verify handling when CGI script lacks execute permission.

**Test Procedure**:
```bash
# Create PHP file without execute permission
echo '<?php echo "test"; ?>' > /var/www/cgi-bin/test.php
chmod 644 /var/www/cgi-bin/test.php

curl http://localhost:8080/cgi-bin/test.php
```

**Expected Result**: 500 Internal Server Error or 403 Forbidden.

---

#### Test 16.3: CGI Timeout
**Purpose**: Verify handling when CGI script runs too long.

**Test Script** (`slow.php`):
```php
<?php
sleep(300);  // Sleep for 5 minutes
echo "Done";
?>
```

**Test Procedure**:
```bash
curl http://localhost:8080/slow.php
```

**Expected Result**: Request times out and returns 504 Gateway Timeout.

---

## Bonus Features Tests

### 17. Cookie and Session Management Tests

#### Test 17.1: Cookie Setting and Reading
**Purpose**: Verify server can set and read cookies.

**Test Procedure**:
```bash
# Server sets cookie
curl -v http://localhost:8080/set-cookie

# Client sends cookie back
curl -v -H "Cookie: session=abc123" http://localhost:8080/read-cookie
```

**Expected Result**: 
- Set-Cookie header in first response
- Server recognizes cookie in second request

---

#### Test 17.2: Session Persistence
**Purpose**: Verify session data persists across requests.

**Test Procedure**:
```bash
# Create session and get session ID
SESSION=$(curl -v http://localhost:8080/login 2>&1 | grep -i set-cookie | cut -d' ' -f3)

# Use session in subsequent requests
curl -H "Cookie: $SESSION" http://localhost:8080/profile
curl -H "Cookie: $SESSION" http://localhost:8080/dashboard
```

**Expected Result**: Session maintained across multiple requests.

---

### 18. Additional CGI Types

#### Test 18.1: Perl CGI Support
**Purpose**: Verify support for Perl scripts.

**Test Script** (`test.pl`):
```perl
#!/usr/bin/perl
print "Content-Type: text/plain\n\n";
print "Hello from Perl CGI\n";
print "Query: $ENV{'QUERY_STRING'}\n";
```

**Test Procedure**:
```bash
chmod +x test.pl
curl http://localhost:8080/test.pl?name=test
```

**Expected Result**: Perl script executes successfully.

---

#### Test 18.2: Ruby CGI Support
**Purpose**: Verify support for Ruby scripts.

**Test Script** (`test.rb`):
```ruby
#!/usr/bin/env ruby
puts "Content-Type: text/plain\n\n"
puts "Hello from Ruby CGI"
puts "Query: #{ENV['QUERY_STRING']}"
```

**Test Procedure**:
```bash
chmod +x test.rb
curl http://localhost:8080/test.rb?name=test
```

**Expected Result**: Ruby script executes successfully.

---

## Test Execution Summary

### Running All Tests

Create a test script that runs all tests systematically:

```bash
#!/bin/bash

echo "=== Starting Webserv Test Suite ==="

# Start server
./webserv test.conf &
SERVER_PID=$!
sleep 2

# Check if server started
if ! kill -0 $SERVER_PID 2>/dev/null; then
    echo "FAIL: Server failed to start"
    exit 1
fi

echo "Server started with PID $SERVER_PID"

# Run test categories
run_test_category "Core Requirements" ./tests/core_tests.sh
run_test_category "HTTP Protocol" ./tests/http_tests.sh
run_test_category "Configuration" ./tests/config_tests.sh
run_test_category "CGI" ./tests/cgi_tests.sh
run_test_category "Performance" ./tests/performance_tests.sh
run_test_category "Error Handling" ./tests/error_tests.sh

# Cleanup
kill $SERVER_PID
wait $SERVER_PID 2>/dev/null

echo "=== Test Suite Complete ==="
```

### Test Checklist

- [ ] Server compiles with no warnings
- [ ] Valgrind shows no memory leaks
- [ ] Non-blocking I/O verified
- [ ] Single poll() loop confirmed
- [ ] GET, POST, DELETE methods work
- [ ] Correct HTTP status codes
- [ ] Configuration file parsed correctly
- [ ] Multiple ports supported
- [ ] CGI scripts execute properly
- [ ] CGI environment variables correct
- [ ] Chunked requests unchunked for CGI
- [ ] File uploads work
- [ ] Error pages display correctly
- [ ] Request timeouts implemented
- [ ] Concurrent connections handled
- [ ] Stress tests pass
- [ ] Browser compatibility verified
- [ ] No crashes under any conditions

---

## Expected Outcomes

### Mandatory Requirements
All tests in sections 1-16 must pass for the project to be considered complete.

### Bonus Requirements
Tests in section 17-18 only need to pass if implementing bonus features.

### Common Failure Points
1. **Not using non-blocking I/O properly** - Immediate 0 grade
2. **Memory leaks** - Flag during evaluation
3. **Server crashes** - Immediate 0 grade
4. **Incorrect HTTP status codes** - Partial credit
5. **CGI environment variables missing** - CGI section fails
6. **Not unchunking for CGI** - CGI section fails

### Performance Benchmarks
While specific numbers aren't required, your server should:
- Handle at least 100 concurrent connections
- Serve static files at reasonable speeds
- Not leak memory over thousands of requests
- Remain responsive under load
