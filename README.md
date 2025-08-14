# GoonBoard-HE
## Overview
This repository contains the firmware source code for building a USB HID keyboard with the Pro Micro development board using hall-effect switches.

The web configurator companion is [GoonBoard-Configurator](https://github.com/goonmandu/GoonBoard-Configurator).

## Features
- 1000 Hz polling rate
- Rapid trigger
- SnapTap (two modules)
- Full on-the-fly reconfigurability for:
  - Keymaps
  - Actuation points per key
  - Rapid trigger enable/sensitivity
  - SnapTap enable/keys
- NKRO Support

## Parts used
| Type            | Manufacturer         | Part Number       | Unit Price (USD) |
|-----------------|----------------------|-------------------|------------------|
| ADC             | TI                   | ADS7885           | 3.67             |
| Microcontroller | Microchip            | ATmega32U4        | 5.29             |
| Multiplexer     | TI                   | CD74HC4067M96     | 0.65             |
| HE Sensor       | Nanjing Ouzhuo Tech  | OH49E-S           | 0.18             |
| HE Switch       | Gateron              | KS-20T (90 pcs)   | 57.60            |
| Rotary Encoder  | Alps                 | EC11              | 1.99             |

## Compiling and Uploading
Since I have used the PlatformIO toolchain for the entirety of the development process, the steps using PlatformIO will be shared.

1. Install Visual Studio Code.
2. Install the PlatformIO extension.
3. Open the `GoonBoard-HE` workspace.
4. Click on the `PlatformIO: Build` button.
5. Click on the `PlatformIO: Upload` button.
6. Put your microcontroller into DFU mode by shorting `RST` and `GND`.
7. Wait a few seconds and your microcontroller will be recognized as `GoonMandu GoonBoard` with VID:PID `2B00:B1E5`.

## Performance
Tested with ATmega32U4 @ 16 MHz, target polling rate 1000 Hz, both SnapTap modules enabled.

Reference release: [V3.250814.0](https://github.com/goonmandu/GoonBoard-HE/releases/tag/V3.250814.0)

| Metric | RT On | RT Off |
|-|-|-|
| Start polling to sending HID report | 760 us | 730 us |
| 1000 Hz polling rate compliance | ✅ | ✅ |

## Pin Assignments
| AVR | Pro Micro | Function | Notes |
|-|-|-|-|
| PD0 | TXO | Row Mux S0 |
| PD1 | RXI | Row Mux S1 |
| PD2 | 2 | Row Mux S2 |
| PD3 | 3 | Row Mux S3 |
| PF4 | A3 | Key Mux S0 |
| PF5 | A2 | Key Mux S1 |
| PF6 | A1 | Key Mux S2 |
| PF7 | A0 | Key Mux S3 |
| PD4 | 4 | Knob "CLK" | aka signal "A", pulled high |
| PD7 | 6 | Knob "DT" | aka signal "B", pulled high |
| PC6 | 5 | Knob Button | pulled high |
| PE6 | 7 | Caps Lock LED |
| PB4 | 8 | Num Lock LED |
| PB5 | 9 | Scr Lock LED | used as debug bit if<br>`DEBUG_BITS` is defined. |
| PB6 | 10 | SPI /SS |
| PB3 | 14 | SPI MISO |
| PB1 | 15 | SPI SCLK |

## Versioning
Format:
```
V{compat}.{YY}{MM}{DD}.{n}
```
- `compat`: Integer. This changing means that the hardware/timing compatibility or pin assignments have changed and is not physically compatible with other `compat` versions.
- `YY`: Two-digit year.
- `MM`: Zero-padded month.
- `DD`: Zero-padded day.
- `n`: Integer. Release number in the same day.

## Maintenance? PRs? Issues?
I will not be paying much attention to this repository once I have implemented the features I want so they will probably be ignored. If you want to build upon this repository and want clarification, *please* feel free to annoy me at:

Discord: `goonmandu`<br>
Email: `hwang [dot] mingi7 [at] gmail [dot] com`

## Acknowledgements and Open-Source Licenses
All files under `src/` that do not contain a license header are licensed under the MIT License, Copyright (c) 2025 Mingi Hwang.

This firmware is built upon the Keyboard demo from the LUFA repository:
- [LUFA](https://github.com/abcminiuser/lufa):
    ```
    LUFA Library
    Copyright (C) Dean Camera, 2021.

    dean [at] fourwalledcubicle [dot] com
    www.lufa-lib.org

    Permission to use, copy, modify, and distribute this software and its documentation for any purpose is hereby granted without fee, provided that the above copyright notice appear in all copies and that both that the copyright notice and this permission notice and warranty disclaimer appear in supporting documentation, and that the name of the author not be used in advertising or publicity pertaining to distribution of the software without specific, written prior permission.

    The author disclaims all warranties with regard to this software, including all implied warranties of merchantability and fitness.  In no event shall the author be liable for any special, indirect or consequential damages or any damages whatsoever resulting from loss of use, data or profits, whether in an action of contract, negligence or other tortious action, arising out of or in connection with the use or performance of this software.
    ```