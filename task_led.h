/*
 * task_led.h
 *
 *  Created on: Dec 14, 2022
 *      Author: Yunzhao Liu
 */

#ifndef TASK_LED_H_
#define TASK_LED_H_

void blink_led1_hw_init(void);
/******************************************************************************
* Tasked used to blink LED1 on MSP432 Launchpad
******************************************************************************/
void Task_Blink_LED1(void *pvParameters);

#endif /* TASK_LED_H_ */
