#ifndef SRC_CUSTOM_I2C_H
#define SRC_CUSTOM_I2C_H

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/twi.h>
#include <stdint.h>
#include <stdio.h>

/*
 * Initialize TWI (I2C) as master, 400 kHz SCL
 */
void i2c_init_master(void) {
    /* prescaler = 1 */
    TWSR = 0x00;
    /* TWBR = (F_CPU/SCL - 16) / 2 = (16e6/400e3 - 16)/2 = 12 */
    TWBR = 12;
    /* enable TWI */
    TWCR = (1 << TWEN);
}

/*
 * Send START and SLA+R/W.
 * address = 7‑bit slave address <<1 | R/W (0=write, 1=read).
 * returns 0 if ACK received, else non‑zero.
 */
uint8_t i2c_start(uint8_t address) {
    /* send START */
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)))
        ;
    if ((TWSR & 0xF8) != TW_START)
        return 1;

    /* send SLA+R/W */
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

/*
 * Write one byte, return 0 if ACK received
 */
uint8_t i2c_write(uint8_t data) {
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)))
        ;
    if ((TWSR & 0xF8) != TW_MT_DATA_ACK)
        return 1;
    return 0;
}

/*
 * Read one byte and send ACK
 */
uint8_t i2c_read_ack(void) {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    while (!(TWCR & (1 << TWINT)))
        ;
    return TWDR;
}

/*
 * Read one byte and send NACK (last byte)
 */
uint8_t i2c_read_nack(void) {
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)))
        ;
    return TWDR;
}

/*
 * Send STOP condition
 */
void i2c_stop(void) {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
    while (TWCR & (1 << TWSTO))
        ;
}

/*
 * Send a uint8_t as a decimal string to I2C address 0x72
 * Uses snprintf from <stdio.h> to convert the value.
 */
void i2c_send_decimal(uint8_t addr, uint8_t value) {
    char buf[4];           /* enough for "255" + '\0' */
    int len;

    /* format the decimal string */
    len = snprintf(buf, sizeof(buf), "%u", value);
    if (len <= 0)          /* formatting error? */
        return;

    /* start I2C write to addr */
    if (i2c_start((addr << 1) | 0) != 0)
        return;

    /* send each character */
    for (int i = 0; i < len; ++i) {
        if (i2c_write(buf[i]) != 0)
            break;
    }

    i2c_stop();
}

void i2c_send_pipedash(uint8_t addr) {
    /* start I2C write to addr */
    if (i2c_start((addr << 1) | 0) != 0)
        return;

    i2c_write('|');
    i2c_write('-');
    i2c_stop();
}

#endif /* SRC_CUSTOM_I2C_H */