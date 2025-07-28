#include "I2C.h"
#include <avr/io.h>
#include <util/twi.h>
#include <stdio.h>

void i2c_init_master(void) {
    TWSR = 0x00;
    TWBR = 12;
    TWCR = (1 << TWEN);
}

uint8_t i2c_start(uint8_t address) {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)))
        ;
    if ((TWSR & 0xF8) != TW_START)
        return 1;

    TWDR = address;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)))
        ;
    {
        uint8_t st = TWSR & 0xF8;
        if (st != TW_MT_SLA_ACK && st != TW_MR_SLA_ACK)
            return 2;
    }
    return 0;
}

uint8_t i2c_write(uint8_t data) {
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)))
        ;
    if ((TWSR & 0xF8) != TW_MT_DATA_ACK)
        return 1;
    return 0;
}

uint8_t i2c_read_ack(void) {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    while (!(TWCR & (1 << TWINT)))
        ;
    return TWDR;
}

uint8_t i2c_read_nack(void) {
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)))
        ;
    return TWDR;
}

void i2c_stop(void) {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
    while (TWCR & (1 << TWSTO))
        ;
}

void i2c_send_decimal(uint8_t addr, uint8_t value) {
    char buf[4];
    int len = snprintf(buf, sizeof(buf), "%u", value);
    if (len <= 0)
        return;

    if (i2c_start((addr << 1) | 0) != 0)
        return;

    for (int i = 0; i < len; ++i) {
        if (i2c_write(buf[i]) != 0)
            break;
    }

    i2c_stop();
}

void i2c_send_pipedash(uint8_t addr) {
    if (i2c_start((addr << 1) | 0) != 0)
        return;

    i2c_write('|');
    i2c_write('-');
    i2c_stop();
}
