/*
 * task_light.c
 *
 *  Created on: 12/05/2022
 *      Author: Daniel Zhao
 */

#include "task_light.h"

TaskHandle_t Task_Light_Handle;

/**
 * Initialize the light sensor.
 */
void i2c_light_init(void) {
    i2c_init();
    i2c_write_16(I2C_LIGHT_ADDR, I2C_LIGHT_REG_CONFIG, I2C_LIGHT_CONFIG);
    i2c_write_16(I2C_LIGHT_ADDR, I2C_LIGHT_REG_LOWL, I2C_LIGHT_LOWLIMIT);
    i2c_write_16(I2C_LIGHT_ADDR, I2C_LIGHT_REG_HIGHL, I2C_LIGHT_HIGHLIMIT);
}

/**
 * Read the value from the light sensor.
 */
float i2c_light_read(void) {
    uint16_t raw_data;
    raw_data = i2c_read_16(I2C_LIGHT_ADDR, I2C_LIGHT_REG_RESULT);
    float lux_value = 0.01 * (1 << (raw_data >> 12)) * (raw_data & 0x0FFF);
    return lux_value;
}

/**
 * Task used for periodic data collection
 */
void Task_Light(void *pvParameters) {
    MainInfo LightInfo;
    LIGHT_LEVEL last = LIGHT_BRIGHT;
    while(1) {
        LightInfo.type = INFO_AMBIENT_LIGHT;
        float lux = i2c_light_read();
        if(lux > 20) {
            LightInfo.value = LIGHT_BRIGHT;
        }
        else if(lux < 10) {
            LightInfo.value = LIGHT_DARK;
        }
        if(LightInfo.value != last) {
            xQueueSend(Queue_Main,&LightInfo,portMAX_DELAY);
        }
        last = LightInfo.value;
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}


