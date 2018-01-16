/***************************************************
 This is our library for the Adafruit ILI9341 Breakout and Shield
 ----> http://www.adafruit.com/products/1651

 Check out the links above for our tutorials and wiring diagrams
 These displays use SPI to communicate, 4 or 5 pins are required to
 interface (RST is optional)
 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries.
 MIT license, all text above must be included in any redistribution
 *******************************************************************
 Library modified for ESP32 RTOS-SDK usage
 *******************************************************************/

#include "sm_spi.h"

#include "stdint.h"
#include <limits.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"

#include "driver/gpio.h"

#include "sm_config.h"


/*
 * Software SPI Macros
 * */

#define SPI_WRITE(v)           spi_write_data(v)
#define SPI_WRITE16(s)         SPI_WRITE((s) >> 8); SPI_WRITE(s)
#define SPI_WRITE32(l)         SPI_WRITE((l) >> 24); SPI_WRITE((l) >> 16); SPI_WRITE((l) >> 8); SPI_WRITE(l)
#define SPI_WRITE_PIXELS(c,l)  for(uint32_t i=0; i<(l); i+=2){ SPI_WRITE(((uint8_t*)(c))[i+1]); SPI_WRITE(((uint8_t*)(c))[i]); }

#define DATA_TRANSFER (1u) //1 = data; 0 = command

enum
{
	ILI9341_DC_TRANSFER_COMMAND,
	ILI9341_DC_TRANSFER_DATA
};


void spi_init(spi_device_handle_t *spi, spi_bus_config_t *buscfg, spi_device_interface_config_t *devcfg )
{
	esp_err_t ret;

	/*Initialize the SPI bus*/
	ret = spi_bus_initialize(HSPI_HOST, buscfg, 1);
	assert(ret==ESP_OK);

	/*Attach the LCD to the SPI bus*/
	ret = spi_bus_add_device(HSPI_HOST, devcfg, spi);
	assert(ret==ESP_OK);

}


void spi_write(spi_device_handle_t spi, uint8_t* data, uint16_t len)
{

	esp_err_t ret;
	spi_transaction_t t;

	if (len == 0)
		return;             //no need to send anything

	memset(&t, 0, sizeof(t));       //Zero out the transaction
	t.length = len * 8;        //Len is in bytes, transaction length is in bits.
	t.tx_buffer = data;               //Data
	t.user = (void*) ILI9341_DC_TRANSFER_DATA;                //D/C needs to be set to 1
	ret = spi_device_transmit(spi, &t);  //Transmit!
	assert(ret==ESP_OK);            //Should have had no issues.

}


/*Write spi data with user field*/
void spi_write_u(spi_device_handle_t spi, uint8_t* data, int len, void *user)
{
    esp_err_t ret;
    spi_transaction_t t;

    if(len == 0)
    {
        return;                                              //no need to send anything
    }

    memset(&t, 0, sizeof(t));                                //Zero out the transaction
    t.length = len * 8;                                      //Len is in bytes, transaction length is in bits.
    t.tx_buffer = data;                                      //Data
    t.user = user;                                           //D/C needs to be set to 1
    ret = spi_device_queue_trans(spi, &t, portMAX_DELAY);    //Transmit!
    assert(ret==ESP_OK);                                     //Should have had no issues.
}






uint8_t spi_read(spi_device_handle_t spi)
{

	uint8_t r = 0;

	return r;
}

