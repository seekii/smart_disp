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

#include <lcd_ili9341.h>
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

#include "sm_spi.h"
#include "sm_config.h"

#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04




enum{
	ILI9341_DC_TRANSFER_COMMAND,
	ILI9341_DC_TRANSFER_DATA
};


static void il9341_set_addr_window(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
static void writeColor(uint16_t color, uint32_t len);
static void il9341_gpio_init(void);
static void spi_write_command(uint8_t cmd);
static void spi_write_data_u8(uint8_t data);
static void spi_write_data_u16(uint16_t data);
static void spi_write_data_u32(uint32_t data);

/*Local variables*/
uint16_t _height = 0;
uint16_t _width = 0;
uint32_t _freq = 0;
uint16_t rotation = 0;



void il9341_init(void)

{

	il9341_gpio_init();

	spi_write_command(0xEF);
	spi_write_data_u8(0x03);
	spi_write_data_u8(0x80);
	spi_write_data_u8(0x02);

	spi_write_command(0xCF);
	spi_write_data_u8(0x00);
	spi_write_data_u8(0XC1);
	spi_write_data_u8(0X30);

	spi_write_command(0xED);
	spi_write_data_u8(0x64);
	spi_write_data_u8(0x03);
	spi_write_data_u8(0X12);
	spi_write_data_u8(0X81);

	spi_write_command(0xE8);
	spi_write_data_u8(0x85);
	spi_write_data_u8(0x00);
	spi_write_data_u8(0x78);

	spi_write_command(0xCB);
	spi_write_data_u8(0x39);
	spi_write_data_u8(0x2C);
	spi_write_data_u8(0x00);
	spi_write_data_u8(0x34);
	spi_write_data_u8(0x02);

	spi_write_command(0xF7);
	spi_write_data_u8(0x20);

	spi_write_command(0xEA);
	spi_write_data_u8(0x00);
	spi_write_data_u8(0x00);

	spi_write_command(ILI9341_PWCTR1);    //Power control
	spi_write_data_u8(0x23);   //VRH[5:0]

	spi_write_command(ILI9341_PWCTR2);    //Power control
	spi_write_data_u8(0x10);   //SAP[2:0];BT[3:0]

	spi_write_command(ILI9341_VMCTR1);    //VCM control
	spi_write_data_u8(0x3e);
	spi_write_data_u8(0x28);

	spi_write_command(ILI9341_VMCTR2);    //VCM control2
	spi_write_data_u8(0x86);  //--

	spi_write_command(ILI9341_MADCTL);    // Memory Access Control
	spi_write_data_u8(0x48);

	spi_write_command(ILI9341_VSCRSADD); // Vertical scroll
	spi_write_data_u16(0);                 // Zero

	spi_write_command(ILI9341_PIXFMT);
	spi_write_data_u8(0x55);

	spi_write_command(ILI9341_FRMCTR1);
	spi_write_data_u8(0x00);
	spi_write_data_u8(0x18);

	spi_write_command(ILI9341_DFUNCTR);    // Display Function Control
	spi_write_data_u8(0x08);
	spi_write_data_u8(0x82);
	spi_write_data_u8(0x27);

	spi_write_command(0xF2);    // 3Gamma Function Disable
	spi_write_data_u8(0x00);

	spi_write_command(ILI9341_GAMMASET);    //Gamma curve selected
	spi_write_data_u8(0x01);

	spi_write_command(ILI9341_GMCTRP1);    //Set Gamma
	spi_write_data_u8(0x0F);
	spi_write_data_u8(0x31);
	spi_write_data_u8(0x2B);
	spi_write_data_u8(0x0C);
	spi_write_data_u8(0x0E);
	spi_write_data_u8(0x08);
	spi_write_data_u8(0x4E);
	spi_write_data_u8(0xF1);
	spi_write_data_u8(0x37);
	spi_write_data_u8(0x07);
	spi_write_data_u8(0x10);
	spi_write_data_u8(0x03);
	spi_write_data_u8(0x0E);
	spi_write_data_u8(0x09);
	spi_write_data_u8(0x00);

	spi_write_command(ILI9341_GMCTRN1);    //Set Gamma
	spi_write_data_u8(0x00);
	spi_write_data_u8(0x0E);
	spi_write_data_u8(0x14);
	spi_write_data_u8(0x03);
	spi_write_data_u8(0x11);
	spi_write_data_u8(0x07);
	spi_write_data_u8(0x31);
	spi_write_data_u8(0xC1);
	spi_write_data_u8(0x48);
	spi_write_data_u8(0x08);
	spi_write_data_u8(0x0F);
	spi_write_data_u8(0x0C);
	spi_write_data_u8(0x31);
	spi_write_data_u8(0x36);
	spi_write_data_u8(0x0F);

	spi_write_command(ILI9341_SLPOUT);    //Exit Sleep
	//delay(120);
	spi_write_command(ILI9341_DISPON);    //Display on
	//delay(120);

	_width = ILI9341_TFTWIDTH;
	_height = ILI9341_TFTHEIGHT;

	il9341_set_rotation(3);
}

void il9341_set_rotation(uint8_t m)
{
	rotation = m % 4; // can't be higher than 3
	switch (rotation)
	{
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
	spi_write_data_u8(m);

}

void il9341_invert_display(uint8_t i)
{
	spi_write_command(i ? ILI9341_INVON : ILI9341_INVOFF);
}

void il9341_scroll_to(uint16_t y)
{
	spi_write_command(ILI9341_VSCRSADD);

	spi_write_data_u16(y);
}





static void il9341_set_addr_window(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    uint32_t xa = ((uint32_t) x << 16) | (x + w - 1);
    uint32_t ya = ((uint32_t) y << 16) | (y + h - 1);
    spi_write_command(ILI9341_CASET); // Column addr set
    spi_write_data_u32(xa);
    spi_write_command(ILI9341_PASET); // Row addr set
    spi_write_data_u32(ya);

    spi_write_command(ILI9341_RAMWR); // write to RAM
}

static void pushColor(uint16_t color)
{
    spi_write_data_u16(color);
}

static void writeColor(uint16_t color, uint32_t len)
{
    for(uint32_t t = 0; t < len; t++)
    {
        pushColor(color);
    }
}

/*Write one pixel directly*/
void writePixel(int16_t x, int16_t y, uint16_t color)
{
	if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height))
		return;
	il9341_set_addr_window(x, y, 1, 1);
	pushColor(color);
}

/*Draw rectangle fast*/
void il9341_write_fill_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	if ((x > _width) || (y > _height))
	{
		return;
	}

	int16_t x2 = x + w - 1, y2 = y + h - 1;

	if ((x2 < 0) || (y2 < 0))
	{
		return;
	}
	// Clip left/top
	if (x < 0)
	{
		x = 0;
		w = x2 + 1;
	}
	if (y < 0)
	{
		y = 0;
		h = y2 + 1;
	}

	// Clip right/bottom
	if (x2 >= _width)
		w = _width - x;
	if (y2 >= _height)
		h = _height - y;

	int32_t len = (int32_t) w * h;
	il9341_set_addr_window(x, y, w, h);
	writeColor(color, len);
}

void il9341_write_v_line(int16_t x, int16_t y, int16_t h, uint16_t color)
{
	il9341_write_fill_rect(x, y, 1, h, color);
}

void il9341_write_h_line(int16_t x, int16_t y, int16_t w, uint16_t color)
{
	il9341_write_fill_rect(x, y, w, 1, color);
}





//Initialize the display
static void il9341_gpio_init(void)
{

    //Initialize non-SPI GPIOs
    gpio_set_direction(PIN_NUM_DC, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_RST, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_BCKL, GPIO_MODE_OUTPUT);

    //Reset the display
    gpio_set_level(PIN_NUM_RST, 0);
    vTaskDelay(100 / portTICK_RATE_MS);
    gpio_set_level(PIN_NUM_RST, 1);
    vTaskDelay(100 / portTICK_RATE_MS);

    //Enable backlight
    gpio_set_level(PIN_NUM_BCKL, 1);
}


static void spi_write_command(uint8_t cmd)
{
    spi_write_u(SPI_IL9341_DEV, &cmd, 1, (void*) ILI9341_DC_TRANSFER_COMMAND);
}

static void spi_write_data_u8(uint8_t data)
{

    spi_write_u(SPI_IL9341_DEV, &data, 1, (void*) ILI9341_DC_TRANSFER_DATA);        //Should have had no issues.
}

static void spi_write_data_u16(uint16_t data)
{
    uint8_t buff[2];

    buff[0] = (data >> 8);
    buff[1] = (data);

    spi_write_u(SPI_IL9341_DEV, buff, 2, (void*) ILI9341_DC_TRANSFER_DATA);        //Should have had no issues.
}

static void spi_write_data_u32(uint32_t data)
{
    uint8_t buff[4];

    buff[0] = (data >> 24);
    buff[1] = (data >> 16);
    buff[2] = (data >> 8);
    buff[3] = (data);

    spi_write_u(SPI_IL9341_DEV, buff, 4, (void*) ILI9341_DC_TRANSFER_DATA);        //Should have had no issues.
}

void spi_il9341_transfer_callback(spi_transaction_t *t)
{
    int dc = (int) t->user;
    gpio_set_level(PIN_NUM_DC, dc);
}


