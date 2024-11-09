/*
 * task_s1.c
 *
 *  Created on: 2022Äê12ÔÂ14ÈÕ
 *      Author: Danie
 */

#include "task_s2.h"

TaskHandle_t Task_S1_Handle;

/**
 * Initialization for Button 1 on MKII: Port 5 Pin 1
 */
void S1_Init(void) {
    P5->DIR &= ~BIT1; // Set Port 5 Pin 1 to be of input type
    P5->REN &= ~BIT1; // Disable the internal resistors since there is an external pull-up resistor
}

/**
 * Returns the state for Button 1 on MKII
 * returns: true if button is pressed, false otherwise
 */
bool S1(void) {
    return (P5->IN&BIT1)==0;
}

/*
 * De-bouncing switch S1
 */
bool debounce_S1(void) {
    static uint8_t debounce_state = 0x00;

    // shift the de-bounce variable to the left
    debounce_state = debounce_state << 1;

    // If S1 is being pressed, set the LSB of debounce state to 1
    if(S1()) {
        debounce_state |= 0x01;
    }

    if(debounce_state == 0x7F) {
        return true;
    }
    else {
        return false;
    }
}

/**
 * Periodic Task for collecting Button S1 input
 */
void Task_S1(void *pvParameters) {
    MainInfo s1Info;
    while(1) {
        s1Info.type = INFO_BUTTON_1;
        if(debounce_S1()) {
            s1Info.value = 1;
            xQueueSend(Queue_Main,&s1Info,portMAX_DELAY);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
