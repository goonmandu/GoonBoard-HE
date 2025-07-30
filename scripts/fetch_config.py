import hid
import sys

def main():
    VID        = 0x2B00      # your keyboard’s Vendor ID
    PID        = 0xB1E5      # your keyboard’s Product ID
    REPORT_ID  = 3
    REPORT_LEN = 194         # FULL_CONFIG_REPORT_SIZE

    try:
        dev = hid.device()
        # open by VID/PID instead of a raw path
        dev.open(VID, PID)
    except Exception as e:
        print(f"Error opening HID device {VID:04X}:{PID:04X}: {e}", file=sys.stderr)
        sys.exit(1)

    try:
        # Request REPORT_LEN+1 bytes (first byte is ReportID)
        dev.get_feature_report(REPORT_ID, REPORT_LEN + 1)
    except Exception as e:
        print(f"Error sending GetFeatureReport: {e}", file=sys.stderr)
        sys.exit(1)
    finally:
        dev.close()

if __name__ == "__main__":
    main()
