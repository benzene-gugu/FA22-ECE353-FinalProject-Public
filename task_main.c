/*
 * main_task.c
 *
 *  Created on: Dec 7, 2022
 *      Author: Yunzhao Liu
 */
#include "FreeRTOS.h"
#include "queue.h"

#include "task_main.h"

#include "task_main_char.h"

#include "queue_handles.h"
#include "task_tree.h"
#include "task_other_char.h"
#include "task_display.h"

#include "task_adc14.h"
#include "task_light.h"
#include "task_s1.h"
#include "task_s2.h"

#define WIN_SCORE 4

extern QueueHandle_t Queue_Main = NULL;

void main_init(void) {
    Queue_Main = xQueueCreate(60, sizeof(MainInfo));
}

void Task_main(void* pvParameter) {
    // score +1 iff kick or punch an opponent
    int score = 0;
    // record time after game over: time = gameOverCnt x 100ms
    int gameOverCnt = 0;

    int joystickDir = JOYSTICK_CENTER;
    while(1) {

        MainInfo input;
        BaseType_t status;
        status=xQueueReceive(Queue_Main, &input, portMAX_DELAY);

        if (input.type == INFO_TIMER_100MS) {
            MainCharInfo info;
            // win condition
            if (score >= WIN_SCORE) {
                gameOverCnt++;
                if (gameOverCnt == 8) { // wait for 800ms then loop forever
                    info.type = MAIN_CHAR_WIN;
                    xQueueSend(Queue_Main_Char, &info, portMAX_DELAY);
                }
            }
            // lose condition
            else if (score < 0) {
                gameOverCnt++;
                info.type = MAIN_CHAR_LOSE;
                xQueueSend(Queue_Main_Char, &info, portMAX_DELAY);
            }

            // MAIN_CHAR_NEXT every 50ms
            info.type = MAIN_CHAR_NEXT;
            xQueueSend(Queue_Main_Char, &info, portMAX_DELAY);

            // OTHER_CHAR_NEXT every 100ms
            if (score < WIN_SCORE || gameOverCnt < 7) {
                OtherCharInfo other_info;
                other_info.type = OTHER_CHAR_NEXT;
                xQueueSend(Queue_Other_Char, &other_info, portMAX_DELAY);
            }
        }
        else if (input.type == INFO_TIMER_50MS) {
            // MAIN_CHAR_NEXT every 50ms
            MainCharInfo info;
            info.type = MAIN_CHAR_NEXT;
            xQueueSend(Queue_Main_Char, &info, portMAX_DELAY);

            // TREE_INFO_NEXT every 100ms
            TreeInfo tree_info;
            tree_info.type = TREE_INFO_NEXT;
            xQueueSend(Queue_Tree, &tree_info, portMAX_DELAY);
        }
        else if (input.type == INFO_SET_SPEED) {
            // update speed for tree and opponent
            TreeInfo tree_info;
            tree_info.type = TREE_INFO_SET_SPEED;
            tree_info.value = input.value;
            xQueueSend(Queue_Tree, &tree_info, portMAX_DELAY);

            OtherCharInfo other_info;
            other_info.type = OTHER_CHAR_SET_SPEED;
            other_info.value = input.value;
            xQueueSend(Queue_Other_Char, &other_info, portMAX_DELAY);
        }
        else if (input.type == INFO_AMBIENT_LIGHT) {
            // change color base on ambient light
            RenderObject obj;
            if (input.value == LIGHT_BRIGHT) {
                obj.id = DISPLAY_NORMAL_ID;
            }
            else if (input.value == LIGHT_DARK) {
                obj.id = DISPLAY_INVERSE_ID;
            }
            xQueueSend(Queue_Display, &obj, portMAX_DELAY);
        }
        else if (score > WIN_SCORE || score < 0) {
            continue; // ignore input below if game over
        }
        else if (input.type == INFO_JOYSTICK) {
            MainCharInfo info;
            joystickDir = input.value;
            if (input.value == JOYSTICK_UP) {
                info.type = MAIN_CHAR_ACC;
                info.value = 1;
                xQueueSend(Queue_Main_Char, &info, portMAX_DELAY);
            }
            else {
                info.type = MAIN_CHAR_ACC;
                info.value = 0;
                xQueueSend(Queue_Main_Char, &info, portMAX_DELAY);
            }
        }
        else if (input.type == INFO_BUTTON_1) {
            // Punch
            MainCharInfo info;
            info.type = MAIN_CHAR_PUNCH;
            if (joystickDir == JOYSTICK_RIGHT) {
                info.value = 1;
            }
            else if (joystickDir == JOYSTICK_LEFT) {
                info.value = -1;
            }
            xQueueSend(Queue_Main_Char, &info, portMAX_DELAY);
        }
        else if (input.type == INFO_BUTTON_2) {
            // Kick
            MainCharInfo info;
            info.type = MAIN_CHAR_KICK;
            if (joystickDir == JOYSTICK_RIGHT) {
                info.value = 1;
            }
            else if (joystickDir == JOYSTICK_LEFT) {
                info.value = -1;
            }
            xQueueSend(Queue_Main_Char, &info, portMAX_DELAY);
        }
        else if (input.type == INFO_ACCELEROMETER) {
            // use accelerometer to change the position of the main character.
            MainCharInfo info;
            info.type = MAIN_CHAR_TURN;
            if (input.value == ACCEL_LEFT) {
                info.value = -1;
            }
            else if (input.value == ACCEL_RIGHT) {
                info.value = 1;
            }
            else {
                info.value = 0;
            }
            xQueueSend(Queue_Main_Char, &info, portMAX_DELAY);
        }
        else if (input.type == INFO_MAIN_CHAR_HIT_LEFT) {
            OtherCharInfo other_info;
            other_info.type = OTHER_CHAR_CHECK_HIT_LEFT;
            other_info.value = input.value;
            xQueueSend(Queue_Other_Char, &other_info, portMAX_DELAY);
        }
        else if (input.type == INFO_MAIN_CHAR_HIT_RIGHT) {
            OtherCharInfo other_info;
            other_info.type = OTHER_CHAR_CHECK_HIT_RIGHT;
            other_info.value = input.value;
            xQueueSend(Queue_Other_Char, &other_info, portMAX_DELAY);
        }
        else if (input.type == INFO_OTHER_CHAR_HIT) {
            MainCharInfo info;
            info.type = MAIN_CHAR_CHECK_HIT;
            info.value = input.value;
            xQueueSend(Queue_Main_Char, &info, portMAX_DELAY);
        }
        else if (input.type == INFO_OTHER_CHAR_HURT) {
            score += 1;
        }
        else if (input.type == INFO_MAIN_CHAR_HURT) {
            score = -1000;
        }
    }
}


