# Webserv CGI Test Scripts

## test.php - Basic CGI test
```php
<?php
header("Content-Type: text/plain");
echo "Hello from PHP CGI\n";
echo "Server: " . $_SERVER['SERVER_NAME'] . "\n";
echo "Method: " . $_SERVER['REQUEST_METHOD'] . "\n";
?>
```

## env.php - Environment variable test
```php
<?php
header("Content-Type: text/plain");
echo "=== CGI Environment Variables ===\n\n";

$important_vars = [
    'REQUEST_METHOD',
    'QUERY_STRING',
    'CONTENT_LENGTH',
    'CONTENT_TYPE',
    'SCRIPT_FILENAME',
    'SCRIPT_NAME',
    'REQUEST_URI',
    'SERVER_PROTOCOL',
    'SERVER_NAME',
    'SERVER_PORT',
    'PATH_INFO',
    'REMOTE_ADDR',
];

foreach ($important_vars as $var) {
    $value = isset($_SERVER[$var]) ? $_SERVER[$var] : '(not set)';
    echo "$var = $value\n";
}

echo "\n=== All HTTP Headers ===\n\n";
foreach ($_SERVER as $key => $value) {
    if (strpos($key, 'HTTP_') === 0) {
        echo "$key = $value\n";
    }
}

echo "\n=== Query Parameters ===\n\n";
foreach ($_GET as $key => $value) {
    echo "$key = $value\n";
}
?>
```

## post.php - POST data test
```php
<?php
header("Content-Type: text/plain");

echo "=== POST Data Test ===\n\n";

$input = file_get_contents('php://input');
echo "Received " . strlen($input) . " bytes\n";
echo "Content-Type: " . ($_SERVER['CONTENT_TYPE'] ?? 'not set') . "\n";
echo "Content-Length: " . ($_SERVER['CONTENT_LENGTH'] ?? 'not set') . "\n";
echo "\n=== Raw Input ===\n";
echo $input . "\n";

echo "\n=== Parsed POST ===\n";
foreach ($_POST as $key => $value) {
    echo "$key = $value\n";
}
?>
```

## chunked.php - Test chunked encoding handling
```php
<?php
header("Content-Type: text/plain");

$input = file_get_contents('php://input');
echo "=== Chunked Request Test ===\n\n";
echo "Received data: " . $input . "\n";
echo "Data length: " . strlen($input) . " bytes\n";

// If server properly unchunked, we should see the complete message
if (strpos($input, 'Hello, World') !== false) {
    echo "\nSUCCESS: Chunked data was properly decoded\n";
} else {
    echo "\nERROR: Data may not have been unchunked properly\n";
}
?>
```

## slow.php - Timeout test
```php
<?php
header("Content-Type: text/plain");
echo "Starting slow script...\n";
flush();

sleep(10);  // 10 second delay

echo "Script completed\n";
?>
```

## error.php - Error handling test
```php
<?php
// This script intentionally causes an error
header("Content-Type: text/plain");
trigger_error("Intentional error for testing", E_USER_ERROR);
?>
```

## upload.php - File upload test
```php
<?php
header("Content-Type: text/plain");

if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    if (isset($_FILES['file'])) {
        echo "File uploaded:\n";
        echo "  Name: " . $_FILES['file']['name'] . "\n";
        echo "  Size: " . $_FILES['file']['size'] . " bytes\n";
        echo "  Type: " . $_FILES['file']['type'] . "\n";
        echo "  Temp: " . $_FILES['file']['tmp_name'] . "\n";
    } else {
        $input = file_get_contents('php://input');
        echo "Raw POST data received: " . strlen($input) . " bytes\n";
    }
} else {
    echo "Use POST method to upload files\n";
}
?>
```

## redirect.php - Redirect test
```php
<?php
header("Location: /new-location.html", true, 301);
exit;
?>
```

## headers.php - Response header test
```php
<?php
header("Content-Type: text/plain");
header("X-Custom-Header: CustomValue");
header("X-Server: Webserv");

echo "Custom headers set\n";
echo "Check response headers to verify\n";
?>
```

---

## Python CGI Scripts

## test.py - Basic Python CGI
```python
#!/usr/bin/env python3
print("Content-Type: text/plain\n")
print("Hello from Python CGI")

import os
print(f"Method: {os.environ.get('REQUEST_METHOD', 'unknown')}")
print(f"Query: {os.environ.get('QUERY_STRING', 'none')}")
```

## env.py - Python environment test
```python
#!/usr/bin/env python3
import os

print("Content-Type: text/plain\n")
print("=== Python CGI Environment ===\n")

important_vars = [
    'REQUEST_METHOD',
    'QUERY_STRING',
    'CONTENT_LENGTH',
    'CONTENT_TYPE',
    'SCRIPT_FILENAME',
    'SCRIPT_NAME',
    'REQUEST_URI',
    'SERVER_PROTOCOL',
    'SERVER_NAME',
    'SERVER_PORT',
]

for var in important_vars:
    value = os.environ.get(var, '(not set)')
    print(f"{var} = {value}")

print("\n=== All Environment Variables ===\n")
for key, value in sorted(os.environ.items()):
    print(f"{key} = {value}")
```

## post.py - Python POST test
```python
#!/usr/bin/env python3
import sys
import os

print("Content-Type: text/plain\n")
print("=== Python POST Test ===\n")

content_length = os.environ.get('CONTENT_LENGTH', '0')
print(f"Content-Length: {content_length}")

if content_length:
    data = sys.stdin.read(int(content_length))
    print(f"Received {len(data)} bytes")
    print(f"\nData:\n{data}")
else:
    print("No POST data received")
```

---

## Setup Instructions

1. Create CGI directory:
```bash
mkdir -p /var/www/cgi-bin
```

2. Copy PHP scripts:
```bash
cp test.php env.php post.php chunked.php slow.php error.php upload.php redirect.php headers.php /var/www/cgi-bin/
```

3. Copy Python scripts:
```bash
cp test.py env.py post.py /var/www/cgi-bin/
chmod +x /var/www/cgi-bin/*.py
```

4. Install PHP-CGI (if not installed):
```bash
# Ubuntu/Debian
sudo apt-get install php-cgi

# macOS
brew install php
```

5. Test PHP-CGI:
```bash
php-cgi -v
```

---

## Manual Testing Commands

### Basic CGI:
```bash
curl http://localhost:8080/test.php
curl http://localhost:8080/test.py
```

### Environment variables:
```bash
curl http://localhost:8080/env.php?param1=value1&param2=value2
curl -H "X-Custom-Header: test" http://localhost:8080/env.php
```

### POST requests:
```bash
curl -X POST -d "name=John&age=30" http://localhost:8080/post.php
echo "test data" | curl -X POST --data-binary @- http://localhost:8080/post.py
```

### Chunked encoding:
```bash
curl -X POST -H "Transfer-Encoding: chunked" \
     --data "Hello, World" http://localhost:8080/chunked.php
```

### File upload:
```bash
echo "test content" > test.txt
curl -X POST -F "file=@test.txt" http://localhost:8080/upload.php
```

### Headers:
```bash
curl -I http://localhost:8080/headers.php
```

---

## Expected Results

### test.php / test.py:
```
Hello from PHP CGI
Server: localhost
Method: GET
```

### env.php with query:
```
REQUEST_METHOD = GET
QUERY_STRING = param1=value1&param2=value2
...
```

### post.php:
```
Received 13 bytes
Content-Type: application/x-www-form-urlencoded
Content-Length: 13

=== Raw Input ===
name=John&age=30
```

### chunked.php:
```
Received data: Hello, World
Data length: 12 bytes

SUCCESS: Chunked data was properly decoded
```
