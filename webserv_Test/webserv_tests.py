#!/usr/bin/env python3
"""
Webserv Automated Test Suite
Comprehensive testing for the 42 webserv project
"""

import socket
import time
import subprocess
import os
import sys
import threading
from typing import Tuple, Optional, List
import signal

class Colors:
    GREEN = '\033[92m'
    RED = '\033[91m'
    YELLOW = '\033[93m'
    BLUE = '\033[94m'
    RESET = '\033[0m'
    BOLD = '\033[1m'

class TestResult:
    def __init__(self):
        self.passed = 0
        self.failed = 0
        self.warnings = 0
    
    def print_summary(self):
        total = self.passed + self.failed
        print(f"\n{Colors.BOLD}{'='*60}{Colors.RESET}")
        print(f"{Colors.BOLD}Test Summary:{Colors.RESET}")
        print(f"  {Colors.GREEN}Passed: {self.passed}/{total}{Colors.RESET}")
        print(f"  {Colors.RED}Failed: {self.failed}/{total}{Colors.RESET}")
        print(f"  {Colors.YELLOW}Warnings: {self.warnings}{Colors.RESET}")
        print(f"{Colors.BOLD}{'='*60}{Colors.RESET}")

result = TestResult()

def print_test(name: str):
    """Print test name"""
    print(f"\n{Colors.BLUE}▶ Testing: {Colors.BOLD}{name}{Colors.RESET}")

def print_pass(message: str = "PASSED"):
    """Print success message"""
    global result
    result.passed += 1
    print(f"  {Colors.GREEN}✓ {message}{Colors.RESET}")

def print_fail(message: str = "FAILED"):
    """Print failure message"""
    global result
    result.failed += 1
    print(f"  {Colors.RED}✗ {message}{Colors.RESET}")

def print_warn(message: str):
    """Print warning message"""
    global result
    result.warnings += 1
    print(f"  {Colors.YELLOW}⚠ {message}{Colors.RESET}")

def send_http_request(host: str, port: int, request: bytes, timeout: int = 5) -> Tuple[Optional[bytes], Optional[str]]:
    """Send HTTP request and return response"""
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(timeout)
        sock.connect((host, port))
        sock.send(request)
        response = b""
        while True:
            chunk = sock.recv(4096)
            if not chunk:
                break
            response += chunk
            # Check if we have complete response
            if b"\r\n\r\n" in response:
                # Check for Content-Length to know when to stop
                headers = response.split(b"\r\n\r\n")[0]
                if b"Content-Length:" in headers:
                    for line in headers.split(b"\r\n"):
                        if line.startswith(b"Content-Length:"):
                            length = int(line.split(b":")[1].strip())
                            body_start = response.find(b"\r\n\r\n") + 4
                            if len(response) >= body_start + length:
                                break
        sock.close()
        return response, None
    except socket.timeout:
        return None, "Timeout"
    except Exception as e:
        return None, str(e)

def get_status_code(response: bytes) -> Optional[int]:
    """Extract status code from HTTP response"""
    try:
        status_line = response.split(b"\r\n")[0].decode()
        return int(status_line.split()[1])
    except:
        return None

# ============================================================================
# CORE REQUIREMENTS TESTS
# ============================================================================

def test_server_responds():
    """Test 1.1: Server responds to basic request"""
    print_test("Server responds to basic request")
    
    request = b"GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"
    response, error = send_http_request("localhost", 8080, request)
    
    if error:
        print_fail(f"No response: {error}")
        return False
    
    if response:
        print_pass("Server responded")
        return True
    
    print_fail("Empty response")
    return False

def test_concurrent_connections():
    """Test 1.2: Server handles concurrent connections"""
    print_test("Concurrent connection handling")
    
    def make_request():
        request = b"GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"
        response, error = send_http_request("localhost", 8080, request)
        return response is not None and error is None
    
    threads = []
    results = []
    
    for i in range(50):
        t = threading.Thread(target=lambda: results.append(make_request()))
        threads.append(t)
        t.start()
    
    for t in threads:
        t.join()
    
    success_rate = sum(results) / len(results) * 100
    
    if success_rate >= 95:
        print_pass(f"Handled {len(results)} concurrent connections ({success_rate:.1f}% success)")
        return True
    else:
        print_fail(f"Only {success_rate:.1f}% successful")
        return False

def test_slow_client():
    """Test 1.3: Server doesn't block on slow client"""
    print_test("Slow client doesn't block server")
    
    def slow_client():
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.connect(("localhost", 8080))
            request = b"GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"
            for byte in request:
                sock.send(bytes([byte]))
                time.sleep(0.1)  # Slow send
            time.sleep(30)  # Keep connection open
            sock.close()
        except:
            pass
    
    # Start slow client
    threading.Thread(target=slow_client, daemon=True).start()
    time.sleep(0.5)
    
    # Fast client should still complete quickly
    start = time.time()
    request = b"GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"
    response, error = send_http_request("localhost", 8080, request, timeout=2)
    duration = time.time() - start
    
    if response and duration < 1.0:
        print_pass(f"Fast client completed in {duration:.2f}s")
        return True
    else:
        print_fail(f"Fast client took {duration:.2f}s (blocked by slow client?)")
        return False

def test_request_timeout():
    """Test 2.1: Incomplete requests timeout"""
    print_test("Request timeout on incomplete request")
    
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(65)  # Wait up to 65 seconds
        sock.connect(("localhost", 8080))
        
        # Send incomplete request
        sock.send(b"GET / HTTP/1.1\r\n")
        
        # Wait for timeout
        start = time.time()
        try:
            data = sock.recv(1024)
            duration = time.time() - start
            
            if duration > 5 and duration < 120:  # Between 5 and 120 seconds
                print_pass(f"Connection timed out after {duration:.1f}s")
                sock.close()
                return True
            else:
                print_warn(f"Timeout occurred but at unusual time: {duration:.1f}s")
                sock.close()
                return True
        except socket.timeout:
            print_pass("Connection timed out (>60s)")
            sock.close()
            return True
    except Exception as e:
        print_fail(f"Error: {e}")
        return False

# ============================================================================
# HTTP PROTOCOL TESTS
# ============================================================================

def test_get_method():
    """Test 4.1: GET method works"""
    print_test("GET method")
    
    request = b"GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"
    response, error = send_http_request("localhost", 8080, request)
    
    if not response:
        print_fail(f"No response: {error}")
        return False
    
    status = get_status_code(response)
    if status == 200:
        print_pass(f"GET returned 200 OK")
        return True
    elif status == 404:
        print_warn(f"GET returned 404 (no index file?)")
        return True
    else:
        print_fail(f"Unexpected status: {status}")
        return False

def test_post_method():
    """Test 4.2: POST method works"""
    print_test("POST method")
    
    body = b"name=test&value=123"
    request = (
        b"POST /upload HTTP/1.1\r\n"
        b"Host: localhost\r\n"
        b"Content-Length: " + str(len(body)).encode() + b"\r\n"
        b"Content-Type: application/x-www-form-urlencoded\r\n"
        b"\r\n" + body
    )
    
    response, error = send_http_request("localhost", 8080, request)
    
    if not response:
        print_fail(f"No response: {error}")
        return False
    
    status = get_status_code(response)
    if status in [200, 201, 204]:
        print_pass(f"POST returned {status}")
        return True
    elif status == 404:
        print_warn("POST returned 404 (route not configured?)")
        return True
    else:
        print_fail(f"Unexpected status: {status}")
        return False

def test_delete_method():
    """Test 4.3: DELETE method works"""
    print_test("DELETE method")
    
    request = b"DELETE /test.txt HTTP/1.1\r\nHost: localhost\r\n\r\n"
    response, error = send_http_request("localhost", 8080, request)
    
    if not response:
        print_fail(f"No response: {error}")
        return False
    
    status = get_status_code(response)
    if status in [200, 204, 404]:
        print_pass(f"DELETE returned {status}")
        return True
    elif status == 405:
        print_warn("DELETE not allowed (needs configuration)")
        return True
    else:
        print_fail(f"Unexpected status: {status}")
        return False

def test_unsupported_method():
    """Test 4.4: Unsupported methods rejected"""
    print_test("Unsupported HTTP method rejection")
    
    request = b"PUT / HTTP/1.1\r\nHost: localhost\r\n\r\n"
    response, error = send_http_request("localhost", 8080, request)
    
    if not response:
        print_fail(f"No response: {error}")
        return False
    
    status = get_status_code(response)
    if status == 405:
        print_pass("PUT returned 405 Method Not Allowed")
        return True
    elif status in [501, 400]:
        print_warn(f"Returned {status} instead of 405")
        return True
    else:
        print_fail(f"Unexpected status: {status}")
        return False

def test_404_not_found():
    """Test 5.3: 404 for non-existent resources"""
    print_test("404 Not Found")
    
    request = b"GET /this-file-definitely-does-not-exist-12345.html HTTP/1.1\r\nHost: localhost\r\n\r\n"
    response, error = send_http_request("localhost", 8080, request)
    
    if not response:
        print_fail(f"No response: {error}")
        return False
    
    status = get_status_code(response)
    if status == 404:
        print_pass("Returned 404 Not Found")
        return True
    else:
        print_fail(f"Expected 404, got {status}")
        return False

def test_malformed_request():
    """Test 14.1: Malformed request handling"""
    print_test("Malformed request handling")
    
    # No HTTP version
    request = b"GET /\r\n\r\n"
    response, error = send_http_request("localhost", 8080, request, timeout=3)
    
    if not response:
        print_warn("Server closed connection on malformed request")
        return True
    
    status = get_status_code(response)
    if status == 400:
        print_pass("Returned 400 Bad Request")
        return True
    else:
        print_warn(f"Returned {status} instead of 400")
        return True

def test_no_host_header():
    """Test HTTP/1.1 requires Host header"""
    print_test("Missing Host header (HTTP/1.1)")
    
    request = b"GET / HTTP/1.1\r\n\r\n"  # No Host header
    response, error = send_http_request("localhost", 8080, request, timeout=3)
    
    if not response:
        print_warn("Server closed connection")
        return True
    
    status = get_status_code(response)
    if status == 400:
        print_pass("Returned 400 Bad Request")
        return True
    elif status == 200:
        print_warn("Server accepted request without Host (lenient)")
        return True
    else:
        print_fail(f"Unexpected status: {status}")
        return False

def test_chunked_encoding():
    """Test 7.2: Chunked transfer encoding"""
    print_test("Chunked transfer encoding")
    
    request = (
        b"POST /upload HTTP/1.1\r\n"
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
    
    response, error = send_http_request("localhost", 8080, request)
    
    if not response:
        print_fail(f"No response: {error}")
        return False
    
    status = get_status_code(response)
    if status in [200, 201, 204]:
        print_pass(f"Chunked request handled ({status})")
        return True
    elif status == 404:
        print_warn("Endpoint not found, but chunking may work")
        return True
    else:
        print_fail(f"Unexpected status: {status}")
        return False

def test_large_request():
    """Test 7.3: Large request body"""
    print_test("Large request body handling")
    
    body = b"X" * (1024 * 1024)  # 1MB
    request = (
        b"POST /upload HTTP/1.1\r\n"
        b"Host: localhost\r\n"
        b"Content-Length: " + str(len(body)).encode() + b"\r\n"
        b"\r\n" + body
    )
    
    response, error = send_http_request("localhost", 8080, request, timeout=10)
    
    if not response:
        print_warn(f"No response: {error}")
        return True
    
    status = get_status_code(response)
    if status in [200, 201, 204, 413]:
        print_pass(f"Large body handled ({status})")
        return True
    elif status == 404:
        print_warn("Endpoint not found")
        return True
    else:
        print_fail(f"Unexpected status: {status}")
        return False

# ============================================================================
# CGI TESTS
# ============================================================================

def test_cgi_basic():
    """Test 10.1: Basic CGI execution"""
    print_test("Basic CGI execution")
    
    # This assumes a test.php exists
    request = b"GET /test.php HTTP/1.1\r\nHost: localhost\r\n\r\n"
    response, error = send_http_request("localhost", 8080, request)
    
    if not response:
        print_warn(f"No CGI script or CGI not configured")
        return True
    
    status = get_status_code(response)
    if status == 200:
        print_pass("CGI script executed")
        return True
    elif status == 404:
        print_warn("CGI script not found (create test.php)")
        return True
    else:
        print_fail(f"CGI failed with status {status}")
        return False

def test_cgi_query_string():
    """Test 10.2: CGI receives query string"""
    print_test("CGI query string handling")
    
    request = b"GET /env.php?param1=value1&param2=value2 HTTP/1.1\r\nHost: localhost\r\n\r\n"
    response, error = send_http_request("localhost", 8080, request)
    
    if not response:
        print_warn("CGI script not available")
        return True
    
    status = get_status_code(response)
    if status == 200:
        if b"QUERY_STRING" in response or b"param1" in response:
            print_pass("Query string passed to CGI")
            return True
        else:
            print_warn("CGI executed but query string handling unclear")
            return True
    else:
        print_warn(f"CGI returned {status}")
        return True

def test_cgi_post():
    """Test 10.3: CGI receives POST data"""
    print_test("CGI POST data handling")
    
    body = b"test=data"
    request = (
        b"POST /post.php HTTP/1.1\r\n"
        b"Host: localhost\r\n"
        b"Content-Length: " + str(len(body)).encode() + b"\r\n"
        b"Content-Type: application/x-www-form-urlencoded\r\n"
        b"\r\n" + body
    )
    
    response, error = send_http_request("localhost", 8080, request)
    
    if not response:
        print_warn("CGI POST script not available")
        return True
    
    status = get_status_code(response)
    if status == 200:
        print_pass("CGI POST handled")
        return True
    else:
        print_warn(f"CGI returned {status}")
        return True

# ============================================================================
# STRESS TESTS
# ============================================================================

def test_rapid_connections():
    """Test 13.1: Rapid connection/disconnection"""
    print_test("Rapid connect/disconnect stress test")
    
    failures = 0
    iterations = 100
    
    for i in range(iterations):
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(1)
            sock.connect(("localhost", 8080))
            sock.close()
        except:
            failures += 1
    
    success_rate = (iterations - failures) / iterations * 100
    
    if success_rate >= 95:
        print_pass(f"Handled {iterations} rapid connections ({success_rate:.1f}% success)")
        return True
    else:
        print_fail(f"Only {success_rate:.1f}% successful")
        return False

def test_many_connections():
    """Test 11.1: Many simultaneous connections"""
    print_test("High concurrent connection load")
    
    def make_request():
        try:
            request = b"GET / HTTP/1.1\r\nHost: localhost\r\n\r\n"
            response, error = send_http_request("localhost", 8080, request, timeout=10)
            return response is not None
        except:
            return False
    
    threads = []
    results = []
    num_connections = 100
    
    for i in range(num_connections):
        t = threading.Thread(target=lambda: results.append(make_request()))
        threads.append(t)
        t.start()
    
    for t in threads:
        t.join(timeout=15)
    
    success_rate = sum(results) / len(results) * 100
    
    if success_rate >= 90:
        print_pass(f"Handled {num_connections} connections ({success_rate:.1f}% success)")
        return True
    elif success_rate >= 70:
        print_warn(f"Handled {success_rate:.1f}% of {num_connections} connections")
        return True
    else:
        print_fail(f"Only {success_rate:.1f}% successful")
        return False

# ============================================================================
# MAIN TEST RUNNER
# ============================================================================

def run_all_tests():
    """Run all test suites"""
    print(f"\n{Colors.BOLD}{'='*60}{Colors.RESET}")
    print(f"{Colors.BOLD}Webserv Test Suite{Colors.RESET}")
    print(f"{Colors.BOLD}{'='*60}{Colors.RESET}")
    
    print(f"\n{Colors.BOLD}>>> CORE REQUIREMENTS TESTS{Colors.RESET}")
    test_server_responds()
    test_concurrent_connections()
    test_slow_client()
    # test_request_timeout()  # Takes 60+ seconds, uncomment for full test
    
    print(f"\n{Colors.BOLD}>>> HTTP PROTOCOL TESTS{Colors.RESET}")
    test_get_method()
    test_post_method()
    test_delete_method()
    test_unsupported_method()
    test_404_not_found()
    test_malformed_request()
    test_no_host_header()
    test_chunked_encoding()
    test_large_request()
    
    print(f"\n{Colors.BOLD}>>> CGI TESTS{Colors.RESET}")
    test_cgi_basic()
    test_cgi_query_string()
    test_cgi_post()
    
    print(f"\n{Colors.BOLD}>>> STRESS TESTS{Colors.RESET}")
    test_rapid_connections()
    test_many_connections()
    
    result.print_summary()

if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] == "--help":
        print("Usage: python3 webserv_tests.py")
        print("\nMake sure your webserv is running on localhost:8080")
        print("Start it with: ./webserv config.conf")
        sys.exit(0)
    
    # Check if server is running
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.settimeout(1)
        sock.connect(("localhost", 8080))
        sock.close()
    except:
        print(f"{Colors.RED}Error: Cannot connect to server on localhost:8080{Colors.RESET}")
        print("Please start your webserv first: ./webserv config.conf")
        sys.exit(1)
    
    try:
        run_all_tests()
    except KeyboardInterrupt:
        print(f"\n{Colors.YELLOW}Tests interrupted by user{Colors.RESET}")
        result.print_summary()
        sys.exit(1)
