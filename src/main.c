#include "Keyboard.h"
#include "SPI.h"
#include "I2C.h"
#include "MatrixScanner.h"
#include "Hacks.h"

extern uint8_t PrevKeyboardHIDReportBuffer[sizeof(USB_KeyboardReport_Data_t)];
extern USB_ClassInfo_HID_Device_t Keyboard_HID_Interface;

int main(void) {
    SetupHardware();

    // Turn off Pro Micro TX/RX LEDs
    DDRB |= (1 << PB0);
    DDRD |= (1 << PD5);
    PORTB |= (1 << PB0);
    PORTD |= (1 << PD5);

    // PC6 is Rapid-Trigger Enable
    DDRC &= ~(1 << PC6);  // Set PC6 as input
    PORTC |= (1 << PC6);  // Enable pull-up

    DEBUG_BIT_PIN_CONFIG;

    spi_init_master();
    setup_mux_pins();

    measure_adc_baseline();
    
    GlobalInterruptEnable();

    while (1) {
        SET_DEBUG_BIT;
        HID_Device_USBTask(&Keyboard_HID_Interface);
        CLEAR_DEBUG_BIT;
        USB_USBTask();
    }
}