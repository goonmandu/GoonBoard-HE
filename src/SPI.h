#ifndef SRC_CUSTOM_SPI_H
#define SRC_CUSTOM_SPI_H

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>

#define SPI_SS_PIN   PB6
#define SPI_MOSI     PB2
#define SPI_SCK      PB1
#define SPI_MISO     PB3

/*
 * Initialize SPI as master at fosc/2 (highest SCLK), mode 3 (CPOL=1, CPHA=1)
 */
void spi_init_master(void) {
    /* MOSI, SCK, SS outputs; MISO input */
    DDRB |= (1 << SPI_MOSI)
          |  (1 << SPI_SCK)
          |  (1 << SPI_SS_PIN);
    DDRB &= ~(1 << SPI_MISO);

    /* Deselect slave (idle high) */
    PORTB |= (1 << SPI_SS_PIN);

    /* Enable SPI, master mode, mode 3 */
    SPCR = (1 << SPE)
         | (1 << MSTR)
         | (1 << CPOL)
         | (1 << CPHA);

    /* Double speed for fosc/2 */
    SPSR |= (1 << SPI2X);
}

/*
 * Transfer one byte over SPI and return the received byte
 */
uint8_t spi_transfer(uint8_t data) {
    SPDR = data;
    while (!(SPSR & (1 << SPIF)))
        ;
    return SPDR;
}

/*
 * Read one byte by sending dummy 0xFF
 */
uint8_t spi_read(void) {
    return spi_transfer(0xFF);
}

/*
 * Read 8 data bits, skipping exactly one padding bit
 * in SPI mode 3 (CPOL=1, CPHA=1). SS must already be low.
 */
uint8_t spi_read_skip_first_bit_mode3(void) {
    /* 1. Disable hardware SPI */
    SPCR &= ~(1 << SPE);

    /* 2. Ensure SCK idle high */
    PORTB |= (1 << SPI_SCK);

    /* 3. Manually clock one full cycle: high→low→high */
    PORTB &= ~(1 << SPI_SCK);
    PORTB |=  (1 << SPI_SCK);

    /* 4. Clear any pending SPIF */
    (void)SPSR;
    (void)SPDR;

    /* 5. Re-enable SPI hardware */
    SPCR |= (1 << SPE);

    /* 6. Read the next 8 bits */
    return spi_read();
}


void spi_select_slave() {
    PORTB &= ~(1 << SPI_SS_PIN);
}

void spi_deselect_slave() {
    PORTB |= (1 << SPI_SS_PIN);
}

#endif /* SRC_CUSTOM_SPI_H */