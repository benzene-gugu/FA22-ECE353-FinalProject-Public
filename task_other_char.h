/*
 * other_char_task.h
 *
 *      Author: Yunzhao Liu
 */

#ifndef ECE353FINALPROJECT_OTHER_CHAR_TASK_H_
#define ECE353FINALPROJECT_OTHER_CHAR_TASK_H_

typedef enum {
    OTHER_CHAR_NEXT,
    OTHER_CHAR_SET_SPEED,
    OTHER_CHAR_CHECK_HIT_LEFT,
    OTHER_CHAR_CHECK_HIT_RIGHT,
} OtherCharInfoType;

typedef struct {
    OtherCharInfoType type;
    int value;
} OtherCharInfo;

void other_char_init(void);

void Task_other_char(void* pvParam);

#endif /* ECE353FINALPROJECT_OTHER_CHAR_TASK_H_ */
