/*
 * task_light.h
 *
 *  Created on: 12/05/2022
 *      Author: Daniel Zhao
 */

#ifndef TASK_LIGHT_H_
#define TASK_LIGHT_H_

#include "msp.h"
#include "msp432p401r.h"
#include <stdint.h>
#include <stdio.h>
#include "i2c.h"
#include <FreeRTOS.h>
#include <task.h>
#include "queue_handles.h"

#define I2C_LIGHT_ADDR        0x44

#define I2C_LIGHT_REG_RESULT  0x00
#define I2C_LIGHT_REG_CONFIG  0x01
#define I2C_LIGHT_REG_LOWL    0x02
#define I2C_LIGHT_REG_HIGHL   0x03

#define I2C_LIGHT_CONFIG      0x6410
#define I2C_LIGHT_LOWLIMIT    0x39C4
#define I2C_LIGHT_HIGHLIMIT   0x49C4

extern TaskHandle_t Task_Light_Handle;

typedef enum {
    LIGHT_BRIGHT = 1,
    LIGHT_DARK = -1,
} LIGHT_LEVEL;

/**
 * Initialize the light sensor.
 */
void i2c_light_init(void);

/**
 * Read the value from the light sensor.
 */
float i2c_light_read(void);

/**
 * Bottom Half of the ISR for data collection
 */
void Task_Light(void *pvParameters);

#endif /* TASK_LIGHT_H_ */
