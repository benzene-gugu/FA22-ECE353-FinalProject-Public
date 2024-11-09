/*
 * tree_task.c
 *
 *      Author: Yunzhao Liu
 */

#include "task_tree.h"

#include "queue_handles.h"
#include "task_display.h"

extern QueueHandle_t Queue_Tree = NULL;

// the unint of the time
#define TN 200
// the maximum time a tree can exist on screen
#define TMAX 220

// movement speed
static int dt = 20;

void tree_task_init(void){
    Queue_Tree = xQueueCreate(10, sizeof(TreeInfo));
}

// calculate position and scale of the tree on the left
static void cal_pos_left(int t, int* x, int* y, int* scale) {
    // 54 64
    // 0 90
    *x = 54 - 54*(t*t) / (TN*TN);
    *y = 64 + (90-64)*(t*t) / (TN*TN);
    *scale = 1 + 100*(t*t) / (TN*TN);
}

// calculate position and scale of the tree on the right
static void cal_pos_right(int t, int* x, int* y, int* scale) {
    // 72 67
    // 130 94
    *x = 72 + (130-72)*(t*t)/(TN*TN);
    *y = 67 + (94-67)*(t*t)/(TN*TN);
    *scale = 1 + 100*(t*t)/(TN*TN);
}

// draw the tree on the left
static void draw_tree_left(int* t, int id) {
    if (*t > TMAX) {
        *t = *t % TMAX;
        RenderObject obj = {
            .id = id,
            .img = NULL,
        };
        xQueueSend(Queue_Display, &obj, portMAX_DELAY);
    }
    else {
        *t += dt;
        RenderObject obj = {
            .id = id,
            .img = &imageInfos[IMG_TREE],
        };
        cal_pos_left(*t, &obj.posx, &obj.posy, &obj.scale);
        xQueueSend(Queue_Display, &obj, portMAX_DELAY);
    }
}

// draw the tree on the right
static void draw_tree_right(int* t, int id) {
    if (*t > TMAX) {
        *t = *t % TMAX;
        RenderObject obj = {
            .id = id,
            .img = NULL,
        };
        xQueueSend(Queue_Display, &obj, portMAX_DELAY);
    }
    else {
        *t += dt;
        RenderObject obj = {
            .id = id,
            .img = &imageInfos[IMG_TREE],
        };
        cal_pos_right(*t, &obj.posx, &obj.posy, &obj.scale);
        xQueueSend(Queue_Display, &obj, portMAX_DELAY);
    }
}

void Task_tree(void* pvParam) {
    TreeInfo input;

    int t2 = 0;
    int t4 = TN/2;
    while(1) {
        xQueueReceive(Queue_Tree, &input, portMAX_DELAY);

        if (input.type == TREE_INFO_NEXT) {
            //draw_tree_right(&t1, 2);
            draw_tree_right(&t2, 3);
            //draw_tree_left(&t3, 4);
            draw_tree_left(&t4, 5);
        }
        else if (input.type == TREE_INFO_SET_SPEED) {
            dt = input.value;
        }

    }
}



