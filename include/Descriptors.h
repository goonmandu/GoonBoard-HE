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
 *  Header file for Descriptors.c.
 */

#ifndef _DESCRIPTORS_H_
#define _DESCRIPTORS_H_

    /* Includes: */
        #include <avr/pgmspace.h>
        #include "../lib/LUFA/Drivers/USB/USB.h"
        #include "KeyboardConfig.h"

    /* Type Defines: */
        /** Type define for the device configuration descriptor structure. This must be defined in the
         *  application code, as the configuration descriptor contains several sub-descriptors which
         *  vary between devices, and which describe the device's usage to the host.
         */
        typedef struct
        {
            USB_Descriptor_Configuration_Header_t Config;

            // Keyboard HID Interface
            USB_Descriptor_Interface_t              HID_Interface;
            USB_HID_Descriptor_HID_t                HID_KeyboardHID;
            USB_Descriptor_Endpoint_t               HID_ReportINEndpoint;
            
            // RawHID Interface
            USB_Descriptor_Interface_t              RawHID_Interface;
            USB_HID_Descriptor_HID_t                RawHID_RawHID;
            USB_Descriptor_Endpoint_t               RawHID_ReportINEndpoint;
        } USB_Descriptor_Configuration_t;

        /** Enum for the device interface descriptor IDs within the device. Each interface descriptor
         *  should have a unique ID index associated with it, which can be used to refer to the
         *  interface from other descriptors.
         */
        enum InterfaceDescriptors_t
        {
            INTERFACE_ID_Keyboard = 0, /**< Keyboard interface descriptor ID */
            INTERFACE_ID_RawHID   = 1, /**< RawHID interface descriptor ID */
        };

        /** Enum for the device string descriptor IDs within the device. Each string descriptor should
         *  have a unique ID index associated with it, which can be used to refer to the string from
         *  other descriptors.
         */
        enum StringDescriptors_t
        {
            STRING_ID_Language     = 0, /**< Supported Languages string descriptor ID (must be zero) */
            STRING_ID_Manufacturer = 1, /**< Manufacturer string ID */
            STRING_ID_Product      = 2, /**< Product string ID */
        };

    /* Macros: */
        /** Endpoint address of the Keyboard HID reporting IN endpoint. */
        #define KEYBOARD_EPADDR                 (ENDPOINT_DIR_IN | 1)

        /** Endpoint addresses of the RawHID interface */
        #define RAWHID_IN_EPADDR                (ENDPOINT_DIR_IN | 2)
        #define RAWHID_OUT_EPADDR               (ENDPOINT_DIR_OUT | 3)

/** Size in bytes of the Keyboard HID reporting IN endpoint. */
/** 1 (ReportID) + 2 (Modifiers + Reserved) + 32KRO */
/** [first byte required due to multiple ReportIDs and not just one IN report] */
#define KEYBOARD_EPSIZE                 (1 + 2 + MAX_NKRO)
#define RAWHID_IN_EPSIZE                (1 + FETCH_CONFIG_REPORT_SIZE)
#define RAWHID_OUT_EPSIZE               (16)  // Reasonable amount

    /* Report ID Codes */
        /** Keyboard Lock LEDs Request is the same as Keyboard HID ReportID */
        #define LOCK_LEDS_REPORT_ID                 HID_KEYBOARD_REPORT_ID

        /** Runtime Keymap Edit Request */
        #define EDIT_KEYMAP_REPORT_ID               0xA0
        #define EDIT_KEYMAP_PARAMETERS_BYTES        4

        /** Runtime Actuation Edit Request */
        #define EDIT_ACTUATIONS_REPORT_ID           0xA1
        #define EDIT_ACTUATIONS_PARAMETERS_BYTES    4

        /** Runtime SnapTap Edit Request */
        #define EDIT_SNAPTAP_A_REPORT_ID            0xB0
        #define EDIT_SNAPTAP_B_REPORT_ID            0xB1
        #define EDIT_SNAPTAP_PARAMETERS_BYTES       4

        /** Runtime Fetch Configuration Request */
        #define FETCH_CONFIG_REPORT_ID              0xC0
        #define FETCH_CONFIG_REPORT_SIZE            (2 * MAX_KEYS_SUPPORTED + 2 + 6)

    /* Function Prototypes: */
    /*
        uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                            const uint16_t wIndex,
                                            const void** const DescriptorAddress)
                                            ATTR_WARN_UNUSED_RESULT ATTR_NON_NULL_PTR_ARG(3);
     */

#endif

