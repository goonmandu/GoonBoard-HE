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
uint8_t PrevKeyboardHIDReportBuffer[PREV_REPORT_BUFFER_SIZE];

/* START CUSTOMIZATION CODE */

// Variables to detect keypresses
extern uint8_t key_status[NUM_ROWS][MAX_KEYS_SUPPORTED_PER_ROW];
uint8_t rotary_now = ROTARY_STATE_A;
uint8_t rotary_prev = ROTARY_STATE_A;
int8_t rotary_counts = 0;

/* END CUSTOMIZATION CODE */

/** LUFA HID Class driver interface configuration and state information. This structure is
 *  passed to all HID Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_HID_Device_t Keyboard_HID_Interface =
    {
        .Config =
            {
                .InterfaceNumber                = INTERFACE_ID_Keyboard,
                .ReportINEndpoint               =
                    {
                        .Address                = KEYBOARD_EPADDR,
                        .Size                   = KEYBOARD_EPSIZE,
                        .Banks                  = 1,
                    },
                .PrevReportINBuffer             = PrevKeyboardHIDReportBuffer,
                .PrevReportINBufferSize         = sizeof(PrevKeyboardHIDReportBuffer),
            },
    };

USB_ClassInfo_HID_Device_t RawHID_HID_Interface = 
    {
        .Config =
            {
                .InterfaceNumber                = INTERFACE_ID_RawHID,
                .ReportINEndpoint               =
                    {
                        .Address                = RAWHID_IN_EPADDR,
                        .Size                   = RAWHID_IN_EPSIZE,
                        .Banks                  = 1,
                    },
                .PrevReportINBuffer             = NULL,  // monkaS bare metal nullptr dereference
                .PrevReportINBufferSize         = sizeof(default_settings),
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
    ConfigSuccess &= HID_Device_ConfigureEndpoints(&RawHID_HID_Interface);

    USB_Device_EnableSOFEvents();

    // LEDs_SetAllLEDs(ConfigSuccess ? LEDMASK_USB_READY : LEDMASK_USB_ERROR);
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
    HID_Device_ProcessControlRequest(&Keyboard_HID_Interface);
    HID_Device_ProcessControlRequest(&RawHID_HID_Interface);
}

/** Event handler for the USB device Start Of Frame event. */
void EVENT_USB_Device_StartOfFrame(void)
{
    HID_Device_MillisecondElapsed(&Keyboard_HID_Interface);
    HID_Device_MillisecondElapsed(&RawHID_HID_Interface);
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
// This function is called in TWO places:
//     `HID_Device_ProcessControlRequest` (which handles host USB requests)
//     `HID_Device_USBTask` (which handles the keyboard's own HID generation)
//
// When called by the Host USB Request handler function, `*ReportID` is nonzero
// and this function should detect the ReportID then generate a suitable report
// for that ReportID.
//
// When called by the periodic HID generation function, `*ReportID` is zero and
// this function should modify `*ReportID` to the ReportID of the HID keyboard
// report (in this codebase, `HID_KEYBOARD_REPORT_ID`), then send the HID report.
//
// I am terribly sorry for the cruel and unusual levels of nesting.
bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                         uint8_t* const ReportID,
                                         const uint8_t ReportType,
                                         void* ReportData,
                                         uint16_t* const ReportSize)
{
    switch (HIDInterfaceInfo->Config.InterfaceNumber) {
        case INTERFACE_ID_Keyboard: {
            /** DO NOT MODIFY!
             *  The HID Keyboard 32KRO interface is merely an extension of the boot protocol
             *  to support 32 simultaneous keys. Everything else is compliant to the boot protocol.
             *  If in Report Protocol mode, everything works as expected and the first 32 keys are registered.
             *  If in Boot Protocol mode, only the first six keys pressed will be recognized.
             */
            switch (*ReportID) {
                // This case is only taken with the periodic keyboard status polls
                // Process normal keyboard report
                case INTERNAL_GENERATE_PERIODIC_HID_REPORT_ID: {
                    // Differentiate it between "Lock LED" OUT Reports
                    if (ReportType == HID_REPORT_ITEM_In) {
                        // Set outbound ReportID to the one specified in the descriptor
                        // This does nothing right now because we're not using Report IDs for keyboards
                        // which makes it default to the Report ID 0x00
                        // *ReportID = HID_KEYBOARD_REPORT_ID;

                        USB_KeyboardReport_Data_t* Rpt = (void*)ReportData;
                        static uint8_t rpt_idx = 0;

                        // Scans keys and fills `key_status` based on whether RT is enabled
                        scan_keys();

                        // Filter SnapTap
                        static uint8_t* snaptap_a_restore_ptr;
                        static uint8_t* snaptap_b_restore_ptr;
                        static uint8_t snaptap_do_restore;
                        if (SNAPTAP_A_STATUS == SNAPTAP_ENABLED)
                            snaptap_do_restore |= snaptap(SNAPTAP_MODULE_A, &snaptap_a_restore_ptr) << 0;
                        if (SNAPTAP_B_STATUS == SNAPTAP_ENABLED)
                            snaptap_do_restore |= snaptap(SNAPTAP_MODULE_B, &snaptap_b_restore_ptr) << 1;

                        // "key_idx < MAX_KEYS_SUPPORTED_PER_ROW;" is left as-is because of timing requirements
                        // If NUM_KEYS_PER_ROW[row_idx] is used, it violates 1000 Hz polling rate even at -Ofast
                        for (uint8_t row_idx = 0; row_idx < NUM_ROWS; ++row_idx) {
                            for (uint8_t key_idx = 0; key_idx < MAX_KEYS_SUPPORTED_PER_ROW && rpt_idx < MAX_NKRO; ++key_idx) {
                                if (key_status[row_idx][key_idx]) Rpt->KeyCode[rpt_idx++] = KEYMAP_MATRIX[row_idx][key_idx];
                            }
                        }

                        // Restore keys overridden to SnapTap for Rapid Trigger compatibility
                        if (SNAPTAP_A_STATUS == SNAPTAP_ENABLED && (snaptap_do_restore & (1 << 0)))
                            *snaptap_a_restore_ptr = PRESSED;
                        if (SNAPTAP_B_STATUS == SNAPTAP_ENABLED && (snaptap_do_restore & (1 << 1)))
                            *snaptap_b_restore_ptr = PRESSED;
                        
                        
                        // Rotary encoder section
                        static uint8_t rotary_key;
                        rotary_prev = rotary_now;
                        rotary_now = PIND & ROTARY_MASK;
                        if (rotary_prev != rotary_now) {
                            rotary_key = determine_rotary_keycode();
                            if (rotary_key != ROTARY_DIR_INVALID && rpt_idx < MAX_NKRO) {
                                Rpt->KeyCode[rpt_idx++] = rotary_key;
                            }
                        }
                        if (ROTARY_PB_PRESSED && rpt_idx < MAX_NKRO) {
                            Rpt->KeyCode[rpt_idx++] = ROTARY_PUSHBUTTON_KEYMAP;
                        }

                        *ReportSize = sizeof(*Rpt);
                        rpt_idx = 0;
                        return false;
                    }
                    return false;
                }
        
                default:
                    return false;
            }
            return false;
        }

        case INTERFACE_ID_RawHID: {
            switch (*ReportID) {
                /**
                 * Below this line contains everything that is NOT the periodically generated keyboard
                 * report and are coming from the host requesting information or interacting with
                 * the device.
                 */
                case FETCH_CONFIG_REPORT_ID: {
                    if (ReportType == HID_REPORT_ITEM_Feature) {
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

                        // Copy rotary encoder
                        buf[offset++] = eeprom_read_byte((const uint8_t*)&default_settings.rotary_counterclockwise);
                        buf[offset++] = eeprom_read_byte((const uint8_t*)&default_settings.rotary_clockwise);
                        buf[offset++] = eeprom_read_byte((const uint8_t*)&default_settings.rotary_pushbutton);

                        // Copy thresholds
                        buf[offset++] = eeprom_read_byte((const uint8_t*)&default_settings.rt_status);
                        buf[offset++] = eeprom_read_byte((const uint8_t*)&default_settings.rt_threshold);
                        buf[offset++] = eeprom_read_byte((const uint8_t*)&default_settings.rt_sc_threshold);

                        // Copy SnapTap settings
                        buf[offset++] = eeprom_read_byte((const uint8_t*)&default_settings.snaptap_a_status);
                        buf[offset++] = eeprom_read_byte((const uint8_t*)&default_settings.snaptap_a_key1);
                        buf[offset++] = eeprom_read_byte((const uint8_t*)&default_settings.snaptap_a_key2);
                        buf[offset++] = eeprom_read_byte((const uint8_t*)&default_settings.snaptap_b_status);
                        buf[offset++] = eeprom_read_byte((const uint8_t*)&default_settings.snaptap_b_key1);
                        buf[offset++] = eeprom_read_byte((const uint8_t*)&default_settings.snaptap_b_key2);

                        *ReportSize = offset;  // should be 201
                        return true;
                    }
                    return false;
                }

                default:
                    return false;
            }
        }

        default:
            return false;
    }
}


/** HID class driver callback function for the processing of HID reports from the host.
 *
 *  \param[in] HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in] ReportID    Report ID of the received report from the host
 *  \param[in] ReportType  The type of report that the host has sent, either HID_REPORT_ITEM_Out or HID_REPORT_ITEM_Feature
 *  \param[in] ReportData  Pointer to a buffer where the received report has been stored
 *  \param[in] ReportSize  Size in bytes of the received HID report
 */

// I am terribly sorry for the cruel and unusual levels of nesting.
void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                          const uint8_t ReportID,
                                          const uint8_t ReportType,
                                          const void* ReportData,
                                          const uint16_t ReportSize)
{
    uint8_t* params;
    switch (HIDInterfaceInfo->Config.InterfaceNumber) {
        case INTERFACE_ID_Keyboard: {
            /** DO NOT MODIFY!
             *  The HID Keyboard 32KRO interface is merely an extension of the boot protocol
             *  to support 32 simultaneous keys. Everything else is compliant to the boot protocol.
             *  If in Report Protocol mode, everything works as expected and the first 32 keys are registered.
             *  If in Boot Protocol mode, only the first six keys pressed will be recognized.
             */
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

                default:
                    break;
            }
            break;
        }

        // Custom commands
        case INTERFACE_ID_RawHID: {
            switch (ReportID) {
                case CUSTOM_COMMAND_REPORT_ID: {
                    uint8_t command_id = *(uint8_t*)ReportData;
                    switch (command_id) {
                        // Keymap Edit Report, size = 4
                        // Params                   0           1           2
                        // Byte         0           1           2           3
                        // Meaning      CommandID   Row         Key         KeyCode
                        case EDIT_KEYMAP_COMMAND_ID: {
                            // Check for malformed keymap edit reports
                            if (ReportSize != EDIT_KEYMAP_COMMAND_BYTES + 1) break;
                            params = (uint8_t*)(ReportData + 1);
                            save_keymap_to_eeprom(params[0], params[1], params[2]);
                            KEYMAP_MATRIX[params[0]][params[1]] = params[2];
                            break;
                        }
                        
                        // Actuations Edit Report, size = 4
                        // Params                   0           1           2
                        // Byte         0           1           2           3
                        // Meaning      CommandID   Row         Key         mm
                        case EDIT_ACTUATIONS_COMMAND_ID: {
                            // Check for malformed actuation edit reports
                            if (ReportSize != EDIT_ACTUATIONS_COMMAND_BYTES + 1) break;
                            params = (uint8_t*)(ReportData + 1);
                            save_actuation_to_eeprom(params[0], params[1], params[2]);
                            ACTUATIONS_MATRIX[params[0]][params[1]] = params[2];
                            break;
                        }

                        // Actuations Edit Report, size = 4
                        // Params                   0           1           2
                        // Byte         0           1           2           3
                        // Meaning      CommandID   CtClkw      Clkw        Button
                        case EDIT_ROTARY_KEYMAP_COMMAND_ID: {
                            // Check for malformed actuation edit reports
                            if (ReportSize != EDIT_ACTUATIONS_COMMAND_BYTES + 1) break;
                            params = (uint8_t*)(ReportData + 1);
                            save_rotary_encoder_to_eeprom(params[0], params[1], params[2]);
                            ROTARY_COUNTERCLOCKWISE_KEYMAP = params[0];
                            ROTARY_CLOCKWISE_KEYMAP = params[1];
                            ROTARY_PUSHBUTTON_KEYMAP = params[2];
                            break;
                        }

                        // SnapTap Modules Edit Report, size = 4
                        // Params                   0           1           2
                        // Byte         0           1           2           3
                        // Meaning      CommandID   ST Status   ST Key1     ST Key2
                        case EDIT_SNAPTAP_A_COMMAND_ID: {
                            // Check for malformed actuation edit reports
                            if (ReportSize != EDIT_SNAPTAP_COMMAND_BYTES + 1) break;
                            params = (uint8_t*)(ReportData + 1);
                            save_snaptap_a_to_eeprom(params[0], params[1], params[2]);
                            SNAPTAP_A_STATUS = params[0];
                            SNAPTAP_A_KEY1_COORDS = params[1];
                            SNAPTAP_A_KEY2_COORDS = params[2];
                            break;
                        }
                        case EDIT_SNAPTAP_B_COMMAND_ID: {
                            // Check for malformed snaptap edit reports
                            if (ReportSize != EDIT_SNAPTAP_COMMAND_BYTES + 1) break;
                            params = (uint8_t*)(ReportData + 1);
                            save_snaptap_b_to_eeprom(params[0], params[1], params[2]);
                            SNAPTAP_B_STATUS = params[0];
                            SNAPTAP_B_KEY1_COORDS = params[1];
                            SNAPTAP_B_KEY2_COORDS = params[2];
                            break;
                        }
                        
                        // Rapid Trigger Edit Report, size = 4
                        case EDIT_RAPID_TRIGGER_COMMAND_ID: {
                            // Check for malformed rapid trigger edit reports
                            if (ReportSize != EDIT_RAPID_TRIGGER_COMMAND_BYTES + 1) break;
                            params = (uint8_t*)(ReportData + 1);
                            save_rapid_trigger_to_eeprom(params[0], params[1], params[2]);
                            RAPID_TRIGGER_STATUS = params[0];
                            RAPID_TRIGGER_THRESHOLD = params[1];
                            RAPID_TRIGGER_SHORT_CIRCUIT_THRESHOLD = params[2];
                            break;
                        }

                        default:
                            break;
            }
        }

                default:
                    break;
            }
            break;
        }
        
        default:
            break;
    }
}

// Determines rotary encoder direction and returns the appropriate keycode.
uint8_t determine_rotary_keycode() {
    switch (rotary_prev) {
        case (ROTARY_STATE_A):
            if (rotary_now == ROTARY_STATE_B) {
                // Clockwise
                if (++rotary_counts == 4) {
                    rotary_counts = 0;
                    return ROTARY_CLOCKWISE_KEYMAP;
                }
            }
            if (rotary_now == ROTARY_STATE_D) {
                // Counterclockwise
                if (--rotary_counts == -4) {
                    rotary_counts = 0;
                    return ROTARY_COUNTERCLOCKWISE_KEYMAP;
                }
            }
            return ROTARY_DIR_INVALID;

        case (ROTARY_STATE_B):
            if (rotary_now == ROTARY_STATE_C) {
                // Clockwise
                if (++rotary_counts == 4) {
                    rotary_counts = 0;
                    return ROTARY_CLOCKWISE_KEYMAP;
                }
            }
            if (rotary_now == ROTARY_STATE_A) {
                // Counterclockwise
                if (--rotary_counts == -4) {
                    rotary_counts = 0;
                    return ROTARY_COUNTERCLOCKWISE_KEYMAP;
                }
            }
            return ROTARY_DIR_INVALID;

        case (ROTARY_STATE_C):
            if (rotary_now == ROTARY_STATE_D) {
                // Clockwise
                if (++rotary_counts == 4) {
                    rotary_counts = 0;
                    return ROTARY_CLOCKWISE_KEYMAP;
                }
            }
            if (rotary_now == ROTARY_STATE_B) {
                // Counterclockwise
                if (--rotary_counts == -4) {
                    rotary_counts = 0;
                    return ROTARY_COUNTERCLOCKWISE_KEYMAP;
                }
            }
            return ROTARY_DIR_INVALID;
        
        // case (ROTARY_STATE_D):
        default:
            if (rotary_now == ROTARY_STATE_A) {
                // Clockwise
                if (++rotary_counts == 4) {
                    rotary_counts = 0;
                    return ROTARY_CLOCKWISE_KEYMAP;
                }
            }
            if (rotary_now == ROTARY_STATE_C) {
                // Counterclockwise
                if (--rotary_counts == -4) {
                    rotary_counts = 0;
                    return ROTARY_COUNTERCLOCKWISE_KEYMAP;
                }
            }
            return ROTARY_DIR_INVALID;
    }
}