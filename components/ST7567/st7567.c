#include "st7567.h"

#include <stdio.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "ST7657";


// defines

#define DISP_MOSI   GPIO_NUM_16
#define DISP_SCK    GPIO_NUM_4
#define DISP_DC     GPIO_NUM_21
#define DISP_CS     GPIO_NUM_22

#define LCD_CMD()     gpio_set_level(DISP_DC, 0)
#define LCD_DATA()    gpio_set_level(DISP_DC, 1)

#define DISP_DESELECT()         gpio_set_level(DISP_CS, 1)
#define DISP_SELECT()           gpio_set_level(DISP_CS, 0)

// local functions 
esp_err_t setupGPIO(void);
void sendSPIByte(uint8_t sbyte);
void dispSendCommand(uint8_t cmd);
void dispSendData(uint8_t data);



esp_err_t initGlcd(void)
{
    setupGPIO();
    // start the magic 
    DISP_SELECT();
    
    dispSendCommand(0xE2);
    vTaskDelay(5 / portTICK_PERIOD_MS);

    //dispSendCommand(0xE3);
    //vTaskDelay(5 / portTICK_PERIOD_MS);


    dispSendCommand(0xA3);
    dispSendCommand(0xA0);
    dispSendCommand(0xC0);
    dispSendCommand(0x24);
    dispSendCommand(0x81);
    dispSendCommand(0x22);

    
    dispSendCommand(0x2C);
    //give some delay
    vTaskDelay(50 / portTICK_PERIOD_MS);
    
    dispSendCommand(0x2E);
    vTaskDelay(50 / portTICK_PERIOD_MS);

    dispSendCommand(0x2F);
    vTaskDelay(50 / portTICK_PERIOD_MS);


    DISP_DESELECT();


    vTaskDelay(1000 / portTICK_PERIOD_MS);

    DISP_SELECT();

    dispSendCommand(0xA5);
    dispSendCommand(0xAF);

    DISP_DESELECT();

    vTaskDelay(50 / portTICK_PERIOD_MS);
    
    DISP_SELECT();
    dispSendCommand(0xFE);
    DISP_DESELECT();

    return ESP_OK;
}




// internal functions 
esp_err_t setupGPIO(void)
{
   
    gpio_config_t gpioConfig = 
    {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = ((1ULL << DISP_MOSI) | (1ULL << DISP_SCK) | (1ULL << DISP_CS) | (1ULL << DISP_DC)),
        .pull_up_en = GPIO_PULLDOWN_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE
    };

    return gpio_config(&gpioConfig);
}

void sendSPIByte(uint8_t sbyte)
{
    uint8_t bit;
    for(bit = 0x80; bit; bit >>= 1)
    {
        if(sbyte & bit)
        {
            gpio_set_level(DISP_MOSI, 1);
        }
        else 
        {
            gpio_set_level(DISP_MOSI, 0);
        }

        gpio_set_level(DISP_SCK, 1);
        vTaskDelay(1 / portTICK_PERIOD_MS);
        gpio_set_level(DISP_SCK, 0);
    }
}

void dispSendCommand(uint8_t cmd)
{
    LCD_CMD();
    sendSPIByte(cmd);
}

void dispSendData(uint8_t data)
{
    LCD_DATA();
    sendSPIByte(data);
}