#ifndef _SM_CONFIG_H_
#define _SM_CONFIG_H_


#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include <driver/rmt.h>


#define TRUE (1u)
#define SET (1u)
#define ENABLE (1u)

#define FALSE (0u)
#define RESET (0u)
#define DISABLE (0u)
/*
 * Control Pins
 * */

#define PIN_NUM_MISO GPIO_NUM_25
#define PIN_NUM_MOSI GPIO_NUM_23
#define PIN_NUM_CLK  GPIO_NUM_19
#define PIN_NUM_CS   GPIO_NUM_22


#define PIN_NUM_DC   GPIO_NUM_21
#define PIN_NUM_RST  GPIO_NUM_18
#define PIN_NUM_BCKL GPIO_NUM_5



#define PIN_WS2812_LED        GPIO_NUM_4
#define RMT_CH_WS2812_LED  RMT_CHANNEL_0


#define CNT_WS2812_LED                8u




spi_device_handle_t spi_il9341;


#define SPI_IL9341_DEV spi_il9341

void spi_il9341_config(void);


#endif
