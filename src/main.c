#include "Keyboard.h"
#include "SPI.h"
#include "I2C.h"
#include "MatrixScanner.h"
#include "Hacks.h"
#include "SettingsEditor.h"

extern uint8_t PrevKeyboardHIDReportBuffer[PREV_REPORT_BUFFER_SIZE];
extern USB_ClassInfo_HID_Device_t Keyboard_HID_Interface;
extern USB_ClassInfo_HID_Device_t RawHID_HID_Interface;

int main(void) {
    SetupHardware();

    // PC6 is Rapid-Trigger Enable
    DDRC &= ~(1 << PC6);  // Set PC6 as input
    PORTC |= (1 << PC6);  // Enable pull-up

    DEBUG_BIT_PIN_CONFIG;
    CLEAR_DEBUG_BIT;

    spi_init_master();
    setup_mux_pins();
    setup_led_pins();

    load_eeprom_to_sram();
    measure_adc_baseline();

    // Turn off Pro Micro TX/RX LEDs
    DDRB |= (1 << PB0);
    DDRD |= (1 << PD5);
    PORTB |= (1 << PB0);
    PORTD |= (1 << PD5);
    
    GlobalInterruptEnable();

    while (1) {
        SET_DEBUG_BIT;
        // This is the keyboard generating its OWN report
        // every 1 ms (because the firmware is 1000 Hz).
        HID_Device_USBTask(&Keyboard_HID_Interface);
        HID_Device_USBTask(&RawHID_HID_Interface);
        CLEAR_DEBUG_BIT;

        // This is polled continuously (because HID_Device_USBTask
        // exits early if there is no data to send) to serviceUSB requests
        // USB requests from the host.
        USB_USBTask();
    }
}