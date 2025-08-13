#ifndef CUSTOM_SPI_H
#define CUSTOM_SPI_H

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <stdint.h>

#define SPI_SS_PIN   PB6
#define SPI_MOSI     PB2
#define SPI_SCK      PB1
#define SPI_MISO     PB3

void spi_init_master(void);
uint8_t spi_transfer(uint8_t data);
uint8_t spi_read(void);
void spi_select_slave(void);
void spi_deselect_slave(void);

#endif /* CUSTOM_SPI_H */
