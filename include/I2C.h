#ifndef CUSTOM_I2C_H
#define CUSTOM_I2C_H

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <stdint.h>

void i2c_init_master(void);
uint8_t i2c_start(uint8_t address);
uint8_t i2c_write(uint8_t data);
uint8_t i2c_read_ack(void);
uint8_t i2c_read_nack(void);
void i2c_stop(void);
void i2c_send_decimal(uint8_t addr, uint8_t value);
void i2c_send_pipedash(uint8_t addr);

#endif /* CUSTOM_I2C_H */
