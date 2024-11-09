/*
 * task_main_char.c
 *
 *      Author: Yunzhao Liu
 */
#include "task_main_char.h"
#include "task_display.h"
#include "msp.h"
#include "lcd.h"

#include "image.h"

#include <queue_handles.h>
#include <queue.h>
#include <stdbool.h>

#include <task_buzzer.h>


extern QueueHandle_t Queue_Main_Char = NULL;

void main_char_init(void) {
    Queue_Main_Char = xQueueCreate(10, sizeof(MainCharInfo));
}

typedef enum {
    MAIN_CHAR_STATE_RIDE,
    MAIN_CHAR_STATE_ACC,
    MAIN_CHAR_STATE_LEFT,
    MAIN_CHAR_STATE_RIGHT,
    MAIN_CHAR_STATE_LEFT_RETURN,
    MAIN_CHAR_STATE_RIGHT_RETURN,
    MAIN_CHAR_STATE_PUNCH_LEFT,
    MAIN_CHAR_STATE_PUNCH_RIGHT,
    MAIN_CHAR_STATE_KICK_LEFT,
    MAIN_CHAR_STATE_KICK_RIGHT,
} MainCharState;



void Task_main_char(void* pvParam)
{
	RenderObject obj = {
	        .id = 0,
			.img = &imageInfos[IMG_WAIT_1],
			.posx = 80,
			.posy = 128,
			.scale = 128,
	};

	MainCharState state = MAIN_CHAR_STATE_RIDE;
	MainCharState next_state = state;
	MainCharState return_next_state = state; // only used for return_state
	int animationCnt = 0;
	bool state_init = false;
	MainCharInfo input;
	MainInfo info;

	int speed = 20;

	while(1) {
	    xQueueReceive(Queue_Main_Char, &input, portMAX_DELAY);
	    // win animation (loop forever)
	    if (input.type == MAIN_CHAR_WIN) {
	        // event loop after win.
	        animationCnt = 0;
	        // play game win music
	        Buzz buzz = {GAME_WIN};
	        xQueueSend(Queue_Buzz, &buzz, portMAX_DELAY);

	        while(1) {
	            xQueueReceive(Queue_Main_Char, &input, portMAX_DELAY);
	            if (input.type == MAIN_CHAR_NEXT) {
	                if (animationCnt / 2 % 2) { // every 4x50ms
                        obj.img = &imageInfos[IMG_VICTORY_1 + animationCnt/4];
                        xQueueSend(Queue_Display, &obj, portMAX_DELAY);
	                }
	                animationCnt = (animationCnt + 1) % 8;
	            }
	        }
	    }
	    // lose animation
	    if (input.type == MAIN_CHAR_LOSE) {
	        // set speed to 0
	        info.type = INFO_SET_SPEED;
	        info.value = 0;
	        xQueueSend(Queue_Main, &info, portMAX_DELAY);
	        // play game lose music
	        Buzz buzz = {GAME_LOSE};
	        xQueueSend(Queue_Buzz, &buzz, portMAX_DELAY);

	        animationCnt = 0;
	        while(1) {
	            xQueueReceive(Queue_Main_Char, &input, portMAX_DELAY);
	            if (input.type == MAIN_CHAR_NEXT) {
	                if (animationCnt < 4) {
	                    obj.img = &imageInfos[IMG_FALL_1 + animationCnt];
	                    xQueueSend(Queue_Display, &obj, portMAX_DELAY);
	                    animationCnt++;
	                }
	            }
	        }
	    }
	    // play music based on the state transition
	    if (input.type == MAIN_CHAR_NEXT) {
	        if (next_state != state) {
	            Buzz buzz;
	            if (next_state == MAIN_CHAR_STATE_ACC) {
	                buzz.type = MAIN_CHAR_ACCS;
	            }
	            else if (next_state == MAIN_CHAR_STATE_KICK_LEFT ||
	                    next_state == MAIN_CHAR_STATE_KICK_RIGHT){
	                buzz.type = MAIN_CHAR_KICKS;
	            }
	            else if (next_state == MAIN_CHAR_STATE_PUNCH_LEFT ||
	                    next_state == MAIN_CHAR_STATE_PUNCH_RIGHT) {
	                buzz.type = MAIN_CHAR_PUNCHS;
	            }
	            else if (next_state == MAIN_CHAR_STATE_LEFT ||
	                    next_state == MAIN_CHAR_STATE_RIGHT) {
	                buzz.type = MAIN_CHAR_TURNS;
	            }
	            else {
	                buzz.type = MAIN_CHAR_GO;
	            }

	            xQueueSend(Queue_Buzz, &buzz, portMAX_DELAY);
	        }
	        state = next_state;
	    }
	    // initialize current state
	    if (state_init && input.type == MAIN_CHAR_NEXT) {
	        state_init = false;
	        animationCnt = 0;
	        if (state == MAIN_CHAR_STATE_LEFT ||
	                state == MAIN_CHAR_STATE_PUNCH_LEFT ||
	                state == MAIN_CHAR_STATE_KICK_LEFT) {
	            if (obj.scale > 0) obj.scale = -obj.scale;
	        }
	        else if (state == MAIN_CHAR_STATE_RIGHT ||
	                state == MAIN_CHAR_STATE_PUNCH_RIGHT ||
	                state == MAIN_CHAR_STATE_KICK_RIGHT) {
	            if (obj.scale < 0) obj.scale = -obj.scale;
	        }
	    }
        // speed
        if (input.type == MAIN_CHAR_NEXT) {
            if (state == MAIN_CHAR_STATE_ACC) {
                if (speed < 40) {
                    speed+=2;
                    info.type = INFO_SET_SPEED;
                    info.value = speed;
                    xQueueSend(Queue_Main, &info, portMAX_DELAY);
                }
            }
            else {
                if (speed > 20) {
                    speed-=1;
                    info.type = INFO_SET_SPEED;
                    info.value = speed;
                    xQueueSend(Queue_Main, &info, portMAX_DELAY);
                }
            }
        }
        // hurtbox check
        if (input.type == MAIN_CHAR_CHECK_HIT) {
            if (obj.posx < input.value+10 && obj.posx > input.value-10) {
                info.type = INFO_MAIN_CHAR_HURT;
                xQueueSend(Queue_Main, &info, portMAX_DELAY);
            }
        }
	    // return_next_state. It is used for MAIN_CHAR_STATE_*_RETURN state.
	    if (input.type == MAIN_CHAR_TURN) {
	        if (input.value < 0) {
	            return_next_state = MAIN_CHAR_STATE_LEFT;
	        }
	        else if (input.value > 0) {
	            return_next_state = MAIN_CHAR_STATE_RIGHT;
	        }
	        else {
	            return_next_state = MAIN_CHAR_STATE_RIDE;
	        }
	    }
	    else if (input.type == MAIN_CHAR_ACC) {
	        return_next_state = MAIN_CHAR_STATE_ACC;
	    }
	    else if (input.type == MAIN_CHAR_PUNCH) {
	        if (input.value < 0) {
	            return_next_state = MAIN_CHAR_STATE_PUNCH_LEFT;
	        }
	        else if (input.value > 0) {
	            return_next_state = MAIN_CHAR_STATE_PUNCH_RIGHT;
	        }
	    }
	    else if (input.type == MAIN_CHAR_KICK) {
	        if (input.value < 0) {
	            return_next_state = MAIN_CHAR_STATE_KICK_LEFT;
	        }
	        else if (input.value > 0) {
	            return_next_state = MAIN_CHAR_STATE_KICK_RIGHT;
	        }
	    }
	    // state changes & animation
	    // STATE RIDE
	    if (state == MAIN_CHAR_STATE_RIDE) {
            if (input.type == MAIN_CHAR_NEXT) { // update frame
                if (animationCnt == 0) {
                    obj.img = &imageInfos[IMG_ACCELERATE_3];
                    xQueueSend(Queue_Display, &obj, portMAX_DELAY);
                    animationCnt++;
                }
            }
            else if (input.type == MAIN_CHAR_TURN) {
                if (input.value > 0) {
                    next_state = MAIN_CHAR_STATE_RIGHT;
                    state_init = true;
                }
                else if (input.value < 0) {
                    next_state = MAIN_CHAR_STATE_LEFT;
                    state_init = true;
                }
            }
            else if (input.type == MAIN_CHAR_ACC) {
                if (input.value) {
                    next_state = MAIN_CHAR_STATE_ACC;
                    state_init = true;
                }
            }
            else if (input.type == MAIN_CHAR_PUNCH) {
                if (input.value < 0) {
                    next_state = MAIN_CHAR_STATE_PUNCH_LEFT;
                    state_init = true;
                }
                else if (input.value > 0) {
                    next_state = MAIN_CHAR_STATE_PUNCH_RIGHT;
                    state_init = true;
                }
            }
            else if (input.type == MAIN_CHAR_KICK) {
                if (input.value < 0) {
                    next_state = MAIN_CHAR_STATE_KICK_LEFT;
                    state_init = true;
                }
                else if (input.value > 0) {
                    next_state = MAIN_CHAR_STATE_KICK_RIGHT;
                    state_init = true;
                }
            }
	    }
	    // STATE RIGHT
	    else if (state == MAIN_CHAR_STATE_RIGHT) {
            if (input.type == MAIN_CHAR_NEXT) { // update frame
                if (obj.posx < 120) obj.posx++;
                if (animationCnt < 5) {
                    obj.img = &imageInfos[IMG_RIGHT_1 + animationCnt];
                    animationCnt++;
                }
                xQueueSend(Queue_Display, &obj, portMAX_DELAY);
            }
            else if (input.type == MAIN_CHAR_TURN || input.type == MAIN_CHAR_ACC) {
                next_state = MAIN_CHAR_STATE_RIGHT_RETURN;
                animationCnt--;
            }
	    }
	    // STATE LEFT
	    else if (state == MAIN_CHAR_STATE_LEFT) {
            if (input.type == MAIN_CHAR_NEXT) { // update frame
                if (obj.posx > 10) obj.posx--;
                if (animationCnt < 5) {
                    obj.img = &imageInfos[IMG_RIGHT_1 + animationCnt];
                    animationCnt++;
                }
                xQueueSend(Queue_Display, &obj, portMAX_DELAY);
            }
            else if (input.type == MAIN_CHAR_TURN || input.type == MAIN_CHAR_ACC) {
                next_state = MAIN_CHAR_STATE_LEFT_RETURN;
                animationCnt--;
            }
        }
	    // STATE RIGHT RETURN
	    else if (state == MAIN_CHAR_STATE_RIGHT_RETURN) {
	        if (input.type == MAIN_CHAR_NEXT) { // update frame
	            if (obj.posx < 120) obj.posx++;
	            obj.img = &imageInfos[IMG_RIGHT_1 + animationCnt];
	            xQueueSend(Queue_Display, &obj, portMAX_DELAY);

                if (animationCnt > 0) {
                    animationCnt--;
                }
                else {
                    next_state = return_next_state;
                    return_next_state = MAIN_CHAR_STATE_RIDE;
                    state_init = true;
                }
	        }
	        else if (input.type == MAIN_CHAR_TURN) {
	            if (input.value > 0) {
	                next_state = MAIN_CHAR_STATE_RIGHT;
	            }
	        }
	    }
	    // STATE LEFT RETURN
	    else if (state == MAIN_CHAR_STATE_LEFT_RETURN) {
            if (input.type == MAIN_CHAR_NEXT) { // update frame
                if(obj.posx > 10) obj.posx--;
                obj.img = &imageInfos[IMG_RIGHT_1 + animationCnt];
                xQueueSend(Queue_Display, &obj, portMAX_DELAY);

                if (animationCnt > 0) {
                    animationCnt--;
                }
                else {
                    next_state = return_next_state;
                    return_next_state = MAIN_CHAR_STATE_RIDE;
                    state_init = true;
                }
            }
            else if (input.type == MAIN_CHAR_TURN) {
                if (input.value < 0) {
                     next_state = MAIN_CHAR_STATE_LEFT;
                }
            }
	    }
	    // STATE ACC
	    else if (state == MAIN_CHAR_STATE_ACC) {
	        if (input.type == MAIN_CHAR_NEXT) { // update frame
	            obj.img = &imageInfos[IMG_ACCELERATE_1 + animationCnt];
	            xQueueSend(Queue_Display, &obj, portMAX_DELAY);

	            if (animationCnt < 1) {
	                animationCnt++;
	            }
	        }
	        else if (input.type == MAIN_CHAR_ACC) {
	            if (input.value == 0) {
	                next_state = MAIN_CHAR_STATE_RIDE;
	                animationCnt = 0;
	            }
	        }
	    }
	    // STATE PUNCH RIGHT
	    else if (state == MAIN_CHAR_STATE_PUNCH_RIGHT) {
	        if (input.type == MAIN_CHAR_NEXT) { // update frame
	            // animation
	            if (animationCnt == 0) {
	                obj.img = &imageInfos[IMG_PUNCH_1];
	                xQueueSend(Queue_Display, &obj, portMAX_DELAY);

	            }
	            else if (animationCnt == 4){
	                obj.img = &imageInfos[IMG_PUNCH_2];
	                xQueueSend(Queue_Display, &obj, portMAX_DELAY);
	            }
	            if (animationCnt < 10) {
	                animationCnt++;
	            }
	            else {
                    if (return_next_state == MAIN_CHAR_STATE_PUNCH_RIGHT) {
                        return_next_state = MAIN_CHAR_STATE_RIDE;
                    }
	                next_state = return_next_state;
	                return_next_state = MAIN_CHAR_STATE_RIDE;
	                state_init = true;
	            }

	            // hitbox
	            if (animationCnt >= 4) {
                    info.type = INFO_MAIN_CHAR_HIT_RIGHT;
                    info.value = obj.posx + 25;
                    xQueueSend(Queue_Main, &info, portMAX_DELAY);
	            }
	        }
	    }
	    // STATE PUNCH LEFT
	    else if (state == MAIN_CHAR_STATE_PUNCH_LEFT) {
            if (input.type == MAIN_CHAR_NEXT) { // update frame
                // animation
                if (animationCnt == 0) {
                    obj.img = &imageInfos[IMG_PUNCH_1];
                    xQueueSend(Queue_Display, &obj, portMAX_DELAY);
                }
                else if (animationCnt == 4){
                    obj.img = &imageInfos[IMG_PUNCH_2];
                    xQueueSend(Queue_Display, &obj, portMAX_DELAY);
                }
                if (animationCnt < 10) {
                    animationCnt++;
                }
                else {
                    if (return_next_state == MAIN_CHAR_STATE_PUNCH_LEFT) {
                        return_next_state = MAIN_CHAR_STATE_RIDE;
                    }
                    next_state = return_next_state;
                    return_next_state = MAIN_CHAR_STATE_RIDE;
                    state_init = true;
                }

                // hitbox
                if (animationCnt >= 4) {
                    info.type = INFO_MAIN_CHAR_HIT_LEFT;
                    info.value = obj.posx - 25;
                    xQueueSend(Queue_Main, &info, portMAX_DELAY);
                }
            }
	    }
	    // STATE KICK RIGHT
	    else if (state == MAIN_CHAR_STATE_KICK_RIGHT) {
            if (input.type == MAIN_CHAR_NEXT) { // update frame
                // animation
                if (animationCnt == 0) {
                    obj.img = &imageInfos[IMG_KICK_1];
                    xQueueSend(Queue_Display, &obj, portMAX_DELAY);
                }
                else if (animationCnt == 4){
                    obj.img = &imageInfos[IMG_KICK_2];
                    xQueueSend(Queue_Display, &obj, portMAX_DELAY);
                }
                if (animationCnt < 10) {
                    animationCnt++;
                }
                else {
                    if (return_next_state == MAIN_CHAR_STATE_KICK_RIGHT) {
                        return_next_state = MAIN_CHAR_STATE_RIDE;
                    }
                    next_state = return_next_state;
                    return_next_state = MAIN_CHAR_STATE_RIDE;
                    state_init = true;
                }

                // hitbox
                if (animationCnt >= 4) {
                    info.type = INFO_MAIN_CHAR_HIT_RIGHT;
                    info.value = obj.posx + 25;
                    xQueueSend(Queue_Main, &info, portMAX_DELAY);
                }
            }
	    }
        // STATE KICK LEFT
        else if (state == MAIN_CHAR_STATE_KICK_LEFT) {
            if (input.type == MAIN_CHAR_NEXT) { // update frame
                // animation
                if (animationCnt == 0) {
                    obj.img = &imageInfos[IMG_KICK_1];
                    xQueueSend(Queue_Display, &obj, portMAX_DELAY);
                }
                else if (animationCnt == 4){
                    obj.img = &imageInfos[IMG_KICK_2];
                    xQueueSend(Queue_Display, &obj, portMAX_DELAY);
                }
                if (animationCnt < 10) {
                    animationCnt++;
                }
                else {
                    if (return_next_state == MAIN_CHAR_STATE_KICK_LEFT) {
                        return_next_state = MAIN_CHAR_STATE_RIDE;
                    }
                    next_state = return_next_state;
                    return_next_state = MAIN_CHAR_STATE_RIDE;
                    state_init = true;
                }

                // hitbox
                if (animationCnt >= 4) {
                    info.type = INFO_MAIN_CHAR_HIT_LEFT;
                    info.value = obj.posx - 25;
                    xQueueSend(Queue_Main, &info, portMAX_DELAY);
                }
            }
        }
	}
}
