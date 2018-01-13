#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"

#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"


#include "sm_config.h"
#include <lcd_ili9341.h>
#include "Adafruit_GFX.h"
#include "ws2812.h"



#include <freertos/queue.h>
#include <freertos/task.h>

#include "sdkconfig.h"
#include <esp_log.h>

static char tag[] = "test_intr";
static QueueHandle_t q1;

#define TEST_GPIO (13)

static void handler(void *args) {
    gpio_num_t gpio;
    gpio = TEST_GPIO;
    xQueueSendToBackFromISR(q1, &gpio, NULL);
}

void test1_task(void *ignore) {
    ESP_LOGD(tag, ">> test1_task");
    gpio_num_t gpio;
    q1 = xQueueCreate(10, sizeof(gpio_num_t));

    gpio_config_t gpioConfig;
    gpioConfig.pin_bit_mask = GPIO_SEL_13;
    gpioConfig.mode         = GPIO_MODE_INPUT;
    gpioConfig.pull_up_en   = GPIO_PULLUP_DISABLE;
    gpioConfig.pull_down_en = GPIO_PULLDOWN_ENABLE;
    gpioConfig.intr_type    = GPIO_INTR_POSEDGE;
    gpio_config(&gpioConfig);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(TEST_GPIO, handler, NULL   );


    while(1) {
        ESP_LOGD(tag, "Waiting on interrupt queue");
        BaseType_t rc = xQueueReceive(q1, &gpio, portMAX_DELAY);
        ESP_LOGD(tag, "Woke from interrupt queue wait: %d", rc);
    }

    vTaskDelete(NULL);
}


void ws2812_task(void *pvParameters)
{
    uint8_t anim_max = 1;
    uint16_t step = 0;
    pixel_t pixel_color;

    while (1)
    {


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

            break;
        case 5:
            pixel_color.red = 0;
            pixel_color.green = 0;
            pixel_color.blue = 0;
            step = 0;
            break;

        default:
            step = 0;
            break;
        }



        for (uint8_t i = 0; i < 8; i++)
        {
            ws2812_set_led_pixel(i, pixel_color);
        }

        ws2812_show();

        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}



esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}


void main_display_task(void *pvParameter)
{
    il9341_init();


    uint16_t x = 0;
    uint16_t y = 0;

    cp437(0);
    setTextWrap(1);
    setTextSize(2);
    setTextColor(ILI9341_GREEN);
    setCursor(10, 10);
    writex('f');
    writex('a');

    while (true)
    {
        drawChar(100, 100, x, ILI9341_ORANGE, ILI9341_BLACK, 8);
        writePixel(x, y, ILI9341_GREEN);


        if (x > 240)
        {
            x = 0;
            y++;
            setCursor(10, 10);
            writex('d');
            writex('a');
        }
        else
        {
            x++;
        }

        if (y > 320)
        {
            y = 0;
        }

        writex(x);

         vTaskDelay(300 / portTICK_PERIOD_MS);


    }
}




void app_main(void)
{

    spi_il9341_config();
    ws2812_init(PIN_WS2812_LED, CNT_WS2812_LED, RMT_CH_WS2812_LED);

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




    xTaskCreate(main_display_task, "main_display_task", 1024, NULL, 5, NULL);
    xTaskCreate(ws2812_task, "ws2812 rainbow demo", 4096, NULL, 6, NULL);
    xTaskCreate(test1_task, "test1_task", 4096, NULL, 6, NULL);

}



