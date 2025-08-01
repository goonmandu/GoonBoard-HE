/*
             LUFA Library
     Copyright (C) Dean Camera, 2021.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2021  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *
 *  This file contains the main tasks of the keyboard and is responsible for the 
 *  initial application hardware configuration.
 */


#include "Keyboard.h"
#include "SPI.h"
#include "I2C.h"
#include "MatrixScanner.h"
#include "SettingsEditor.h"
#include "DefaultSettings.h"
#include "Hacks.h"
#include "SnapTap.h"

/** Buffer to hold the previously generated Keyboard HID report, for comparison purposes inside the HID class driver. */
uint8_t PrevKeyboardHIDReportBuffer[1 + FETCH_CONFIG_REPORT_SIZE];

/* START CUSTOMIZATION CODE */

// Variables to detect keypresses
extern uint8_t key_status[NUM_ROWS][MAX_KEYS_SUPPORTED_PER_ROW];
static volatile uint8_t rpt_idx = 0;

/* END CUSTOMIZATION CODE */

/** LUFA HID Class driver interface configuration and state information. This structure is
 *  passed to all HID Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_HID_Device_t Keyboard_HID_Interface =
    {
        .Config =
            {
                .InterfaceNumber              = INTERFACE_ID_Keyboard,
                .ReportINEndpoint             =
                    {
                        .Address              = KEYBOARD_EPADDR,
                        .Size                 = KEYBOARD_EPSIZE,
                        .Banks                = 1,
                    },
                .PrevReportINBuffer           = PrevKeyboardHIDReportBuffer,
                .PrevReportINBufferSize       = sizeof(PrevKeyboardHIDReportBuffer),
            },
    };

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware()
{
#if (ARCH == ARCH_AVR8)
    /* Disable watchdog if enabled by bootloader/fuses */
    MCUSR &= ~(1 << WDRF);
    wdt_disable();

    /* Disable clock division */
    clock_prescale_set(clock_div_1);
#elif (ARCH == ARCH_XMEGA)
    /* Start the PLL to multiply the 2MHz RC oscillator to 32MHz and switch the CPU core to run from it */
    XMEGACLK_StartPLL(CLOCK_SRC_INT_RC2MHZ, 2000000, F_CPU);
    XMEGACLK_SetCPUClockSource(CLOCK_SRC_PLL);

    /* Start the 32MHz internal RC oscillator and start the DFLL to increase it to 48MHz using the USB SOF as a reference */
    XMEGACLK_StartInternalOscillator(CLOCK_SRC_INT_RC32MHZ);
    XMEGACLK_StartDFLL(CLOCK_SRC_INT_RC32MHZ, DFLL_REF_INT_USBSOF, F_USB);

    PMIC.CTRL = PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
#endif

    /* Hardware Initialization */
    // Joystick_Init();
    // LEDs_Init();
    // Buttons_Init();
    USB_Init();
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
    // LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
    // LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
    bool ConfigSuccess = true;

    ConfigSuccess &= HID_Device_ConfigureEndpoints(&Keyboard_HID_Interface);

    USB_Device_EnableSOFEvents();

    // LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
    HID_Device_ProcessControlRequest(&Keyboard_HID_Interface);
}

/** Event handler for the USB device Start Of Frame event. */
void EVENT_USB_Device_StartOfFrame(void)
{
    HID_Device_MillisecondElapsed(&Keyboard_HID_Interface);
}

/** HID class driver callback function for the creation of HID reports to the host.
 *
 *  \param[in]     HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in,out] ReportID    Report ID requested by the host if non-zero, otherwise callback should set to the generated report ID
 *  \param[in]     ReportType  Type of the report to create, either HID_REPORT_ITEM_In or HID_REPORT_ITEM_Feature
 *  \param[out]    ReportData  Pointer to a buffer where the created report should be stored
 *  \param[out]    ReportSize  Number of bytes written in the report (or zero if no report is to be sent)
 *
 *  \return Boolean \c true to force the sending of the report, \c false to let the library determine if it needs to be sent
 */
bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                         uint8_t* const ReportID,
                                         const uint8_t ReportType,
                                         void* ReportData,
                                         uint16_t* const ReportSize)
{
    // If fetch config request received, do that
    if (ReportType == HID_REPORT_ITEM_Feature && *ReportID == FETCH_CONFIG_REPORT_ID) {
        uint8_t* buf    = (uint8_t*)ReportData;
        uint16_t offset = 0;

        // Copy keymap
        eeprom_read_block(buf + offset,
                          (const void*)&default_settings.keymap[0][0],
                          sizeof default_settings.keymap);
        offset += sizeof default_settings.keymap;  // +96

        // Copy actuations
        eeprom_read_block(buf + offset,
                          (const void*)&default_settings.actuations[0][0],
                          sizeof default_settings.actuations);
        offset += sizeof default_settings.actuations;  // +96 (now offset=192)

        // Copy thresholds
        buf[offset++] = eeprom_read_byte((const uint8_t*)&default_settings.rt_threshold);
        buf[offset++] = eeprom_read_byte((const uint8_t*)&default_settings.rt_sc_threshold);

        *ReportSize = offset;  // should be 194
        return true;
    }

    // Else, process normal keyboard report
    USB_KeyboardReport_Data_t* Rpt = (void*)ReportData;
    memset(Rpt, 0, sizeof(*Rpt));
    rpt_idx = 0;

    /* CORE KEY SCANNING LOGIC*/
    scan_keys();

    // Filter SnapTap
    static uint8_t* snaptap_restore_ptr;
    static uint8_t snaptap_do_restore;

    if (SNAPTAP_STATUS == SNAPTAP_ENABLED) {
        snaptap_do_restore = snaptap(&snaptap_restore_ptr);
    }

    // "key_idx < MAX_KEYS_SUPPORTED_PER_ROW;" is left as-is because of timing requirements
    // If NUM_KEYS_PER_ROW[row_idx] is used, it violates 1000 Hz polling rate even at -Ofast
    for (uint8_t row_idx = 0; row_idx < NUM_ROWS; ++row_idx) {
        for (uint8_t key_idx = 0; key_idx < MAX_KEYS_SUPPORTED_PER_ROW && rpt_idx < MAX_NKRO; ++key_idx) {
            if (key_status[row_idx][key_idx]) Rpt->KeyCode[rpt_idx++] = KEYMAP_MATRIX[row_idx][key_idx];
        }
    }

    // Restore keys overridden to SnapTap for Rapid Trigger compatibility
    if (SNAPTAP_STATUS == SNAPTAP_ENABLED && snaptap_do_restore)
        *snaptap_restore_ptr = PRESSED;

    *ReportSize = sizeof(*Rpt);
    
    return false;
}


/** HID class driver callback function for the processing of HID reports from the host.
 *
 *  \param[in] HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in] ReportID    Report ID of the received report from the host
 *  \param[in] ReportType  The type of report that the host has sent, either HID_REPORT_ITEM_Out or HID_REPORT_ITEM_Feature
 *  \param[in] ReportData  Pointer to a buffer where the received report has been stored
 *  \param[in] ReportSize  Size in bytes of the received HID report
 */
void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                          const uint8_t ReportID,
                                          const uint8_t ReportType,
                                          const void* ReportData,
                                          const uint16_t ReportSize)
{
    uint8_t* params;
    // TODO: Implement EEPROM update functions as Feature SetReport.
    switch (ReportID) {
        // LED Status Update Report, size = 2
        //
        // Byte         0           1
        // Meaning      ReportID    LED Bits
        case LOCK_LEDS_REPORT_ID: {
            uint8_t* LEDReport = (uint8_t*)ReportData;
            if (*LEDReport & HID_KEYBOARD_LED_NUMLOCK)
                numlock_on();
            else
                numlock_off();
            if (*LEDReport & HID_KEYBOARD_LED_CAPSLOCK)
                capslock_on();
            else
                capslock_off();
            if (*LEDReport & HID_KEYBOARD_LED_SCROLLLOCK)
                scrlock_on();
            else
                scrlock_off();
            break;
        }

        // Keymap Edit Report, size = 4
        //
        // Byte         0           1           2           3
        // Meaning      ReportID    Row         Key         KeyCode
        case EDIT_KEYMAP_REPORT_ID: {
            // Check for malformed keymap edit reports
            if (ReportSize != EDIT_KEYMAP_PARAMETERS_BYTES) break;
            params = (uint8_t*)ReportData;
            save_keymap_to_eeprom(params[0], params[1], params[2]);
            KEYMAP_MATRIX[params[0]][params[1]] = params[2];
            break;
        }
        
        // Actuation Point Edit Report, size = 4
        //
        // Byte         0           1           2           3
        // Meaning      ReportID    Row         Key         Actuation
        case EDIT_ACTUATIONS_REPORT_ID: {
            // Check for malformed actuation edit reports
            if (ReportSize != EDIT_ACTUATIONS_PARAMETERS_BYTES) break;
            params = (uint8_t*)ReportData;
            save_actuation_to_eeprom(params[0], params[1], params[2]);
            ACTUATIONS_MATRIX[params[0]][params[1]] = params[2];
            break;
        }

        // Unknown Report, skip
        default: {
            break;
        }
    }
}
