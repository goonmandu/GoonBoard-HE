#include "Keyboard.h"
#include "SPI.h"
#include "I2C.h"
#include "MatrixScanner.h"

extern uint8_t PrevKeyboardHIDReportBuffer[sizeof(USB_KeyboardReport_Data_t)];
extern USB_ClassInfo_HID_Device_t Keyboard_HID_Interface;

int main(void) {
	SetupHardware();

    DDRB |= (1 << PB0);
    DDRD |= (1 << PD5);
    PORTB |= (1 << PB0);
    PORTD |= (1 << PD5);

	// PF7 is Logic Analyzer Debug Bit
	DDRF |= (1 << PF7);

	// PC6 is Rapid-Trigger Enable
	DDRC &= ~(1 << PC6);  // Set PC6 as input
	PORTC |= (1 << PC6);  // Enable pull-up

	spi_init_master();
	// i2c_init_master();
	setup_mux_pins();
	
	GlobalInterruptEnable();

	for (;;) {
		HID_Device_USBTask(&Keyboard_HID_Interface);
		USB_USBTask();
	}
}