/*
 * tree_task.h
 *
 *      Author: Yunzhao Liu
 */

#ifndef ECE353FINALPROJECT_TREE_TASK_H_
#define ECE353FINALPROJECT_TREE_TASK_H_

typedef enum {
    TREE_INFO_NEXT,
    TREE_INFO_SET_SPEED,
} TreeInfoType;

typedef struct {
    TreeInfoType type;
    int value;
} TreeInfo;

void tree_task_init(void);
void Task_tree(void* pvParam);



#endif /* ECE353FINALPROJECT_TREE_TASK_H_ */
