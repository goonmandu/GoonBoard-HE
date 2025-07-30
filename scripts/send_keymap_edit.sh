#!/bin/bash

DEVICE=/dev/hidraw5  # Replace with the correct hidraw device for your board

# Bytes: [ReportID][row][key][val]
echo -ne '\x01\x00\x00\x13' > "$DEVICE"

# echo -ne '\x02\x00\x00\x0F' > "$DEVICE"