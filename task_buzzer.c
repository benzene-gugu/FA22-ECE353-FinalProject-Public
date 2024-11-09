/*
 * buzzer_task.c
 *
 *  Created on: Dec 10, 2022
 *      Author: aladawi
 */
#include <task_buzzer.h>

// array of tones (frequency) and rhythm (tempo)
// start song
uint16_t start[26] = {NOTE_C4_1, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_F4, NOTE_E4,NOTE_C4_1, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_G4, NOTE_F4,NOTE_C4_1, NOTE_C4, NOTE_C5, NOTE_A4, NOTE_F4, NOTE_F4,NOTE_E4, NOTE_D4, NOTE_AS4, NOTE_AS4, NOTE_A4,NOTE_F4, NOTE_G4, NOTE_F4};
// start song rhythm (1=one tempo, 2= half a tempo etc)
uint16_t start_ryth[26] = {4, 4, 2, 2, 2, 1, 4, 4, 2, 2, 2, 1, 4, 4, 2, 2, 4, 4, 2, 1, 4, 4, 2, 2, 2, 1};

// punch sound
uint16_t punch[3] = {NOTE_G4,NOTE_G5,NOTE_G6};
uint16_t punch_ryth[3] = {8,8,8};

// acceleration sound
uint16_t acc[9] = {NOTE_C6,NOTE_CS6,NOTE_D6,NOTE_DS6,NOTE_E6,NOTE_F6,NOTE_FS6,NOTE_G6,NOTE_GS6};
uint16_t acc_ryth[9] = {1,2,4,1,2,4,1,2,4};

// turn sound
uint16_t turn[9] = {NOTE_B0,NOTE_C1,NOTE_CS1,NOTE_D1,NOTE_DS1,NOTE_E1,NOTE_FS1,NOTE_G1,NOTE_GS1};
uint16_t turn_ryth[7] = {8,8,8,8,8,8,8};

// kick sound
uint16_t kick[2] = {NOTE_G4,NOTE_C5};
uint16_t kick_ryth[2] = {1,4};

// hit on second character
uint16_t hit[1] = {NOTE_FS6};
uint16_t hit_ryth[1] = {8};

// win sound
uint16_t win[21] = { NOTE_E4, NOTE_A4, NOTE_B4, NOTE_A4,NOTE_GS4, NOTE_AS4, NOTE_D4, NOTE_C4, NOTE_G4, NOTE_F4,NOTE_C4_1, NOTE_C4, NOTE_C5, NOTE_A4, NOTE_F4, NOTE_F4,NOTE_E4, NOTE_D4, NOTE_AS4, NOTE_AS4, NOTE_A4};
uint16_t win_ryth[21] = {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};

// lose sound
uint16_t lose[12] = {NOTE_C5,NOTE_G4,NOTE_E4,NOTE_A4,NOTE_B4,NOTE_A4,NOTE_GS4,NOTE_AS4,NOTE_GS4,NOTE_G4,NOTE_D4,NOTE_E4};
uint16_t lose_ryth[12] = {4,4,4,8,8,8,8,8,8,8,8,2};

// declaring queue
extern QueueHandle_t Queue_Buzz = NULL;

/*
 * Initiating the buzzer
 */
void song_task_init(void) {
    buzzer_init(SystemCoreClock);
    Queue_Buzz = xQueueCreate(10, sizeof(Buzz));
}
// Used to calculate the tone array length
#define ARRAY_LEN(x) (sizeof(x)/sizeof(x[0]))

/*
 * check the queue and play sound requested
 *
 *
 */
void start_song_task(void *pvParameters)
{
    Buzz input;
    // setting the start sound
    input.type = MAIN_CHAR_GO;
    while(1)
    {
        int i;

        buzzer_on();
// to restart the task_buzzer for a new sound
replay:
        //////////////////////////////////////////////////////////////
        // Sound to play while riding
        if(input.type == MAIN_CHAR_GO)
        {
            for( i = 0; i<ARRAY_LEN(start);i++)
            {
                // set the new period for each note
                buzzer_reinit(SystemCoreClock/start[i]);
                // play the note based on specified tempo
                vTaskDelay(pdMS_TO_TICKS(1000/start[i]));
                // check if there is another sound requested
                if (xQueueReceive(Queue_Buzz, &input, 0) && input.type != MAIN_CHAR_GO) {
                    goto replay;
                }
            }
        }
        ///////////////////////////////////////////////////////////////////
        // sound to play when a hit is conformed
        if(input.type == SIDE_CHAR_HIT)
        {
            for( i = 0; i<ARRAY_LEN(hit);i++)
            {
                // set the new period for each note
                buzzer_reinit(SystemCoreClock/hit[i]);
                // play the note based on specified tempo
                vTaskDelay(pdMS_TO_TICKS(1000/hit_ryth[i]));
                // check if there is another sound requested
                if (xQueueReceive(Queue_Buzz, &input, 0) && input.type != SIDE_CHAR_HIT) {
                    goto replay;
                }
            }
        }
        //////////////////////////////////////////////////////////////////
        // punch sound
        if(input.type == MAIN_CHAR_PUNCHS)
        {
            for( i = 0; i<ARRAY_LEN(punch);i++)
            {
                // set the new period for each note
                buzzer_reinit(SystemCoreClock/punch[i]);
                // play the note based on specified tempo
                vTaskDelay(pdMS_TO_TICKS(1000/punch_ryth[i]));
                // check if there is another sound requested
                if (xQueueReceive(Queue_Buzz, &input, 0) && input.type != MAIN_CHAR_PUNCHS) {
                    goto replay;
                }
            }
        }
        ////////////////////////////////////////////////////////////////
        if(input.type == MAIN_CHAR_KICKS)
        {
            for( i = 0; i<ARRAY_LEN(kick);i++)
            {
                // set the new period for each note
                buzzer_reinit(SystemCoreClock/kick[i]);
                // play the note based on specified tempo
                vTaskDelay(pdMS_TO_TICKS(1000/kick_ryth[i]));
                // check if there is another sound requested
                if (xQueueReceive(Queue_Buzz, &input, 0) && input.type != MAIN_CHAR_KICKS) {
                    goto replay;
                }
            }
        }
        /////////////////////////////////////////////////////////////////
        if(input.type == MAIN_CHAR_ACCS)
        {
            for( i = 0; i<ARRAY_LEN(acc);i++)
            {
                // set the new period for each note
                buzzer_reinit(SystemCoreClock/acc[i]);
                // play the note based on specified tempo
                vTaskDelay(pdMS_TO_TICKS(1000/acc_ryth[i]));
                // check if there is another sound requested
                if (xQueueReceive(Queue_Buzz, &input, 0) && input.type != MAIN_CHAR_ACCS) {
                    goto replay;
                }
            }
        }
        /////////////////////////////////////////////////////////////
        if(input.type == MAIN_CHAR_TURNS)
        {
            for( i = 0; i<ARRAY_LEN(turn);i++)
            {
                // set the new period for each note
                buzzer_reinit(SystemCoreClock/turn[i]);
                // play the note based on specified tempo
                vTaskDelay(pdMS_TO_TICKS(1000/turn_ryth[i]));
                // check if there is another sound requested
                if (xQueueReceive(Queue_Buzz, &input, 0) && input.type != MAIN_CHAR_TURNS) {
                    goto replay;
                }
            }
        }
        //////////////////////////////////////////////////////
        if(input.type == GAME_WIN)
        {
            for( i = 0; i<ARRAY_LEN(win);i++)
            {
                // set the new period for each note
                buzzer_reinit(SystemCoreClock/win[i]);
                // play the note based on specified tempo
                vTaskDelay(pdMS_TO_TICKS(1000/win_ryth[i]));
                // check if there is another sound requested
                if (xQueueReceive(Queue_Buzz, &input, 0) && input.type != GAME_WIN) {
                    goto replay;
                }
            }
        }
        //////////////////////////////////////////////////////
        if(input.type == GAME_LOSE)
        {
            for( i = 0; i<ARRAY_LEN(lose);i++)
            {
                // set the new period for each note
                buzzer_reinit(SystemCoreClock/lose[i]);
                // play the note based on specified tempo

                vTaskDelay(pdMS_TO_TICKS(1000/lose_ryth[i]));
                // check if there is another sound requested
                if (xQueueReceive(Queue_Buzz, &input, 0) && input.type != GAME_LOSE) {
                    goto replay;
                }
            }
        }
        // turn the buzzer of if sound ends
        buzzer_off();
        // check for new sounds
        xQueueReceive(Queue_Buzz, &input, portMAX_DELAY);
    }
}

