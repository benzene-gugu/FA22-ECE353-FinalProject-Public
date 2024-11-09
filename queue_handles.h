/*
 * queue_handles.h
 *
 *  Created on: Dec 5, 2022
 *      Author: Yunzhao Liu
 */

#ifndef QUEUE_HANDLES_H_
#define QUEUE_HANDLES_H_

#include <FreeRTOS.h>
#include <queue.h>



// output of all Char
extern QueueHandle_t Queue_Display;
extern QueueHandle_t Queue_Buzz;

// output of main SM
extern QueueHandle_t Queue_Main_Char;
extern QueueHandle_t Queue_Other_Char;
extern QueueHandle_t Queue_Tree;

// input of main SM
extern QueueHandle_t Queue_Main;



typedef enum {
    INFO_JOYSTICK,
    INFO_BUTTON_1,
    INFO_BUTTON_2,
    INFO_ACCELEROMETER,
    INFO_AMBIENT_LIGHT,
    INFO_TIMER_100MS,
    INFO_TIMER_50MS,
    INFO_SET_SPEED,
    INFO_MAIN_CHAR_HIT_LEFT, // value is the x position of the hit box
    INFO_MAIN_CHAR_HIT_RIGHT,
    INFO_MAIN_CHAR_HURT, // indicate main character is hurt by other characters
    INFO_OTHER_CHAR_HIT, // value is the x position of the hit box
    INFO_OTHER_CHAR_HURT,
    INFO_GAME_OVER,
} MainInfoType;

typedef struct {
    MainInfoType type;
    int value;
} MainInfo;



#endif /* QUEUE_HANDLES_H_ */
