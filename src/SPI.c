#include "SPI.h"
#include "Hacks.h"

void spi_init_master(void) {
    /* MOSI, SCK, SS outputs; MISO input */
    DDRB |= (1 << SPI_SCK)
          |  (1 << SPI_SS_PIN)
          |  (1 << SPI_MOSI);
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

uint8_t spi_transfer(uint8_t data) {
    SPDR = data;
    while (!(SPSR & (1 << SPIF)))
        ;
    return SPDR;
}

uint8_t spi_read(void) {
    return spi_transfer(0xFF);
}

void spi_select_slave(void) {
    PORTB &= ~(1 << SPI_SS_PIN);
}

void spi_deselect_slave(void) {
    PORTB |= (1 << SPI_SS_PIN);
}
