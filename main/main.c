#include <lcd_ili9341.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include <soc/rmt_struct.h>
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

#include "sm_config.h"

#include "Adafruit_GFX.h"


#include "ws2812.h"



#define WS2812_PIN  GPIO_NUM_4

#define delay_ms(ms) vTaskDelay((ms) / portTICK_RATE_MS)




//static rgbVal pixels[PIXEL_COUNT *sizeof(rgbVal)];

void rainbow(void *pvParameters)
{
//  const uint8_t anim_step = 10;
  const uint8_t anim_max = 1;
//  const uint8_t pixel_count = 8; // Number of your "pixels"
//  const uint8_t delay = 100; // duration between color changes
//  rgbVal color = makeRGBVal(anim_max, 0, 0);
  uint8_t step = 0;
//  rgbVal color2 = makeRGBVal(anim_max, 0, 0);
//  uint8_t step2 = 0;
 // rgbVal *pixels;



  pixel_t pixel_color;

    WS2812(GPIO_NUM_4, 8, RMT_CHANNEL_0);
    setColorOrder("RGB");

      void setPixel1(uint16_t index, uint8_t red, uint8_t green, uint8_t blue);

//  pixels = malloc(sizeof(rgbVal) * pixel_count);

  while (1) {





    for (uint8_t i = 0; i < 8; i++) {
        setPixel2(i, pixel_color);
    }

    step++;
        switch (step)
        {
        case 1:
            pixel_color.red = anim_max;
            pixel_color.green = 0;
            pixel_color.blue = 0;
            break;
        case 2:
            pixel_color.red = 0;
            pixel_color.green = anim_max;
            pixel_color.blue = 0;
            break;
        case 3:
            pixel_color.red = 0;
            pixel_color.green = 0;
            pixel_color.blue = anim_max;
            break;
        case 4:
            pixel_color.red = anim_max;
            pixel_color.green = anim_max;
            pixel_color.blue = anim_max;
            step=0;
            break;
        default:
            step=0;
            break;
        }

        show();
//
//
//
//    ws2812_setColors(pixel_count, pixels);


    printf("led task \n");
    vTaskDelay(400 / portTICK_PERIOD_MS);
  }
}



esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}



void main_display_task(void *pvParameter)
{

    il9341_init();
   // fillScreen(ILI9341_BLACK);

    int x =  0;
    int y =0;

    cp437(0);
    setTextWrap(1);
    setTextSize(2);
    setTextColor(ILI9341_GREEN);
    setCursor(10, 10);
    writex('f');
    writex('a');

    while (true) {


        drawChar(100, 100, x, ILI9341_ORANGE, ILI9341_BLACK, 8);
      //  writePixel(x, y, ILI9341_GREEN);
        if(x>240){
        	x=0;
        	y++;
            setCursor(10, 10);
            writex('d');
            writex('a');
        }
        else{
        	x++;
        }

        if(y>320){
        	y=0;
        }

        writex(x);


        vTaskDelay(300 / portTICK_PERIOD_MS);

    }
}



void app_main(void)
{
    nvs_flash_init();
//    tcpip_adapter_init();
//    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
//    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
//    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
//    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
//    wifi_config_t sta_config = {
//        .sta = {
//            .ssid = "access_point_name",
//            .password = "password",
//            .bssid_set = false
//        }
//    };
//    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
//    ESP_ERROR_CHECK( esp_wifi_start() );
//    ESP_ERROR_CHECK( esp_wifi_connect() );

    spi_il9341_config();


    xTaskCreate(main_display_task, "main_display_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
  //  xTaskCreate(rainbow, "ws2812 rainbow demo", 4096, NULL, 5, NULL);


}



