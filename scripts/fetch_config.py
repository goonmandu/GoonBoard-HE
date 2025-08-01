import hid
import sys
from keycodes import keycodes

# Constants matching the device configuration
VID = 0x2B00       # your keyboard’s Vendor ID
PID = 0xB1E5       # your keyboard’s Product ID
REPORT_ID = 3      # Full config Feature report ID
NUM_ROWS = 6
NUM_COLS = 16
KEYMAP_SIZE = NUM_ROWS * NUM_COLS
ACTUATION_SIZE = KEYMAP_SIZE
THRESHOLD_SIZE = 2
SNAPTAP_SIZE = 6
FULL_CONFIG_SIZE = KEYMAP_SIZE + ACTUATION_SIZE + THRESHOLD_SIZE + SNAPTAP_SIZE


def main():
    try:
        dev = hid.device()
        dev.open(VID, PID)
    except Exception as e:
        print(f"Error opening HID device {VID:04X}:{PID:04X}: {e}", file=sys.stderr)
        sys.exit(1)

    try:
        # Request FULL_CONFIG_SIZE+1 bytes (first byte is ReportID)
        print("Reading EEPROM...")
        buf = dev.get_feature_report(REPORT_ID, FULL_CONFIG_SIZE + 1)
    except Exception as e:
        print(f"Error sending GetFeatureReport: {e}", file=sys.stderr)
        dev.close()
        sys.exit(1)
    finally:
        dev.close()

    if not buf or buf[0] != REPORT_ID:
        print(f"Unexpected report ID: {buf[0] if buf else None}", file=sys.stderr)
        sys.exit(1)

    payload = buf[1:]
    if len(payload) < FULL_CONFIG_SIZE:
        print(f"Incomplete payload: got {len(payload)} bytes, expected {FULL_CONFIG_SIZE}", file=sys.stderr)
        sys.exit(1)

    # Split the payload
    keymap     = payload[0:KEYMAP_SIZE]
    actuations = payload[KEYMAP_SIZE:KEYMAP_SIZE+ACTUATION_SIZE]
    thresholds = payload[KEYMAP_SIZE+ACTUATION_SIZE:KEYMAP_SIZE+ACTUATION_SIZE+THRESHOLD_SIZE]
    snaptap    = payload[-SNAPTAP_SIZE:]

    # Print keymap matrix
    print("Keymap:")
    for r in range(NUM_ROWS):
        row = keymap[r*NUM_COLS:(r+1)*NUM_COLS]
        print(f" Row {r}: ", '  '.join(f"{keycodes[b].upper().ljust(8)}" for b in row))

    # Print actuation matrix
    print("Actuations:")
    for r in range(NUM_ROWS):
        row = actuations[r*NUM_COLS:(r+1)*NUM_COLS]
        print(f" Row {r}: ", '  '.join(f"{b/10}mm".ljust(8) for b in row))

    # Print thresholds
    rt, rt_sc = thresholds
    print(f"RT threshold: {rt/10}mm")
    print(f"RT SC threshold: {rt_sc/10}mm")

    # Print SnapTap settings
    stas, sta1, sta2, stbs, stb1, stb2 = snaptap
    print(f"SnapTap Module 1 {"ON, " if stas else "OFF,"} Keys 0x{sta1:02X}, 0x{sta2:02X}")
    print(f"SnapTap Module 2 {"ON, " if stbs else "OFF,"} Keys 0x{stb1:02X}, 0x{stb2:02X}")


if __name__ == "__main__":
    main()
