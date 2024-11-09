/*
 * task_adc14.h
 *
 *  Created on: 12/10/2022
 *      Author: Daniel Zhaos
 */

#ifndef TASK_ADC14_H_
#define TASK_ADC14_H_

#include "msp.h"
#include "msp432p401r.h"
#include <stdint.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include "queue_handles.h"
#include <math.h>

#define JOYSTICK_X_PORT P6
#define JOYSTICK_X_PIN  BIT0
#define JOYSTICK_X_AC   ADC14_MCTLN_INCH_15
#define JOYSTICK_Y_PORT P4
#define JOYSTICK_Y_PIN  BIT4
#define JOYSTICK_Y_AC   ADC14_MCTLN_INCH_9
#define ACCEL_X_PORT    P6
#define ACCEL_X_PIN     BIT1
#define ACCEL_X_AC      ADC14_MCTLN_INCH_14
#define ACCEL_Y_PORT    P4
#define ACCEL_Y_PIN     BIT0
#define ACCEL_Y_AC      ADC14_MCTLN_INCH_13
#define ACCEL_Z_PORT    P4
#define ACCEL_Z_PIN     BIT2
#define ACCEL_Z_AC      ADC14_MCTLN_INCH_11
#define VOLT_0P85       1056      // 0.85 /(3.3/4096)
#define VOLT_2P50       3103      // 2.50 /(3.3/4096)
#define PI              3.14159265f

extern TaskHandle_t Task_ADC_Handle;
extern TaskHandle_t Task_ADC_Bottom_Half_Handle;
extern TaskHandle_t Task_ADC_Main_Handle;

typedef enum {
    JOYSTICK_CENTER = 0,
    JOYSTICK_LEFT = -1,
    JOYSTICK_RIGHT = 1,
    JOYSTICK_UP = 10,
    JOYSTICK_DOWN = -10,
} JOYSTICK_DIR;

typedef enum {
    ACCEL_RIGHT = 1,
    ACCEL_LEFT = -1,
    ACCEL_CENTER = 0,
} ACCEL_DIR;

/**
 * Initializes the configuration of both the joystick and the accelerometer
 */
void adc_init(void);

/**
 * Main Task to start the ADC conversion
 */
void Task_ADC_Main(void *pvParameters);

/**
 * Bottom Half of the ISR for data conversion
 */
void Task_ADC_Bottom_Half(void *pvParameters);

#endif /* TASK_ADC14_H_ */
