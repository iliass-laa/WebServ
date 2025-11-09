#!/usr/bin/python3
import sys

# Write HTTP header manually (with proper CRLF)
sys.stdout.write("Content-Type: image/png\r\n\r\n")
sys.stdout.flush()  # Make sure header is sent

# Now write raw binary image data
with open("/var/www/tmpSite/images/blood.png", "rb") as f:
    sys.stdout.buffer.write(f.read())