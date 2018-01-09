/******************************************************************
 This is our library for the Adafruit ILI9341 Breakout and Shield
 ----> http://www.adafruit.com/products/1651

 Check out the links above for our tutorials and wiring diagrams
 These displays use SPI to communicate, 4 or 5 pins are required
 to interface (RST is optional)
 Adafruit invests time and resources providing this open source
 code, please support Adafruit and open-source hardware by
 purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries.
 MIT license, all text above must be included in any redistribution
 *******************************************************************
 Library modified for ESP32 RTOS-SDK usage
 *******************************************************************/
#ifndef _SM_SPI_H_
#define _SM_SPI_H_


#include "driver/spi_master.h"
#include "driver/gpio.h"




#define SPI_MAX_DATA_SIZE (255u)


 uint8_t spi_tx_buff[SPI_MAX_DATA_SIZE];



void spi_init(spi_device_handle_t *spi, spi_bus_config_t *buscfg, spi_device_interface_config_t *devcfg);
void spi_write(spi_device_handle_t spi, uint8_t* data, uint16_t len);
void spi_write_u(spi_device_handle_t spi, uint8_t* data, int len, void *user);



uint8_t spi_read(spi_device_handle_t spi);


#endif
