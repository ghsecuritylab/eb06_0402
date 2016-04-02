#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "includes.h"

struct queue_node
{    //声明队列链接结构
	int data;
	struct queue_node *next;
};


struct Queue_NodeType
{    //声明队列链接结构
	u8 data[20];
	struct Queue_NodeType *next;
};


typedef struct Queue_NodeType queue_list; //定义队列类型
typedef queue_list* queue_link;    //定义队列指针类型

extern signed char QueueLenght;
extern queue_link rear;
extern queue_link front;

extern u8 delqueue_extend(u8 type,u8 dataLen,u8 *dst);
extern u8 delqueue(u8 dataLen,u8 *dst);
extern u8 addqueue(u8 dataLen,u8 *datapointer);
extern u8 out_queue_rear(u8 type,u8 dataLen,u8 *dst);
extern void dle_queue_node(queue_link node);
extern void queue_init(void);


#endif  /* _QUEUE_H_ */

