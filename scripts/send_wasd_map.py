import os
import stat
import time
import sys

def wait_for_device(path: str, timeout: float = 0.0) -> None:
    """Wait until the character device at path appears."""
    start = time.time()
    while True:
        try:
            st = os.stat(path)
            if stat.S_ISCHR(st.st_mode):
                return
        except FileNotFoundError:
            pass
        if timeout and (time.time() - start) >= timeout:
            print(f"Timed out waiting for {path}", file=sys.stderr)
            sys.exit(1)
        time.sleep(0.1)

def open_device(path: str) -> int:
    """Open the HID device for read/write and return its file descriptor."""
    return os.open(path, os.O_RDWR)

def send_reports(fd: int) -> None:
    """Send a sequence of HID reports for rows 0-5 and cols 0-15."""
    mapping = {
        (0, 1): 0x1A,   # W
        (0, 3): 0x2C,   # Space
        (2, 0): 0x04,   # A
        (2, 1): 0x16,   # S
        (2, 2): 0x07,   # D
        (1, 3): 0x15,   # R
    }
    total = 6 * 16
    count = 1

    for row in range(6):
        for col in range(16):
            code = mapping.get((row, col), 0x00)
            print(f"Sending {count} / {total}")
            report = bytes((0xA0, row, col, code))
            os.write(fd, report)
            time.sleep(0.01)
            count += 1

def main():
    device_path = "/dev/hidraw5"
    wait_for_device(device_path)
    fd = open_device(device_path)

    try:
        send_reports(fd)
    finally:
        os.close(fd)

if __name__ == "__main__":
    main()