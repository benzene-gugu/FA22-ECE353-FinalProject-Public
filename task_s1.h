/*
 * task_s1.h
 *
 *  Created on: 2022Äê12ÔÂ14ÈÕ
 *      Author: Danie
 */

#ifndef TASK_S1_H_
#define TASK_S1_H_

#include "msp.h"
#include "msp432p401r.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include "queue_handles.h"

extern TaskHandle_t Task_S1_Handle;

/**
 * Initialization for Button 1 on MKII: Port 5 Pin 1
 */
void S1_Init(void);

/**
 * Periodic Task for collecting Button S1 input
 */
void Task_S1(void *pvParameters);

#endif /* TASK_S1_H_ */
