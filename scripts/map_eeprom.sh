#!/usr/bin/env bash
# map_intelhex_eeprom.sh
#
# Usage:
#   ./map_intelhex_eeprom.sh Keycodes.h firmware.eep.hex
#
# Parses KC_… definitions from Keycodes.h, then reads each data record
# from the Intel‑HEX EEPROM file and prints offset, hex value, and keycode.

set -euo pipefail

if [ "$#" -ne 2 ]; then
  echo "Usage: $0 <keycodes_header.h> <eeprom_intelhex_file>"
  exit 1
fi

KEYCODES_HEADER="$1"
EEP_ROM_HEX="$2"

# 1) Build map: byte value -> keycode name
declare -A code_map
while read -r _ name value; do
  # strip “0x”, uppercase, pad two digits
  hex="${value#0x}"
  hex="${hex^^}"
  printf -v hex "%02s" "$hex"
  code_map["$hex"]="$name"
done < <(grep -E '^#define[[:space:]]+KC_[A-Za-z0-9_]+\s+0x[0-9A-Fa-f]+' "$KEYCODES_HEADER")

# 2) Parse Intel‑HEX and map each data byte
offset=0
while IFS= read -r line; do
  # skip non-data records
  [[ "${line:0:1}" != ":" ]] && continue
  bytecount=$((16#${line:1:2}))
  rectype=$((16#${line:7:2}))
  # data record?
  if [ "$rectype" -eq 0 ]; then
    data="${line:9:bytecount*2}"
    for ((i=0; i<bytecount; i++)); do
      bytehex="${data:i*2:2}"
      name="${code_map[$bytehex]:-UNKNOWN}"
      printf "%04d: 0x%s %s\n" "$offset" "$bytehex" "$name"
      ((offset++))
    done
  fi
done < "$EEP_ROM_HEX"
