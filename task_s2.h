/*
 * task_s2.h
 *
 *  Created on: 2022Äê12ÔÂ14ÈÕ
 *      Author: Danie
 */

#ifndef TASK_S2_H_
#define TASK_S2_H_

#include "msp.h"
#include "msp432p401r.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include "queue_handles.h"


extern TaskHandle_t Task_S2_Handle;

/**
 * Initialization for Button 2 on MKII: Port 3 Pin 5
 */
void S2_Init(void);

/**
 * Periodic Task for collecting Button S2 input
 */
void Task_S2(void *pvParameters);

#endif /* TASK_S2_H_ */
