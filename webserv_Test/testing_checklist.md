# Webserv Testing Guide and Checklist

## Quick Start

### 1. Preparation
```bash
# Compile your server
make

# Create test directories
mkdir -p /var/www/html
mkdir -p /var/www/cgi-bin
mkdir -p /var/www/uploads
mkdir -p /var/www/static

# Create simple test file
echo "<h1>Hello, Webserv!</h1>" > /var/www/html/index.html

# Make test script executable
chmod +x webserv_tests.py
```

### 2. Start Server
```bash
./webserv config.conf
```

### 3. Run Tests
```bash
# In another terminal
python3 webserv_tests.py
```

---

## Manual Testing Checklist

### ✅ Basic Functionality

#### Server Startup
- [ ] Server compiles without warnings
- [ ] Server starts without errors
- [ ] Server binds to configured ports
- [ ] Server accepts configuration file path
- [ ] Server has default configuration if none provided

**Test Commands:**
```bash
./webserv config.conf
netstat -tuln | grep 8080  # Verify port is listening
```

---

### ✅ HTTP GET Method

#### Basic GET
- [ ] Returns 200 OK for existing files
- [ ] Returns proper Content-Type header
- [ ] Returns correct Content-Length
- [ ] Serves complete file content
- [ ] Returns 404 for non-existent files

**Test Commands:**
```bash
curl -v http://localhost:8080/index.html
curl -v http://localhost:8080/nonexistent.html
curl -I http://localhost:8080/  # HEAD-like request
```

#### GET with Query Parameters
- [ ] Accepts query parameters
- [ ] Preserves query string
- [ ] Passes query to CGI correctly

**Test Commands:**
```bash
curl "http://localhost:8080/test.php?name=John&age=30"
curl "http://localhost:8080/?search=test&page=2"
```

---

### ✅ HTTP POST Method

#### Basic POST
- [ ] Accepts POST requests
- [ ] Reads Content-Length header
- [ ] Receives complete body
- [ ] Returns appropriate status (200/201/204)

**Test Commands:**
```bash
curl -X POST -d "name=John&age=30" http://localhost:8080/form
curl -X POST -H "Content-Type: application/json" \
     -d '{"name":"John","age":30}' http://localhost:8080/api
```

#### POST with Large Body
- [ ] Handles bodies up to configured limit
- [ ] Returns 413 when exceeding limit
- [ ] Doesn't crash with huge bodies

**Test Commands:**
```bash
# Create 1MB file
dd if=/dev/zero of=1mb.bin bs=1M count=1

# Upload it
curl -X POST --data-binary @1mb.bin http://localhost:8080/upload

# Try to exceed limit (if limit is 10MB, try 11MB)
dd if=/dev/zero of=11mb.bin bs=1M count=11
curl -X POST --data-binary @11mb.bin http://localhost:8080/upload
```

#### Chunked Transfer Encoding
- [ ] Accepts chunked requests
- [ ] Properly decodes chunks
- [ ] Passes unchunked data to CGI

**Test Python Script:**
```python
import socket

sock = socket.socket()
sock.connect(('localhost', 8080))

request = (
    b"POST /chunked.php HTTP/1.1\r\n"
    b"Host: localhost\r\n"
    b"Transfer-Encoding: chunked\r\n"
    b"\r\n"
    b"5\r\n"
    b"Hello\r\n"
    b"7\r\n"
    b", World\r\n"
    b"0\r\n"
    b"\r\n"
)
sock.send(request)
print(sock.recv(4096).decode())
```

---

### ✅ HTTP DELETE Method

#### Basic DELETE
- [ ] Accepts DELETE requests
- [ ] Actually deletes files
- [ ] Returns appropriate status
- [ ] Returns 404 for non-existent files
- [ ] Checks permissions before deleting

**Test Commands:**
```bash
# Create test file
echo "delete me" > /var/www/uploads/test.txt

# Delete it
curl -X DELETE http://localhost:8080/uploads/test.txt

# Verify deletion
curl -v http://localhost:8080/uploads/test.txt  # Should 404

# Try to delete non-existent
curl -X DELETE http://localhost:8080/uploads/nonexistent.txt
```

---

### ✅ HTTP Status Codes

#### Success Codes (2xx)
- [ ] 200 OK for successful GET
- [ ] 201 Created for successful POST (optional)
- [ ] 204 No Content for successful DELETE

#### Redirection (3xx)
- [ ] 301 Moved Permanently (if configured)
- [ ] 302 Found (if configured)
- [ ] Location header present

**Test Commands:**
```bash
# If redirect configured from /old to /new
curl -v http://localhost:8080/old-page  # Should show 301/302
curl -L http://localhost:8080/old-page  # Should follow redirect
```

#### Client Errors (4xx)
- [ ] 400 Bad Request for malformed requests
- [ ] 403 Forbidden for permission denied
- [ ] 404 Not Found for missing resources
- [ ] 405 Method Not Allowed for unsupported methods
- [ ] 413 Payload Too Large for oversized bodies
- [ ] 414 URI Too Long for huge URIs

**Test Commands:**
```bash
# 404 Test
curl -w "%{http_code}" http://localhost:8080/nonexistent

# 405 Test (if PUT not allowed)
curl -w "%{http_code}" -X PUT http://localhost:8080/

# 413 Test (exceed body size limit)
dd if=/dev/zero bs=1M count=100 | curl -X POST --data-binary @- http://localhost:8080/upload

# 414 Test (very long URI)
curl -w "%{http_code}" http://localhost:8080/$(python3 -c "print('a'*10000)")

# 400 Test (malformed request)
echo -e "INVALID\r\n\r\n" | nc localhost 8080
```

#### Server Errors (5xx)
- [ ] 500 Internal Server Error for CGI failures
- [ ] 501 Not Implemented for unsupported features
- [ ] 503 Service Unavailable under extreme load

---

### ✅ HTTP Headers

#### Request Headers
- [ ] Handles Host header (required in HTTP/1.1)
- [ ] Handles Content-Length
- [ ] Handles Content-Type
- [ ] Handles Connection (keep-alive/close)
- [ ] Handles Transfer-Encoding
- [ ] Handles custom headers

**Test Commands:**
```bash
# With headers
curl -H "Host: example.com" \
     -H "X-Custom: value" \
     -H "User-Agent: test-client" \
     http://localhost:8080/

# Without Host (should fail in HTTP/1.1)
echo -e "GET / HTTP/1.1\r\n\r\n" | nc localhost 8080
```

#### Response Headers
- [ ] Includes Date header
- [ ] Includes Content-Type
- [ ] Includes Content-Length (when known)
- [ ] Includes Server header
- [ ] Includes appropriate status line

**Test Commands:**
```bash
curl -I http://localhost:8080/
```

---

### ✅ Configuration File

#### Multiple Servers
- [ ] Supports multiple server blocks
- [ ] Different ports work independently
- [ ] Different roots per server

**Test Commands:**
```bash
# Test each configured port
curl http://localhost:8080/
curl http://localhost:8081/
curl http://localhost:8082/
```

#### Error Pages
- [ ] Custom 404 page displays
- [ ] Custom 50x pages display
- [ ] Default pages when custom not configured

**Test Commands:**
```bash
curl http://localhost:8080/nonexistent  # Should show custom 404
```

#### Body Size Limit
- [ ] Enforces client_max_body_size
- [ ] Returns 413 when exceeded
- [ ] Different limits per location work

#### Location Blocks
- [ ] Root directive works
- [ ] Index files served correctly
- [ ] Autoindex on/off works
- [ ] Method restrictions work
- [ ] Redirects work

**Test Commands:**
```bash
# Test directory listing
curl http://localhost:8080/static/

# Test method restriction
curl -X DELETE http://localhost:8080/  # Should 405 if restricted
```

---

### ✅ CGI Functionality

#### Basic CGI Execution
- [ ] PHP scripts execute
- [ ] Python scripts execute
- [ ] Other CGI types (if supported)
- [ ] Scripts run in correct directory

**Test Commands:**
```bash
# PHP
curl http://localhost:8080/test.php

# Python
curl http://localhost:8080/test.py

# With query parameters
curl "http://localhost:8080/test.php?name=John&value=123"
```

#### CGI Environment Variables
- [ ] REQUEST_METHOD set correctly
- [ ] QUERY_STRING passed
- [ ] CONTENT_LENGTH for POST
- [ ] CONTENT_TYPE for POST
- [ ] SCRIPT_FILENAME correct
- [ ] HTTP headers passed as HTTP_*
- [ ] SERVER_* variables set

**Test Commands:**
```bash
curl http://localhost:8080/env.php
curl -X POST -d "data" http://localhost:8080/env.php
curl -H "X-Custom: test" http://localhost:8080/env.php
```

#### CGI POST Handling
- [ ] Request body passed via stdin
- [ ] Chunked requests unchunked
- [ ] Content-Length honored
- [ ] Large POSTs work

**Test Commands:**
```bash
echo "test data" | curl -X POST --data-binary @- http://localhost:8080/post.php

# Chunked
curl -X POST -H "Transfer-Encoding: chunked" \
     --data "Hello, World" http://localhost:8080/chunked.php
```

#### CGI Response Handling
- [ ] Parses CGI headers
- [ ] Handles CGI without Content-Length
- [ ] Detects CGI EOF correctly
- [ ] Returns CGI errors as 500

---

### ✅ File Operations

#### Static File Serving
- [ ] HTML files served correctly
- [ ] CSS files with correct MIME type
- [ ] JavaScript files correctly
- [ ] Images served correctly
- [ ] Binary files work

**Test Commands:**
```bash
curl http://localhost:8080/index.html
curl http://localhost:8080/style.css
curl http://localhost:8080/script.js
curl http://localhost:8080/image.png > test.png
```

#### File Upload
- [ ] Files upload successfully
- [ ] Saved to configured location
- [ ] Correct permissions
- [ ] Multiple uploads work

**Test Commands:**
```bash
echo "test content" > upload.txt
curl -X POST -F "file=@upload.txt" http://localhost:8080/upload
ls -la /var/www/uploads/
```

#### Directory Handling
- [ ] Index file served for directories
- [ ] Directory listing when enabled
- [ ] 403 when listing disabled and no index

**Test Commands:**
```bash
curl http://localhost:8080/
curl http://localhost:8080/static/  # If autoindex on
```

---

### ✅ Connection Handling

#### Non-Blocking I/O
- [ ] Uses non-blocking sockets
- [ ] Single poll()/select()/epoll()/kqueue()
- [ ] No read/write without readiness check

**Verification:**
```bash
# Code review
grep -r "fcntl.*O_NONBLOCK" src/
grep -r "poll\|select\|epoll\|kqueue" src/

# Runtime check with strace
strace -e poll,select,epoll_wait,kevent ./webserv config.conf
```

#### Concurrent Connections
- [ ] Handles multiple simultaneous connections
- [ ] No blocking between clients
- [ ] Resources cleaned up properly

**Test Commands:**
```bash
# Apache Bench
ab -n 1000 -c 50 http://localhost:8080/

# Multiple curl in background
for i in {1..20}; do curl http://localhost:8080/ > /dev/null 2>&1 & done
```

#### Keep-Alive
- [ ] Supports persistent connections
- [ ] Multiple requests on same connection
- [ ] Connection: close honored

**Test Python Script:**
```python
import socket

sock = socket.socket()
sock.connect(('localhost', 8080))

for i in range(3):
    request = f"GET /page{i}.html HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\n\r\n"
    sock.send(request.encode())
    response = sock.recv(4096)
    print(f"Request {i}: received {len(response)} bytes")
    
sock.close()
```

#### Timeouts
- [ ] Incomplete requests timeout
- [ ] Slow clients timeout
- [ ] Connections don't hang forever

---

### ✅ Error Handling

#### Malformed Requests
- [ ] Handles invalid HTTP version
- [ ] Handles missing request line parts
- [ ] Handles invalid headers
- [ ] Never crashes
- [ ] Returns appropriate errors

**Test Python Script:**
```python
import socket

test_cases = [
    b"GET\r\n\r\n",  # Incomplete
    b"INVALID HTTP REQUEST\r\n\r\n",  # Garbage
    b"GET / HTTP/9.9\r\n\r\n",  # Invalid version
    b"GET / HTTP/1.1\r\nBad-Header\r\n\r\n",  # Malformed header
]

for req in test_cases:
    sock = socket.socket()
    sock.connect(('localhost', 8080))
    sock.send(req)
    try:
        resp = sock.recv(1024)
        print(f"Response: {resp[:50]}")
    except:
        print("Connection closed")
    sock.close()
```

#### File System Errors
- [ ] Handles permission denied (403)
- [ ] Handles non-existent files (404)
- [ ] Safe symlink handling
- [ ] No directory traversal attacks

**Test Commands:**
```bash
# Permission test
touch /var/www/html/forbidden.txt
chmod 000 /var/www/html/forbidden.txt
curl http://localhost:8080/forbidden.txt  # Should 403

# Directory traversal
curl http://localhost:8080/../../../etc/passwd  # Should deny
curl http://localhost:8080/%2e%2e%2f%2e%2e%2fetc%2fpasswd  # Should deny
```

#### Resource Exhaustion
- [ ] Handles many connections gracefully
- [ ] Doesn't crash when out of FDs
- [ ] Returns 503 when overloaded
- [ ] Recovers after load decreases

---

### ✅ Memory Management

#### Leak Detection
- [ ] No memory leaks (valgrind clean)
- [ ] No leaks after 1000+ requests
- [ ] Resources freed on disconnect

**Test Commands:**
```bash
# Run with valgrind
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         ./webserv config.conf

# In another terminal, generate load
for i in {1..1000}; do curl http://localhost:8080/ > /dev/null; done

# Stop server (Ctrl+C) and check valgrind output
```

#### Memory Usage
- [ ] Stable under sustained load
- [ ] Doesn't grow unbounded
- [ ] Efficient with large files

**Test Commands:**
```bash
# Monitor memory
watch -n 1 'ps aux | grep webserv'

# Generate sustained load
ab -n 10000 -c 50 http://localhost:8080/
```

---

### ✅ Browser Compatibility

#### Manual Browser Testing
- [ ] Works with Chrome/Chromium
- [ ] Works with Firefox
- [ ] Works with Safari
- [ ] Static pages render correctly
- [ ] Forms submit correctly
- [ ] File uploads work

**Test Steps:**
1. Open http://localhost:8080 in browser
2. Navigate between pages
3. Submit a form (if you have one)
4. Upload a file (if supported)
5. Check console for errors

---

### ✅ Stress Testing

#### Load Testing
- [ ] Handles sustained high load
- [ ] No crashes after hours of requests
- [ ] Performance doesn't degrade significantly

**Test Commands:**
```bash
# Extended load test
ab -n 100000 -c 100 -t 3600 http://localhost:8080/

# Siege
siege -c 50 -t 10M http://localhost:8080/
```

#### Rapid Connections
- [ ] Handles rapid connect/disconnect
- [ ] No resource leaks
- [ ] Remains responsive

**Test Python Script:**
```python
for i in range(10000):
    sock = socket.socket()
    sock.connect(('localhost', 8080))
    sock.close()
    if i % 1000 == 0:
        print(f"Iteration {i}")
```

---

## Automated Test Results

When running `webserv_tests.py`, you should see:

```
=== Webserv Test Suite ===

>>> CORE REQUIREMENTS TESTS
✓ Server responds to basic request
✓ Concurrent connection handling
✓ Slow client doesn't block server

>>> HTTP PROTOCOL TESTS
✓ GET method
✓ POST method
✓ DELETE method
✓ Unsupported HTTP method rejection
✓ 404 Not Found
✓ Malformed request handling
✓ Missing Host header (HTTP/1.1)
✓ Chunked transfer encoding
✓ Large request body handling

>>> CGI TESTS
✓ Basic CGI execution
✓ CGI query string handling
✓ CGI POST data handling

>>> STRESS TESTS
✓ Rapid connect/disconnect stress test
✓ High concurrent connection load

Test Summary:
  Passed: 17/17
  Failed: 0/17
  Warnings: 0
```

---

## Common Issues and Solutions

### Issue: Server won't start
- Check if port is already in use: `netstat -tuln | grep 8080`
- Check configuration file syntax
- Check file permissions

### Issue: 404 for everything
- Verify root directory exists and is readable
- Check configuration file paths
- Ensure index.html exists

### Issue: CGI not working
- Verify CGI interpreter is installed: `which php-cgi`
- Check script permissions: `chmod +x script.py`
- Check script shebang line: `#!/usr/bin/env python3`
- Verify cgi_pass path in configuration

### Issue: Memory leaks
- Check for unclosed file descriptors
- Ensure all malloc() has corresponding free()
- Check for leaked connections
- Run valgrind to identify leaks

### Issue: Server crashes
- Check for null pointer dereferences
- Verify buffer overflow protection
- Test with various input sizes
- Use debugger to find crash location

---

## Final Pre-Submission Checklist

- [ ] All tests pass
- [ ] No compiler warnings
- [ ] Valgrind shows no leaks
- [ ] Works in browser
- [ ] Configuration file documented
- [ ] README.md explains how to run
- [ ] Makefile works correctly
- [ ] Code follows C++98 standard
- [ ] No external libraries (except allowed)
- [ ] Subject requirements all met
