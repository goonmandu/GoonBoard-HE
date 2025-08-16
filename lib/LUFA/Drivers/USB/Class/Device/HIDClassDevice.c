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

#define  __INCLUDE_FROM_USB_DRIVER
#include "../../Core/USBMode.h"

#if defined(USB_CAN_BE_DEVICE)

#define  __INCLUDE_FROM_HID_DRIVER
#define  __INCLUDE_FROM_HID_DEVICE_C
#include "HIDClassDevice.h"

void HID_Device_ProcessControlRequest(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo)
{
	if (!(Endpoint_IsSETUPReceived()))
	  return;

	if (USB_ControlRequest.wIndex != HIDInterfaceInfo->Config.InterfaceNumber)
	  return;

	switch (USB_ControlRequest.bRequest)
	{
		case HID_REQ_GetReport:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				uint16_t ReportSize = 0;
				uint8_t  ReportID   = (USB_ControlRequest.wValue & 0xFF);
				uint8_t  ReportType = (USB_ControlRequest.wValue >> 8) - 1;
				uint8_t  ReportData[HIDInterfaceInfo->Config.PrevReportINBufferSize];

				memset(ReportData, 0, sizeof(ReportData));

				CALLBACK_HID_Device_CreateHIDReport(HIDInterfaceInfo, &ReportID, ReportType, ReportData, &ReportSize);

				if (HIDInterfaceInfo->Config.PrevReportINBuffer != NULL)
				{
					memcpy(HIDInterfaceInfo->Config.PrevReportINBuffer, ReportData,
					       HIDInterfaceInfo->Config.PrevReportINBufferSize);
				}

				Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);

				Endpoint_ClearSETUP();

				if (ReportID)
				  Endpoint_Write_8(ReportID);

				Endpoint_Write_Control_Stream_LE(ReportData, ReportSize);
				Endpoint_ClearOUT();
			}

			break;
		case HID_REQ_SetReport:
			if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				uint16_t ReportSize = USB_ControlRequest.wLength;
				uint8_t  ReportID   = (USB_ControlRequest.wValue & 0xFF);
				uint8_t  ReportType = (USB_ControlRequest.wValue >> 8) - 1;
				uint8_t  ReportData[ReportSize];

				Endpoint_ClearSETUP();
				Endpoint_Read_Control_Stream_LE(ReportData, ReportSize);
				Endpoint_ClearIN();

				CALLBACK_HID_Device_ProcessHIDReport(HIDInterfaceInfo, ReportID, ReportType,
				                                     &ReportData[ReportID ? 1 : 0], ReportSize);
			}

			break;
		case HID_REQ_GetProtocol:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();
				while (!(Endpoint_IsINReady()));
				Endpoint_Write_8(HIDInterfaceInfo->State.UsingReportProtocol);
				Endpoint_ClearIN();
				Endpoint_ClearStatusStage();
			}

			break;
		case HID_REQ_SetProtocol:
			if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();
				Endpoint_ClearStatusStage();

				HIDInterfaceInfo->State.UsingReportProtocol = ((USB_ControlRequest.wValue & 0xFF) != 0x00);
			}

			break;
		case HID_REQ_SetIdle:
			if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();
				Endpoint_ClearStatusStage();

				HIDInterfaceInfo->State.IdleCount = ((USB_ControlRequest.wValue & 0xFF00) >> 6);
			}

			break;
		case HID_REQ_GetIdle:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();
				while (!(Endpoint_IsINReady()));
				Endpoint_Write_8(HIDInterfaceInfo->State.IdleCount >> 2);
				Endpoint_ClearIN();
				Endpoint_ClearStatusStage();
			}

			break;
	}
}

bool HID_Device_ConfigureEndpoints(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo)
{
	memset(&HIDInterfaceInfo->State, 0x00, sizeof(HIDInterfaceInfo->State));
	HIDInterfaceInfo->State.UsingReportProtocol = true;
	HIDInterfaceInfo->State.IdleCount           = 500;

	HIDInterfaceInfo->Config.ReportINEndpoint.Type = EP_TYPE_INTERRUPT;

	if (!(Endpoint_ConfigureEndpointTable(&HIDInterfaceInfo->Config.ReportINEndpoint, 1)))
	  return false;

	return true;
}

void HID_Device_USBTask(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo)
{
	if (USB_DeviceState != DEVICE_STATE_Configured)
	  return;

	if (HIDInterfaceInfo->State.PrevFrameNum == USB_Device_GetFrameNumber())
	{
		#if defined(USB_DEVICE_OPT_LOWSPEED)
		if (!(USB_Options & USB_DEVICE_OPT_LOWSPEED))
		  return;
		#else
		return;
		#endif
	}

	Endpoint_SelectEndpoint(HIDInterfaceInfo->Config.ReportINEndpoint.Address);

	// Internal periodic report generation (keyboard poll)
	if (Endpoint_IsReadWriteAllowed())
	{
		uint8_t  ReportINData[HIDInterfaceInfo->Config.PrevReportINBufferSize];
		uint8_t  ReportID     = 0;
		uint16_t ReportINSize = 0;

		memset(ReportINData, 0, sizeof(ReportINData));

		bool ForceSend         = CALLBACK_HID_Device_CreateHIDReport(HIDInterfaceInfo, &ReportID, HID_REPORT_ITEM_In,
		                                                             ReportINData, &ReportINSize);
		bool StatesChanged     = false;
		bool IdlePeriodElapsed = (HIDInterfaceInfo->State.IdleCount && !(HIDInterfaceInfo->State.IdleMSRemaining));

		if (HIDInterfaceInfo->Config.PrevReportINBuffer != NULL)
		{
			StatesChanged = (memcmp(ReportINData, HIDInterfaceInfo->Config.PrevReportINBuffer, ReportINSize) != 0);
			memcpy(HIDInterfaceInfo->Config.PrevReportINBuffer, ReportINData, HIDInterfaceInfo->Config.PrevReportINBufferSize);
		}

		if (ReportINSize && (ForceSend || StatesChanged || IdlePeriodElapsed))
		{
			HIDInterfaceInfo->State.IdleMSRemaining = HIDInterfaceInfo->State.IdleCount;
			// This part is customized to send the Consumer Controls report alongside
			// the required Keyboard/Keypad report.

			// Select HID Keyboard Endpoint
			Endpoint_SelectEndpoint(HIDInterfaceInfo->Config.ReportINEndpoint.Address);

			if (ReportID == 0x01) {
				// This is true if and only if this function was called by the
				// periodic report generator.

				// Switching of hardware banks is automatically done by LUFA
				// when `Endpoint_ClearIN` is called so that data can be written
				// to another bank without interfering with the current transfer.

				// Send keyboard/keypad report with ReportID 0x01
				// to primary hardware bank
				uint8_t* KeyboardKeypadReport = ReportINData;
			  	Endpoint_Write_8(0x01);

				static uint8_t last_three_bytes_padding[3] = {0x00, 0x00, 0x00};
				Endpoint_Write_Stream_LE(KeyboardKeypadReport,
										 // Last 3 bytes are media keys intended
										 // to be sent out via Consumer Control report
										 ReportINSize - sizeof(last_three_bytes_padding),
										 NULL);
				Endpoint_Write_Stream_LE(last_three_bytes_padding,
										 sizeof(last_three_bytes_padding),
										 NULL);
				Endpoint_ClearIN();

				// No need to wait for endpoint to be cleared because
				// we are using two hardware banks
				// Endpoint_WaitUntilReady();

				// Now Consumer Controls Media keys, ReportID 0x02
				// to alternate hardware bank
				uint8_t* ConsumerControlsReport = ReportINData + (ReportINSize - sizeof(last_three_bytes_padding));
				Endpoint_Write_8(0x02);
				
				Endpoint_Write_Stream_LE(ConsumerControlsReport,
										 sizeof(last_three_bytes_padding),
										 NULL);
				Endpoint_ClearIN();
			} else {
				// All other reports
				if (ReportID) Endpoint_Write_8(ReportID);
				Endpoint_Write_Stream_LE(ReportINData,
										 ReportINSize,
										 NULL);
				Endpoint_ClearIN();
			}
		}

		HIDInterfaceInfo->State.PrevFrameNum = USB_Device_GetFrameNumber();
	}
}

#endif

