#include <lcd_gfx.h>
#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"

#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"


#include "sm_config.h"
#include <lcd_ili9341.h>
#include "ws2812.h"

#include <freertos/queue.h>
#include <freertos/task.h>

#include "sdkconfig.h"
#include <esp_log.h>
#include <esp_intr_alloc.h>





extern const unsigned char bmp_water [];
extern const unsigned char bmp_bulb [];
extern const unsigned char bmp_temperature [];


static char tag[] = "test_intr";
static QueueHandle_t q1;
static QueueHandle_t q2;

#define TEST_GPIO (13)

static void handler(void *args) {

    uint32_t tick_val = xTaskGetTickCountFromISR();


    xQueueSendToBackFromISR(q1, &tick_val, NULL);
}

void power_handle_task(void *ignore) {

    gpio_num_t gpio;
    q1 = xQueueCreate(10, sizeof(uint32_t));
    q2 = xQueueCreate(5, sizeof(uint32_t));

    gpio_config_t gpioConfig;
    gpioConfig.pin_bit_mask = GPIO_SEL_13;
    gpioConfig.mode         = GPIO_MODE_INPUT;
    gpioConfig.pull_up_en   = GPIO_PULLUP_DISABLE;
    gpioConfig.pull_down_en = GPIO_PULLDOWN_ENABLE;
    gpioConfig.intr_type    = GPIO_INTR_NEGEDGE;
    gpio_config(&gpioConfig);

    gpio_install_isr_service(ESP_INTR_FLAG_EDGE);
    gpio_isr_handler_add(TEST_GPIO, handler, NULL   );


    static uint32_t pre_tick_val;
    static uint32_t tick_val;
    static uint32_t tick_diff;

    while(1) {

        BaseType_t rc = xQueueReceive(q1, &tick_val, portMAX_DELAY);

        tick_diff = tick_val - pre_tick_val;
        xQueueSendToBack(q2, &tick_diff, NULL);

       // ESP_LOGD(tag, "time diff: %d", tick_val-pre_tick_val );
            pre_tick_val = tick_val;

    }

    vTaskDelete(NULL);
}



void ws2812_task(void *pvParameters)
{
    uint8_t anim_max = 1;
    uint16_t step = 0;
    pixel_t pixel_color;



    pixel_color.red = 0;
    pixel_color.green = 0;
    pixel_color.blue = 0;

    for (uint8_t i = 0; i < 7; i++)
     {
         ws2812_set_led_pixel(i, pixel_color);
     }

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
            pixel_color.green = 0;
            pixel_color.blue = 0;
            step = 0;
            break;


        default:
            step = 0;
            break;
        }




        ws2812_set_led_pixel(7, pixel_color);

        ws2812_show();

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}



esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}


void main_display_task(void *pvParameter)
{
    il9341_init();
    il9341_write_fill_rect(0,0,320,240,ILI9341_BLACK);

    uint16_t x = 0;
    uint16_t y = 0;


    static uint32_t tick_rcv;

    cp437(0);
    setTextWrap(1);
    setTextSize(3);

//    setTextColor(ILI9341_ORANGE);
//    setCursor(60, 5);
//    writeText("3.81");


     gfx_write_float(60, 5, ILI9341_ORANGE, FONT_SIZE_3, 123.12354f, DEC_PLACES_2, WITH_SIGN, DIR_FORWARD);


//    setTextColor(ILI9341_GREEN);
//    setCursor(60, 30);
//    writeText("6.35");

    gfx_write_float(60, 30, ILI9341_GREEN, FONT_SIZE_3, -25.3f, DEC_PLACES_2, WITH_SIGN, DIR_FORWARD);


//    setTextColor(ILI9341_BLUE);
//    setCursor(60, 65);
//    writeText("0.21");
    gfx_write_dec(60, 65, ILI9341_BLUE, FONT_SIZE_3, 2365, WITHOUT_SIGN, DIR_FORWARD);


//    setTextColor(ILI9341_GREEN);
//    setCursor(60, 90);
//    writeText("1.05");
  //  gfx_write_dec(60, 90, ILI9341_GREEN, FONT_SIZE_3, -456, WITH_SIGN, DIR_FORWARD);


    setTextColor(ILI9341_RED);
    setCursor(60, 125);
    writeText("+21.3",DIR_FORWARD);

    setTextColor(ILI9341_CYAN);
    setCursor(150, 150);
    writeText("-17.1",DIR_BACKWARD);




    setTextColor(ILI9341_LIGHTGREY);
    setCursor(10, 220);
    writeText("12.04.2017 22:34", DIR_FORWARD);

    //drawRoundRect(2,2, 72, 27,2,ILI9341_LIGHTGREY);



    drawBitmap(0, 5, (uint8_t*) bmp_bulb, 48, 48, ILI9341_WHITE);
    drawBitmap(0, 68, (uint8_t*) bmp_water, 48, 48, ILI9341_WHITE);




    uint8_t xx = 0;
    while (true)
    {


       (void) xQueueReceive(q2, &tick_rcv, 1000);

       gfx_write_dec(150, 90, ILI9341_GREEN, FONT_SIZE_3, tick_rcv, WITHOUT_SIGN, DIR_BACKWARD);

        if(xx)
        {
            drawBitmap(0, 131, (uint8_t*)bmp_temperature, 48, 48, ILI9341_WHITE);
            xx=0;
        }
        else
        {
            drawBitmap(0, 131, (uint8_t*)bmp_temperature, 48, 48, ILI9341_RED);
            xx=1;
        }

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
    xTaskCreate(ws2812_task, "ws2812 rainbow demo", 4096, NULL, 5, NULL);
    xTaskCreate(power_handle_task, "power_handle_task", 4096, NULL, 6, NULL);

}



