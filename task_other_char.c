/*
 * other_char_task.c
 *
 *      Author: Yunzhao Liu
 */
#include "task_other_char.h"

#include "queue_handles.h"

#include "task_display.h"

#include <stdlib.h>

extern QueueHandle_t Queue_Other_Char = NULL;

#define TN 200
#define TMAX 220

static int dt = 20;
static int speed = 15;

void other_char_init(void) {
    Queue_Other_Char = xQueueCreate(10, sizeof(MainInfo));
}

static int posCnt = 3;

static void cal_pos(int t, int* x, int* y, int* scale) {
    int x0, x1, y0, y1;
    if (posCnt == 0) {
        x0 = 68;
        x1 = 105;
        y0 = 67;
        y1 = 130;
    }
    else if (posCnt == 1) {
        x0 = 60;
        x1 = 10;
        y0 = 67;
        y1 = 130;
    }
    else {
        x0 = 65;
        x1 = 58;
        y0 = 67;
        y1 = 130;
    }
    *x = x0 + (x1-x0)*(t*t)/(TN*TN);
    *y = y0 + (y1-y0)*(t*t)/(TN*TN);
    *scale = 1 + 127*(t*t)/(TN*TN);
}

typedef enum {
    OTHER_CHAR_STATE_RIDE,
    OTHER_CHAR_STATE_HURT_LEFT,
    OTHER_CHAR_STATE_HURT_RIGHT,
} OtherCharState;



void Task_other_char(void* pvParam) {
    OtherCharInfo input;

    OtherCharState state = OTHER_CHAR_STATE_RIDE;
    OtherCharState next_state = state;

    int t = 0;

    while(1) {
        xQueueReceive(Queue_Other_Char, &input, portMAX_DELAY);
        if (input.type == OTHER_CHAR_NEXT) {
            state = next_state;
        }
        // state machine
        // STATE RIDE
        if (state == OTHER_CHAR_STATE_RIDE) {
            speed = 15;
            if (input.type == OTHER_CHAR_NEXT) { // next frame
                if (t > TMAX || t < 0) {
                    t = 0;
                    // create a new opponent at random position
                    RenderObject obj = {
                        .id = 1,
                        .img = NULL,
                    };
                    xQueueSend(Queue_Display, &obj, portMAX_DELAY);
                    posCnt = rand()%3;
                }
                else {
                    t += (dt-speed);
                    RenderObject obj = {
                        .id = 1,
                        .img = &imageInfos[IMG_ACCELERATE_3],
                    };
                    cal_pos(t, &obj.posx, &obj.posy, &obj.scale);
                    xQueueSend(Queue_Display, &obj, portMAX_DELAY);

                    // if is close enough in y direction, request to detect the collision
                    if (obj.posy > 120) {
                        MainInfo info;
                        info.type = INFO_OTHER_CHAR_HIT;
                        info.value = obj.posx;
                        xQueueSend(Queue_Main, &info, portMAX_DELAY);
                    }
                }
            }
            else if (input.type == OTHER_CHAR_SET_SPEED) {
                dt = input.value;
            }
            else if ((input.type == OTHER_CHAR_CHECK_HIT_RIGHT || input.type == OTHER_CHAR_CHECK_HIT_LEFT)
                    // ignore this input type if next_state is a hurt state to avoid more than one OTHER_CHAR_HURT to main task
                    && next_state != OTHER_CHAR_STATE_HURT_RIGHT && next_state != OTHER_CHAR_STATE_HURT_LEFT) {
                int x, y, scale;
                cal_pos(t, &x, &y, &scale);
                if (y > 120 && y < 140 && x < input.value + 10 && x > input.value - 10) {
                    MainInfo info;
                    info.type = INFO_OTHER_CHAR_HURT;
                    xQueueSend(Queue_Main, &info, portMAX_DELAY);
                    if (input.type == OTHER_CHAR_CHECK_HIT_RIGHT) {
                        next_state = OTHER_CHAR_STATE_HURT_RIGHT;
                    }
                    else {
                        next_state = OTHER_CHAR_STATE_HURT_LEFT;
                    }

                }
            }
        }
        // STATE HURT
        else if (state == OTHER_CHAR_STATE_HURT_LEFT || state == OTHER_CHAR_STATE_HURT_RIGHT) {
            speed = 0;
            if (input.type == OTHER_CHAR_NEXT) { // next frame
                if (t > TMAX || t < 0) {
                    t = 0;
                    RenderObject obj = {
                        .id = 1,
                        .img = NULL,
                    };
                    xQueueSend(Queue_Display, &obj, portMAX_DELAY);
                    // create a new opponent at random position
                    next_state = OTHER_CHAR_STATE_RIDE;
                    posCnt = rand()%3;
                }
                else {
                    t += (dt-speed);
                    RenderObject obj = {
                        .id = 1,
                        .img = &imageInfos[IMG_HIT_1],
                    };
                    cal_pos(t, &obj.posx, &obj.posy, &obj.scale);
                    if (state == OTHER_CHAR_STATE_HURT_RIGHT) {
                        obj.scale = -obj.scale;
                    }
                    xQueueSend(Queue_Display, &obj, portMAX_DELAY);
                }
            }
        }
    }
}
