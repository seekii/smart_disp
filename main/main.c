#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

#include "Adafruit_ILI9341.h"
#include "Adafruit_GFX.h"

#define PIN_NUM_MISO 25
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  19
#define PIN_NUM_CS   22

#define PIN_NUM_DC   21
#define PIN_NUM_RST  18
#define PIN_NUM_BCKL 5


esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}



void blink_task(void *pvParameter)
{
    spiInit();
    begin();
    writePixel(50, 50, ILI9341_GREEN);

    writeFastVLine(50, 100, 100, ILI9341_ORANGE);
    writeFastHLine(0, 100, 100, ILI9341_BLUE);
    writeFillRect(200, 200, 30,40, ILI9341_RED);


   // drawTriangle(10,10, 20, 40, 50, 40, ILI9341_ORANGE);
    drawCircle(40, 50, 40, ILI9341_ORANGE);


    int x =  0;
    int y =0;

    cp437(0);
    setTextWrap(1);
    setTextSize(2);
    setTextColor2(ILI9341_BLUE, ILI9341_GREEN);
    setCursor(10, 10);
    writex('d');
    writex('a');


    int level = 0;
    while (true) {
        gpio_set_level(GPIO_NUM_4, level);
        drawChar(100, 100, x, ILI9341_ORANGE, ILI9341_BLACK, 8);
        writePixel(x, y, ILI9341_GREEN);
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

        level = !level;
        vTaskDelay(300 / portTICK_PERIOD_MS);
        printf("blink \n");
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
    gpio_set_direction(PIN_NUM_DC, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_RST, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_BCKL, GPIO_MODE_OUTPUT);

    //Reset the display
    gpio_set_level(PIN_NUM_RST, 0);
    vTaskDelay(100 / portTICK_RATE_MS);
    gpio_set_level(PIN_NUM_RST, 1);
    vTaskDelay(100 / portTICK_RATE_MS);

    gpio_set_level(GPIO_NUM_5, 1);


    gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_NUM_5, GPIO_MODE_OUTPUT);



    xTaskCreate(&blink_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);



}



