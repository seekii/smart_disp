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

#include "sm_config.h"

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
#include "driver/spi_master.h"
#include "sm_spi.h"
#include "lcd_ili9341.h"







#define SPI_IL9341_CB (void*) &spi_il9341_transfer_callback;


static spi_bus_config_t lcd_il9341_spi_buscfg;
static spi_device_interface_config_t lcd_il9341_spi_devcfg;

void spi_il9341_config(void)
{


	lcd_il9341_spi_buscfg.miso_io_num = PIN_NUM_MISO;
	lcd_il9341_spi_buscfg.mosi_io_num = PIN_NUM_MOSI;
	lcd_il9341_spi_buscfg.sclk_io_num = PIN_NUM_CLK;
	lcd_il9341_spi_buscfg.quadwp_io_num = -1;
	lcd_il9341_spi_buscfg.quadhd_io_num = -1;

	lcd_il9341_spi_devcfg.clock_speed_hz = 10000000;    //Clock out at 10 MHz
	lcd_il9341_spi_devcfg.mode = 0;                    //SPI mode 0
	lcd_il9341_spi_devcfg.spics_io_num = PIN_NUM_CS;   //CS pin
	lcd_il9341_spi_devcfg.queue_size = 7; 				//We want to be able to queue 7 transactions at a time
	lcd_il9341_spi_devcfg.pre_cb = SPI_IL9341_CB; //Specify pre-transfer callback to handle D/C line


	spi_init(&spi_il9341, &lcd_il9341_spi_buscfg, &lcd_il9341_spi_devcfg);

}
