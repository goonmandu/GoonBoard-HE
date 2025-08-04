import os
import sys

def write_hid_report(device_path: str, report: bytes) -> None:
    """Open the HID device and write the raw report."""
    # Open in read/write mode to satisfy hidraw requirements
    fd = os.open(device_path, os.O_RDWR)
    try:
        os.write(fd, report)
    finally:
        os.close(fd)

def main():
    # Update this path if your hidraw device is different
    device = "/dev/hidraw5"
    # Four-byte report including NULs
    report = bytes([0xA0, 0x00, 0x0A, 0x00])
    write_hid_report(device, report)

if __name__ == "__main__":
    main()