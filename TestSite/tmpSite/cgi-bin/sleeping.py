#!/usr/bin/env python3
import time
import sys

print("Content-Type: text/plain")
print("Transfer-Encoding: chunked")
print()
for i in range(5):
    print(f"Message {i}")
    sys.stdout.flush()
    time.sleep(1)


# print("Content-Length: chunked")