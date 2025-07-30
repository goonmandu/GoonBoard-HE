#!/bin/bash

# Keyboard is registered to /dev/hidraw8
DEVICE=/dev/hidraw5

# Bytes:  [ReportID]   [row] [key]  [val]
#          Edit key at  (2 ,  2)  to `P`
#            0x01      0x02  0x02   0x13
echo -ne '\x01\x02\x02\x07' > "$DEVICE"

# echo -ne '\x02\x00\x00\x0F' > "$DEVICE"