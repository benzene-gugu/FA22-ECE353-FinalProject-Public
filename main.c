/*
 * Copyright (c) 2016-2019, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== main_freertos.c ========
 */
#include "msp.h"
#include "msp432p401r.h"
#include <stdint.h>
#include <stdio.h>

/* RTOS header files */
#include <FreeRTOS.h>
#include <task.h>
#include <task_buzzer.h>

#include "task_led.h"

#include "task_main_char.h"
#include "task_display.h"
#include "task_main.h"
#include "queue_handles.h"
#include "task_timer.h"
#include "task_tree.h"
#include "task_other_char.h"

#include "task_adc14.h"
#include "task_light.h"
#include "task_s1.h"
#include "task_s2.h"

#include "buzzer.h"

TaskHandle_t Task_Blink_LED1_Handle = NULL;
TaskHandle_t Task_Display_Handle = NULL;
TaskHandle_t Task_Main_Char_Handle = NULL;
TaskHandle_t Task_Main_Handle = NULL;
TaskHandle_t Task_Timer_Handle = NULL;
TaskHandle_t Task_Tree_Handle = NULL;
TaskHandle_t Task_Other_Char_Handle = NULL;
TaskHandle_t Task_song_Handle = NULL;

/* ****************************************************************************
 * This Function initializes the hardware required to blink LED1 on the
 * MSP432 Launchpad
 * ***************************************************************************/


/*
 *  ======== main ========
 */
int main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    blink_led1_hw_init();
    display_init();
    main_char_init();
    main_init();
    tree_task_init();
    other_char_init();
    adc_init();
    i2c_light_init();
    S1_Init();
    S2_Init();
    song_task_init();



    xTaskCreate
    (   Task_main,
        "Main Task",
        configMINIMAL_STACK_SIZE,
        NULL,
        2,
        &Task_Main_Handle
    );

    xTaskCreate
    (   Task_display,
        "Display Task",
        configMINIMAL_STACK_SIZE,
        NULL,
        2,
        &Task_Display_Handle
    );


    xTaskCreate
    (   Task_main_char,
        "main char Task",
        configMINIMAL_STACK_SIZE,
        NULL,
        2,
        &Task_Main_Char_Handle
    );

    xTaskCreate
    (   Task_Blink_LED1,
        "LED1 Blink Task",
        configMINIMAL_STACK_SIZE,
        NULL,
        1,
        &Task_Blink_LED1_Handle
    );

    xTaskCreate
    (   Task_timer,
        "timer Task",
        configMINIMAL_STACK_SIZE,
        NULL,
        2,
        &Task_Timer_Handle
    );

    xTaskCreate
    (   Task_tree,
        "tree Task",
        configMINIMAL_STACK_SIZE,
        NULL,
        2,
        &Task_Tree_Handle
    );

    xTaskCreate
    (   Task_other_char,
        "tree Task",
        configMINIMAL_STACK_SIZE,
        NULL,
        2,
        &Task_Other_Char_Handle
    );

    xTaskCreate
    (   Task_ADC_Main,
        "ADC Main Task",
        configMINIMAL_STACK_SIZE,
        NULL,
        2,
        &Task_ADC_Main_Handle
    );

    xTaskCreate
    (   Task_ADC_Bottom_Half,
        "ADC Handler Task",
        configMINIMAL_STACK_SIZE,
        NULL,
        3,
        &Task_ADC_Bottom_Half_Handle
    );

    xTaskCreate
    (   Task_Light,
        "Light Collection Task",
        configMINIMAL_STACK_SIZE,
        NULL,
        2,
        &Task_Light_Handle
    );

    xTaskCreate
    (   Task_S1,
        "S1 Task",
        configMINIMAL_STACK_SIZE,
        NULL,
        2,
        &Task_S1_Handle
    );

    xTaskCreate
    (   Task_S2,
        "S2 Task",
        configMINIMAL_STACK_SIZE,
        NULL,
        2,
        &Task_S2_Handle
    );

    xTaskCreate
    (   start_song_task,
        "buzzer Task",
        configMINIMAL_STACK_SIZE,
        NULL,
        2,
        &Task_song_Handle
    );


    /* Start the FreeRTOS scheduler */
    vTaskStartScheduler();

    while(1){};
    return (0);
}

//*****************************************************************************
//
//! \brief Application defined malloc failed hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationMallocFailedHook()
{
    /* Handle Memory Allocation Errors */
    while(1)
    {
    }
}

//*****************************************************************************
//
//! \brief Application defined stack overflow hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    //Handle FreeRTOS Stack Overflow
    while(1)
    {
    }
}
