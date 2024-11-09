/*
 * task_display.c
 *
 *      Author: Yunzhao Liu
 */
#include "task_display.h"
#include "queue_handles.h"
#include "lcd.h"

#include <queue.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>


extern QueueHandle_t Queue_Display = NULL;

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b));

typedef struct {
	int x0, y0, x1, y1;
	int scale, img_width;
	bool flip;
	const uint16_t* img;
} RenderBox;

RenderBox render_boxes[MAX_RENDER_ID+1] = {0};
bool colorInvert = false;

// initialize display task
void display_init(void) {
    lcd_init();
    Queue_Display = xQueueCreate(10, sizeof(RenderObject));
}

// draw the pixel of x y when only consider RenderBox box.
// Return true if a pixel is drawn.
bool draw_pixel(const RenderBox* box, int x, int y) {
    // return false if no image to draw
    if (box->img == NULL) {
        return false;
    }

    // return false if current position is outside of the box
	if (x < box->x0 || x > box->x1 || y < box->y0 || y > box->y1) {
		return false;
	}

	// scale the image
	const int draw_x = (box->flip ? (box->x1-x) : (x-box->x0)) * 128 / box->scale;
	const int draw_y = (y-box->y0) * 128 / box->scale;
	const int indx = draw_y * box->img_width + draw_x;
	uint16_t data = box->img[indx];
	// if current pixel is transparent
	if (data == 0xa254) {
		return false;
	}

	if (colorInvert) data = ~data;
    HAL_LCD_writeData(data >> 8);
    HAL_LCD_writeData(data);

	return true;
}

// draw the pixel at x y. Consider all the RenderBox value.
void draw_multilayer_pixel(int x, int y) {
    // y sort for id 0 (player) and id 1 (opponent)
    if (render_boxes[1].y1 > render_boxes[0].y1) {
        // try to draw 1 or 0
        if (draw_pixel(&render_boxes[1], x, y) || draw_pixel(&render_boxes[0], x, y)) {
            return;
        }
    }
    // try to draw 0 or 1
    else if (draw_pixel(&render_boxes[0], x, y) || draw_pixel(&render_boxes[1], x, y)){
        return;
    }

    // draw other layer in order.
    int i;
    for (i = 2; i <= MAX_RENDER_ID; i++) {
        if (draw_pixel(&render_boxes[i], x, y)) {
            return;
        }
    }

    // otherwise draw the background
    uint16_t bColor = img_background[x+y*130];
    if (colorInvert) bColor = ~bColor;
    HAL_LCD_writeData(bColor >> 8);
    HAL_LCD_writeData(bColor);

}

// Draw the obj. Remove the old obj with same id.
void draw_obj(const RenderObject* obj) {
	RenderBox box;
	int id = obj->id;
	// calculate the info of the new box
	if (obj->img != NULL) {
        box.img = obj->img->data;
        box.img_width = obj->img->width;
        if (obj->scale < 0) {
            box.scale = -obj->scale;
            box.flip = true;
            box.x1 = obj->posx + obj->img->xoffset * box.scale / 128;
            box.x0 = box.x1 - obj->img->width * box.scale / 128 + 1;
        }
        else {
            box.scale = obj->scale;
            box.flip = false;
            box.x0 = obj->posx - obj->img->xoffset * box.scale / 128;
            box.x1 = box.x0 + obj->img->width * box.scale / 128 - 1;
        }
        box.y0 = obj->posy - obj->img->yoffset * box.scale / 128;
        box.y1 = box.y0 + obj->img->height * box.scale / 128 - 1;
	}


	int draw_x0, draw_y0, draw_x1, draw_y1;
	if (render_boxes[id].img == NULL) {
	    if (obj->img == NULL) { // nop
	        return;
	    }
	    else { // draw new image
	        draw_x0 = box.x0;
	        draw_y0 = box.y0;
	        draw_x1 = box.x1;
	        draw_y1 = box.y1;
	        render_boxes[id] = box;
	    }
	}
	else if (obj->img == NULL) { // erase old image
	    draw_x0 = render_boxes[id].x0;
	    draw_y0 = render_boxes[id].y0;
	    draw_x1 = render_boxes[id].x1;
	    draw_y1 = render_boxes[id].y1;
	    render_boxes[id].img = NULL;
	}
	else { // erase old image and then draw new image
	    draw_x0 = MIN(box.x0, render_boxes[id].x0);
	    draw_y0 = MIN(box.y0, render_boxes[id].y0);
	    draw_x1 = MAX(box.x1, render_boxes[id].x1);
	    draw_y1 = MAX(box.y1, render_boxes[id].y1);
	    render_boxes[id] = box;
	}

	// clamp to screen
	draw_x0 = MAX(draw_x0, 0);
	draw_y0 = MAX(draw_y0, 0);
	draw_x1 = MIN(draw_x1, 129);
	draw_y1 = MIN(draw_y1, 129);

	lcd_set_draw_frame(draw_x0, draw_y0, draw_x1, draw_y1);
	HAL_LCD_writeCommand(CM_RAMWR);

	// draw all the pixel that need to update.
	int i, j;
	for (i = draw_y0; i <= draw_y1; i++) {
		for (j = draw_x0; j <= draw_x1; j++) {
			draw_multilayer_pixel(j, i);
		}
	}
}


// task for display
void Task_display(void* pvParameter) {
    RenderObject obj;

    // draw the background
    int i;
    lcd_set_draw_frame(0, 0, 129, 129);
    HAL_LCD_writeCommand(CM_RAMWR);
    for (i = 0; i < 130*130; i++) {
        uint16_t bColor = img_background[i];
        if (colorInvert) bColor = ~bColor;
        HAL_LCD_writeData(bColor >> 8);
        HAL_LCD_writeData(bColor);
    }

    while(1) {
        xQueueReceive(Queue_Display, &obj, portMAX_DELAY);
        // if id >= 0, it is a normal draw request.
        if (obj.id >= 0) {
            draw_obj(&obj);
        }
        // otherwise it is a configure request.
        else {
            // use invert color
            if (obj.id == DISPLAY_INVERSE_ID) {
                if (colorInvert) continue;
                colorInvert = true;
            }
            // use normal color
            else if (obj.id == DISPLAY_NORMAL_ID) {
                if (!colorInvert) continue;
                colorInvert = false;
            }

            // redraw whole screen
            lcd_set_draw_frame(0, 0, 129, 129);
            HAL_LCD_writeCommand(CM_RAMWR);
            int i, j;
            for (j = 0; j < 130; j++) {
                for (i = 0; i < 130; i++) {
                    draw_multilayer_pixel(i, j);
                }
            }
        }
    }
}
