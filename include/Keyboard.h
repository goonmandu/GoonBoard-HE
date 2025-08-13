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

#ifdef __cplusplus
extern "C" {
#endif

/** \file
 *
 *  Header file for Keyboard.c.
 */

#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

    /* Includes: */
        #include <avr/io.h>
        #include <avr/wdt.h>
        #include <avr/power.h>
        #include <avr/interrupt.h>
        #include <stdbool.h>
        #include <string.h>

        #include "Descriptors.h"

        #include "../LUFA/Drivers/USB/USB.h"
        #include "../LUFA/Platform/Platform.h"

    /* Macros: */
        /** LED mask for the library LED driver, to indicate that the USB interface is not ready. */
        // #define LEDMASK_USB_NOTREADY      LEDS_LED1

        /** LED mask for the library LED driver, to indicate that the USB interface is enumerating. */
        #define LEDMASK_USB_ENUMERATING  (LEDS_LED2 | LEDS_LED3)

        /** LED mask for the library LED driver, to indicate that the USB interface is ready. */
        #define LEDMASK_USB_READY        (LEDS_LED2 | LEDS_LED4)

        /** LED mask for the library LED driver, to indicate that an error has occurred in the USB interface. */
        #define LEDMASK_USB_ERROR        (LEDS_LED1 | LEDS_LED3)

        #define PREV_REPORT_BUFFER_SIZE     (255)

        /** Rotary Encoder */
        #define ROTARY_SIGPB                PC6
        #define ROTARY_PB_PRESSED           (!(PINC & (1 << ROTARY_SIGPB)))  // Active low
        #define ROTARY_SIGCLK               PD4  // "Signal A"
        #define ROTARY_SIGDT                PD7  // "Signal B"
        #define ROTARY_MASK                 ((uint8_t)((1 << ROTARY_SIGCLK) | (1 << ROTARY_SIGDT)))

        // Gray-coded quadrature rotary encoder
        #define ROTARY_STATE_A              ((uint8_t)((0 << ROTARY_SIGCLK) | (0 << ROTARY_SIGDT)))
        #define ROTARY_STATE_B              ((uint8_t)((1 << ROTARY_SIGCLK) | (0 << ROTARY_SIGDT)))
        #define ROTARY_STATE_C              ((uint8_t)((1 << ROTARY_SIGCLK) | (1 << ROTARY_SIGDT)))
        #define ROTARY_STATE_D              ((uint8_t)((0 << ROTARY_SIGCLK) | (1 << ROTARY_SIGDT)))

        #define ROTARY_DIR_INVALID          0
        #define ROTARY_DIR_COUNTERCLOCKWISE 1
        #define ROTARY_DIR_CLOCKWISE        2

    /* Function Prototypes: */
        void SetupHardware(void);


        void EVENT_USB_Device_Connect(void);
        void EVENT_USB_Device_Disconnect(void);
        void EVENT_USB_Device_ConfigurationChanged(void);
        void EVENT_USB_Device_ControlRequest(void);
        void EVENT_USB_Device_StartOfFrame(void);

        bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                                 uint8_t* const ReportID,
                                                 const uint8_t ReportType,
                                                 void* ReportData,
                                                 uint16_t* const ReportSize);
        void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                                  const uint8_t ReportID,
                                                  const uint8_t ReportType,
                                                  const void* ReportData,
                                                  const uint16_t ReportSize);
        
        uint8_t determine_rotary_keycode();
    
    /* Variable declarations */
        extern uint8_t rotary_prev;
        extern uint8_t rotary_now;
#endif

#ifdef __cplusplus
}
#endif