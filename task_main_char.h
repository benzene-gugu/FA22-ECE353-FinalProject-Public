/*
 * main_char_task.h
 *
 *  Created on: Dec 5, 2022
 *      Author: liu995
 */

#ifndef MAIN_CHAR_TASK_H
#define MAIN_CHAR_TASK_H

typedef enum {
    MAIN_CHAR_NEXT,
    MAIN_CHAR_TURN,
    MAIN_CHAR_ACC,
    MAIN_CHAR_PUNCH,
    MAIN_CHAR_KICK,
    MAIN_CHAR_CHECK_HIT,
    MAIN_CHAR_WIN,
    MAIN_CHAR_LOSE,
} MainCharInfoType;

typedef struct {
    MainCharInfoType type;
    int value;
} MainCharInfo;

void main_char_init(void);

void Task_main_char(void* pvParam);

#endif

