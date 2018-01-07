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

#include "Adafruit_ILI9341.h"
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



#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04

/*
 * Control Pins
 * */

#define PIN_NUM_MISO 25
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  19
#define PIN_NUM_CS   22

#define PIN_NUM_DC   21
#define PIN_NUM_RST  18
#define PIN_NUM_BCKL 5

static spi_device_handle_t spi;

/*
 * Software SPI Macros
 * */


#define SPI_WRITE(v)           spi_write_data(v)
#define SPI_WRITE16(s)         SPI_WRITE((s) >> 8); SPI_WRITE(s)
#define SPI_WRITE32(l)         SPI_WRITE((l) >> 24); SPI_WRITE((l) >> 16); SPI_WRITE((l) >> 8); SPI_WRITE(l)
#define SPI_WRITE_PIXELS(c,l)  for(uint32_t i=0; i<(l); i+=2){ SPI_WRITE(((uint8_t*)(c))[i+1]); SPI_WRITE(((uint8_t*)(c))[i]); }

/*
 * Hardware SPI Macros
 * */

#define SPI_HAS_WRITE_PIXELS

// Optimized SPI (ESP8266 and ESP32)
//#define HSPI_READ()              SPI_OBJECT.transfer(0)
//#define HSPI_WRITE(b)            SPI_OBJECT.write(b)
//#define HSPI_WRITE16(s)          SPI_OBJECT.write16(s)
//#define HSPI_WRITE32(l)          SPI_OBJECT.write32(l)
//#ifdef SPI_HAS_WRITE_PIXELS
#define SPI_MAX_PIXELS_AT_ONCE  32
//#define HSPI_WRITE_PIXELS(c,l)   SPI_OBJECT.writePixels(c,l)

/*
 * Final SPI Macros
 * */


/*Local variables*/
uint16_t _height = 0;
 uint16_t _width = 0;
 uint32_t _freq =0;
 uint16_t rotation =0;

// Pass 8-bit (each) R,G,B, get back 16-bit packed color
uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
	return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3);
}


//Initialize the display
void ili_init()
{

    //Initialize non-SPI GPIOs
    gpio_set_direction(PIN_NUM_DC, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_RST, GPIO_MODE_OUTPUT);
//    gpio_set_direction(PIN_NUM_BCKL, GPIO_MODE_OUTPUT);

    //Reset the display
    gpio_set_level(PIN_NUM_RST, 0);
    vTaskDelay(100 / portTICK_RATE_MS);
    gpio_set_level(PIN_NUM_RST, 1);
    vTaskDelay(100 / portTICK_RATE_MS);


    ///Enable backlight
//    gpio_set_level(PIN_NUM_BCKL, 1);
}


void begin(void)

{
	spi_write_command(0xEF);
	spi_write_data(0x03);
	spi_write_data(0x80);
	spi_write_data(0x02);

	spi_write_command(0xCF);
	spi_write_data(0x00);
	spi_write_data(0XC1);
	spi_write_data(0X30);

	spi_write_command(0xED);
	spi_write_data(0x64);
	spi_write_data(0x03);
	spi_write_data(0X12);
	spi_write_data(0X81);

	spi_write_command(0xE8);
	spi_write_data(0x85);
	spi_write_data(0x00);
	spi_write_data(0x78);

	spi_write_command(0xCB);
	spi_write_data(0x39);
	spi_write_data(0x2C);
	spi_write_data(0x00);
	spi_write_data(0x34);
	spi_write_data(0x02);

	spi_write_command(0xF7);
	spi_write_data(0x20);

	spi_write_command(0xEA);
	spi_write_data(0x00);
	spi_write_data(0x00);

	spi_write_command(ILI9341_PWCTR1);    //Power control
	spi_write_data(0x23);   //VRH[5:0]

	spi_write_command(ILI9341_PWCTR2);    //Power control
	spi_write_data(0x10);   //SAP[2:0];BT[3:0]

	spi_write_command(ILI9341_VMCTR1);    //VCM control
	spi_write_data(0x3e);
	spi_write_data(0x28);

	spi_write_command(ILI9341_VMCTR2);    //VCM control2
	spi_write_data(0x86);  //--

	spi_write_command(ILI9341_MADCTL);    // Memory Access Control
	spi_write_data(0x48);

	spi_write_command(ILI9341_VSCRSADD); // Vertical scroll
	SPI_WRITE16(0);                 // Zero

	spi_write_command(ILI9341_PIXFMT);
	spi_write_data(0x55);

	spi_write_command(ILI9341_FRMCTR1);
	spi_write_data(0x00);
	spi_write_data(0x18);

	spi_write_command(ILI9341_DFUNCTR);    // Display Function Control
	spi_write_data(0x08);
	spi_write_data(0x82);
	spi_write_data(0x27);

	spi_write_command(0xF2);    // 3Gamma Function Disable
	spi_write_data(0x00);

	spi_write_command(ILI9341_GAMMASET);    //Gamma curve selected
	spi_write_data(0x01);

	spi_write_command(ILI9341_GMCTRP1);    //Set Gamma
	spi_write_data(0x0F);
	spi_write_data(0x31);
	spi_write_data(0x2B);
	spi_write_data(0x0C);
	spi_write_data(0x0E);
	spi_write_data(0x08);
	spi_write_data(0x4E);
	spi_write_data(0xF1);
	spi_write_data(0x37);
	spi_write_data(0x07);
	spi_write_data(0x10);
	spi_write_data(0x03);
	spi_write_data(0x0E);
	spi_write_data(0x09);
	spi_write_data(0x00);

	spi_write_command(ILI9341_GMCTRN1);    //Set Gamma
	spi_write_data(0x00);
	spi_write_data(0x0E);
	spi_write_data(0x14);
	spi_write_data(0x03);
	spi_write_data(0x11);
	spi_write_data(0x07);
	spi_write_data(0x31);
	spi_write_data(0xC1);
	spi_write_data(0x48);
	spi_write_data(0x08);
	spi_write_data(0x0F);
	spi_write_data(0x0C);
	spi_write_data(0x31);
	spi_write_data(0x36);
	spi_write_data(0x0F);

	spi_write_command(ILI9341_SLPOUT);    //Exit Sleep
	//delay(120);
	spi_write_command(ILI9341_DISPON);    //Display on
	//delay(120);

	_width = ILI9341_TFTWIDTH;
	_height = ILI9341_TFTHEIGHT;

	setRotation(3);
	printf("begin - finished \n");
}

void setRotation(uint8_t m) {
	rotation = m % 4; // can't be higher than 3
	switch (rotation) {
	case 0:
		m = (MADCTL_MX | MADCTL_BGR);
		_width = ILI9341_TFTWIDTH;
		_height = ILI9341_TFTHEIGHT;
		break;
	case 1:
		m = (MADCTL_MV | MADCTL_BGR);
		_width = ILI9341_TFTHEIGHT;
		_height = ILI9341_TFTWIDTH;
		break;
	case 2:
		m = (MADCTL_MY | MADCTL_BGR);
		_width = ILI9341_TFTWIDTH;
		_height = ILI9341_TFTHEIGHT;
		break;
	case 3:
		m = (MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
		_width = ILI9341_TFTHEIGHT;
		_height = ILI9341_TFTWIDTH;
		break;
	}


	spi_write_command(ILI9341_MADCTL);
	spi_write_data(m);

}

void invertDisplay(uint8_t i) {
	spi_write_command(i ? ILI9341_INVON : ILI9341_INVOFF);
}

void scrollTo(uint16_t y) {
	spi_write_command(ILI9341_VSCRSADD);
	SPI_WRITE16(y);
}

uint8_t spiRead() {

	uint8_t r = 0;

	return r;
}



/*
 * Transaction API
 * */



void spi_pre_transfer_callback(spi_transaction_t *t){
	int dc=(int)t->user;
	    gpio_set_level(PIN_NUM_DC, dc);
}

void spiInit(void){

	 esp_err_t ret;
	    spi_bus_config_t buscfg={
	        .miso_io_num=PIN_NUM_MISO,
	        .mosi_io_num=PIN_NUM_MOSI,
	        .sclk_io_num=PIN_NUM_CLK,
	        .quadwp_io_num=-1,
	        .quadhd_io_num=-1
	    };
	    spi_device_interface_config_t devcfg={
	        .clock_speed_hz=10000000,               //Clock out at 10 MHz
	        .mode=0,                                //SPI mode 0
	        .spics_io_num=PIN_NUM_CS,               //CS pin
	        .queue_size=7,                          //We want to be able to queue 7 transactions at a time
	        .pre_cb=spi_pre_transfer_callback,  //Specify pre-transfer callback to handle D/C line
	    };
	    //Initialize the SPI bus
	    ret=spi_bus_initialize(HSPI_HOST, &buscfg, 1);
	    assert(ret==ESP_OK);

	    //Attach the LCD to the SPI bus
	    ret=spi_bus_add_device(HSPI_HOST, &devcfg, &spi);
	    assert(ret==ESP_OK);

}


void spi_write_command(const uint8_t cmd) {

    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=8;                     //Command is 8 bits
    t.tx_buffer=&cmd;               //The data is the cmd itself
    t.user=(void*)0;                //D/C needs to be set to 0
    ret=spi_device_transmit(spi, &t);  //Transmit!
    assert(ret==ESP_OK);            //Should have had no issues.

}

void spi_write_data(const uint8_t data) {

    esp_err_t ret;
    spi_transaction_t t;
    uint16_t len = 1;
    if (len==0) return;             //no need to send anything
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=len*8;                 //Len is in bytes, transaction length is in bits.
    t.tx_buffer=&data;               //Data
    t.user=(void*)1;                //D/C needs to be set to 1
    ret=spi_device_transmit(spi, &t);  //Transmit!
    assert(ret==ESP_OK);            //Should have had no issues.

}

void setAddrWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
	uint32_t xa = ((uint32_t) x << 16) | (x + w - 1);
	uint32_t ya = ((uint32_t) y << 16) | (y + h - 1);
	spi_write_command(ILI9341_CASET); // Column addr set
	SPI_WRITE32(xa);
	spi_write_command(ILI9341_PASET); // Row addr set
	SPI_WRITE32(ya);
	spi_write_command(ILI9341_RAMWR); // write to RAM
}

void pushColor(uint16_t color) {

	SPI_WRITE16(color);

}

void spi_writePixel(uint16_t color) {
	SPI_WRITE16(color);
}

void writePixels(uint16_t * colors, uint32_t len) {
	SPI_WRITE_PIXELS((uint8_t* )colors, len * 2);
}

void writeColor(uint16_t color, uint32_t len) {

		for (uint32_t t = 0; t < len; t++) {
			spi_writePixel(color);
		}
		return;

}

/*Write one pixel directly*/
void writePixel(int16_t x, int16_t y, uint16_t color) {
	if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height))
		return;
	setAddrWindow(x, y, 1, 1);
	spi_writePixel(color);
}

/*Draw rectangle fast*/
void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
	if ((x >= _width) || (y >= _height))
		return;
	int16_t x2 = x + w - 1, y2 = y + h - 1;
	if ((x2 < 0) || (y2 < 0))
		return;

	// Clip left/top
	if (x < 0) {
		x = 0;
		w = x2 + 1;
	}
	if (y < 0) {
		y = 0;
		h = y2 + 1;
	}

	// Clip right/bottom
	if (x2 >= _width)
		w = _width - x;
	if (y2 >= _height)
		h = _height - y;

	int32_t len = (int32_t) w * h;
	setAddrWindow(x, y, w, h);
	writeColor(color, len);
}

/*Write V line fast*/
void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
	writeFillRect(x, y, 1, h, color);
}

/*Write H line fast*/
void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
	writeFillRect(x, y, w, 1, color);
}

/*TODO check it what for*/
uint8_t readcommand8(uint8_t c, uint8_t index) {
	uint32_t freq = _freq;
	if (_freq > 24000000) {
		_freq = 24000000;
	}
	spi_write_command(0xD9);  // woo sekret command?
	spi_write_data(0x10 + index);
	spi_write_command(c);
	uint8_t r = spiRead();
	_freq = freq;
	return r;
}
