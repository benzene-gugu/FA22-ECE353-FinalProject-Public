/*
 * timer_task.c
 *
 *      Author: yunzhao
 */

#include "FreeRTOS.h"
#include "queue.h"
#include "queue_handles.h"

void Task_timer(void* pvParam)
{
    // INFO_TIMER_100MS is generated every 100ms
    // INFO_TIMER_100MS or INFO_TIMER_50MS is generate every 50ms.
    while(1) {
        MainInfo info;
        info.type = INFO_TIMER_50MS;
        xQueueSend(Queue_Main, &info, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(50));
        info.type = INFO_TIMER_100MS;
        xQueueSend(Queue_Main, &info, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}


