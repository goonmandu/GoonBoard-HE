#!/bin/bash

DEVICE=/dev/hidraw8

# wait for the keyboard device to appear
while [ ! -c "$DEVICE" ]; do
  sleep 0.1
done

total=$((6 * 16))
count=1

for row in {0..5}; do
  for col in {0..15}; do
    echo "Sending $count / $total"

    case "$row,$col" in
      0,1) code=0x1a ;;   # W
      0,3) code=0x2c ;;   # Space
      2,0) code=0x04 ;;   # A
      2,1) code=0x16 ;;   # S
      2,2) code=0x07 ;;   # D
      1,3) code=0x09 ;;   # F
      *)    code=0x00 ;;   # (none)
    esac

    # build an escape sequence and send it
    esc=$(printf '\\x01\\x%02x\\x%02x\\x%02x' "$row" "$col" "$code")
    printf '%b' "$esc" > "$DEVICE"

    sleep 0.005
    count=$((count + 1))
  done
done
