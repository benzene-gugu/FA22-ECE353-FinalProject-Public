/*
 * task_adc14.c
 *
 *  Created on: 12/10/2022
 *      Author: Daniel Zhao
 */

#include "task_adc14.h"

TaskHandle_t Task_ADC_Bottom_Half_Handle;
TaskHandle_t Task_ADC_Main_Handle;

volatile uint32_t JOYSTICK_X_DIR = 0;
volatile uint32_t JOYSTICK_Y_DIR = 0;
volatile uint32_t ACCEL_X_DIR = 0;
volatile uint32_t ACCEL_Y_DIR = 0;
volatile uint32_t ACCEL_Z_DIR = 0;

/**
 * Initializes the configuration of both the joystick and the accelerometer
 */
void adc_init(void) {
    // Configure the ADC14 Control Registers
    ADC14->CTL0 = ADC14_CTL0_SHP |    // Use the ADC timer to control pulse samples
                  ADC14_CTL0_SHT02|   // Sample time of 16 ADC cycles for the first 8 analog channels
                  ADC14_CTL0_CONSEQ_1; // Indicate that ADC should convert a sequence of channels

    // Configure the converter result to be of 12 bits
    ADC14->CTL1 = ADC14_CTL1_RES_2;

    // Configure Port 6 Pin 0 as the analog input pin A15 (X direction)
    JOYSTICK_X_PORT->SEL0 |= JOYSTICK_X_PIN;
    JOYSTICK_X_PORT->SEL1 |= JOYSTICK_X_PIN;

    // Configure Port 4 Pin 4 as the analog input pin A9 (Y direction)
    JOYSTICK_Y_PORT->SEL0 |= JOYSTICK_Y_PIN;
    JOYSTICK_Y_PORT->SEL1 |= JOYSTICK_Y_PIN;

    // Configure the memory control register to associate analog input A15 with MEM[0]
    ADC14->MCTL[0] = JOYSTICK_X_AC;

    // Configure the memory control register to associate analog input A9 with MEM[1]
    ADC14->MCTL[1] = JOYSTICK_Y_AC;

    // Configure Port 6 Pin 1 as the analog input pin A14 (X direction)
    ACCEL_X_PORT->SEL0 |= ACCEL_X_PIN;
    ACCEL_X_PORT->SEL1 |= ACCEL_X_PIN;

    // Configure Port 4 Pin 0 as the analog input pin A13 (Y direction)
    ACCEL_Y_PORT->SEL0 |= ACCEL_Y_PIN;
    ACCEL_Y_PORT->SEL1 |= ACCEL_Y_PIN;

    // Configure Port 4 Pin 2 as the analog input pin A11 (Z direction)
    ACCEL_Z_PORT->SEL0 |= ACCEL_Z_PIN;
    ACCEL_Z_PORT->SEL1 |= ACCEL_Z_PIN;

    // Configure the memory control register to associate analog input A14 with MEM[0]
    ADC14->MCTL[2] = ACCEL_X_AC;

    // Configure the memory control register to associate analog input A13 with MEM[1]
    ADC14->MCTL[3] = ACCEL_Y_AC;

    // Configure the memory control register to associate analog input A11 with MEM[1]
    ADC14->MCTL[4] = ACCEL_Z_AC;

    // Indicates that MEM[4] should be the end of the converting sequence
    ADC14->MCTL[4] |= ADC14_MCTLN_EOS;

    // Enable Interrupts when channel 4 completes conversion
    ADC14->IER0 = ADC14_IER0_IE4;

    // Enable ADC Interrupt
    NVIC_EnableIRQ(ADC14_IRQn);
    NVIC_SetPriority(ADC14_IRQn,2);

    // Turn on ADC
    ADC14->CTL0 |= ADC14_CTL0_ON;
}

/**
 * Main Task to start the ADC conversion
 */
void Task_ADC_Main(void *pvParameters) {
    while(1) {
        // Start the ADC Conversion
        ADC14->CTL0 |= ADC14_CTL0_SC | ADC14_CTL0_ENC;

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/**
 * Bottom Half of the ISR for data conversion
 */
void Task_ADC_Bottom_Half(void *pvParameters) {
    MainInfo joystickInfo,accelInfo;
    JOYSTICK_DIR last_joystick = JOYSTICK_CENTER;
    ACCEL_DIR last_accel = ACCEL_CENTER;
    float xv;

    while(1) {
        // Wait until notification
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        joystickInfo.type = INFO_JOYSTICK;
        if(JOYSTICK_X_DIR > VOLT_2P50) {
            joystickInfo.value = JOYSTICK_RIGHT;
        }
        else if(JOYSTICK_X_DIR < VOLT_0P85) {
            joystickInfo.value = JOYSTICK_LEFT;
        }
        else if(JOYSTICK_Y_DIR > VOLT_2P50){
            joystickInfo.value = JOYSTICK_UP;
        }
        else if(JOYSTICK_Y_DIR < VOLT_0P85) {
            joystickInfo.value = JOYSTICK_DOWN;
        }
        else {
            joystickInfo.value = JOYSTICK_CENTER;
        }


        if(joystickInfo.value != last_joystick) {
            xQueueSend(Queue_Main,&joystickInfo,portMAX_DELAY);
        }

        last_joystick = joystickInfo.value;

        accelInfo.type = INFO_ACCELEROMETER;
        xv = (ACCEL_X_DIR / 4096.0 * 3.3 - 1.65) / 0.660;

        if(xv > 0.4) {
            accelInfo.value = ACCEL_RIGHT;
        }
        else if(xv < -0.4) {
            accelInfo.value = ACCEL_LEFT;
        }
        else {
            accelInfo.value = ACCEL_CENTER;
        }

        if(last_accel != accelInfo.value) {
            xQueueSend(Queue_Main,&accelInfo,portMAX_DELAY);
        }

        last_accel = accelInfo.value;
    }
}

/**
 * Top Half of the ADC Handler for both bottom half tasks
 */
void ADC14_IRQHandler(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Read the X and Y values from the ADC due to the joystick
    JOYSTICK_X_DIR = ADC14->MEM[0];
    JOYSTICK_Y_DIR = ADC14->MEM[1];

    // Read the X, Y, and Z values from the ADC due to the accelerometer
    ACCEL_X_DIR = ADC14->MEM[2];
    ACCEL_Y_DIR = ADC14->MEM[3];
    ACCEL_Z_DIR = ADC14->MEM[4];

    // Notify the bottom half of the ISR
    vTaskNotifyGiveFromISR(Task_ADC_Bottom_Half_Handle,&xHigherPriorityTaskWoken);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
