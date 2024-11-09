/*
 * task_s2.c
 *
 *  Created on: 2022Äê12ÔÂ14ÈÕ
 *      Author: Danie
 */

#include "task_s2.h"

TaskHandle_t Task_S2_Handle;

/**
 * Initialization for Button 2 on MKII: Port 3 Pin 5
 */
void S2_Init(void) {
    P3->DIR &= ~BIT5; // Set Port 3 Pin 5 to be of input type
    P3->REN &= ~BIT5; // Disable the internal resistors since there is an external pull-up resistor
}

/**
 * Returns the state for Button 2 on MKII
 * returns: true if button is pressed, false otherwise
 */
bool S2(void) {
    return (P3->IN&BIT5)==0;
}

/*
 * De-bouncing switch S2
 */
bool debounce_S2(void) {
    static uint8_t debounce_state = 0x00;

    // shift the de-bounce variable to the left
    debounce_state = debounce_state << 1;

    // If S2 is being pressed, set the LSB of debounce state to 1
    if(S2()) {
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
 * Periodic Task for collecting Button S2 input
 */
void Task_S2(void *pvParameters) {
    MainInfo s2Info;
    while(1) {
        s2Info.type = INFO_BUTTON_2;
        if(debounce_S2()) {
            s2Info.value = 1;
            xQueueSend(Queue_Main,&s2Info,portMAX_DELAY);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
