/*
 * task_led.c
 *
 *  Created on: Dec 14, 2022
 *      Author: Yunzhao Liu
 */
#include "task_led.h"

#include "msp.h"
#include "msp432p401r.h"
#include <stdint.h>
#include <stdio.h>

/* RTOS header files */
#include <FreeRTOS.h>
#include <task.h>

void blink_led1_hw_init(void)
{
    // set direction as an output
    P1->DIR |= BIT0;

    // Turn off LED
    P1->OUT &= ~BIT0;
}

/******************************************************************************
* Tasked used to blink LED1 on MSP432 Launchpad
******************************************************************************/
void Task_Blink_LED1(void *pvParameters)
{
    int i;
    while(1)
    {
        // turn on the LED
        P1->OUT |= BIT0;

        // Delay
        vTaskDelay(pdMS_TO_TICKS(500));

        // turn off the LED
        P1->OUT &= ~BIT0;

        // Delay
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
