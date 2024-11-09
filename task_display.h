/*
 * task_display.h
 *
 *      Author: Yunzhao Liu
 */
#ifndef DISPLAY_TASK_H_
#define DISPLAY_TASK_H_

#include <stdbool.h>
#include <stdint.h>
#include "image.h"

#define MAX_RENDER_ID 6
// send a RenderObject with id DISPLAY_INVERSE_ID will set the display color to inverted color
#define DISPLAY_INVERSE_ID (-1)
// send a RenderObject with id DISPLAY_NORMAL_ID will set the display color to original color
#define DISPLAY_NORMAL_ID (-2)

typedef struct {
    int id;
	int posx, posy;
	int scale; // x flip if negative. Same size if 128
	const ImageInfo* img;
} RenderObject;

void draw_obj(const RenderObject* obj);

void Task_display(void* pvParameter);

void display_init(void);

#endif /* DISPLAY_TASK_H_ */
