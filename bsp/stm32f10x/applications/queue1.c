
#ifndef _QUEUE1_C_
#define _QUEUE1_C_



#include    <locale.h>
#include    <stdio.h>
#include    <string.h>
#include    <ctype.h>
#include    <stdlib.h>
#include    <stdarg.h>

#include "malloc.h"
#include "queue.h"


#define	printf_s	printf
#define	MAX_QUEUE_LIST	15

#ifndef u8
typedef unsigned char u8;
#endif
	

signed char QueueLenght = 0;

queue_link front = NULL;      //初始队头指针
queue_link rear  = NULL;      //初始队尾指针

//输入队列数据
//数据已经满返回2，内存不够返回0，正常接收返回1
u8 addqueue(u8 dataLen,u8 *datapointer)
{
	u8 i;
	
	queue_link new_node;

	new_node = (queue_link) mymalloc(sizeof(queue_list));

		
	if(new_node != NULL)
	{
		if(QueueLenght < MAX_QUEUE_LIST)
		{
			for(i=0;i<dataLen;i++)
				new_node->data[i] = datapointer[i];    //将数据存入队列
				
			new_node->next = NULL;    //设置初值
			
			if(rear == NULL || front == NULL) //若为队列的第一个数据 
				front = rear = new_node;    //将front指向新节点
			else
				rear = rear->next = new_node;   //rear所指的节点指向新节点
			
			QueueLenght++;
			return 1;
		}
		else
		{
			return 2;
		}
		
	}
	else
		return 0;
}


/*------------------------------------------------------------
操作目的： 得到队列的长度
初始条件： 队列Q已存在
操作结果： 返回Q中数据元素的个数
函数参数：
   LinkQueue Q   队列Q
返回值：
   int     数据元素的个数
------------------------------------------------------------*/
int QueueLength(queue_link Q)
{
	int count = 0;

    queue_link p;
    p = front; //p指向头结点
    while(p->next != NULL)
    {
        count++;
        p = p->next;
    }
    return count;
}


//输出队列数据
//有数据返回1，无数据返回0
//type:0,表示无损式数据抽取，每次读取缓冲数据时，会保留当前读取的数据在缓冲区
//1,表示为有损式抽取数据，每次读取数据后，缓冲区会同时清空被读取的数据
u8 delqueue_extend(u8 type,u8 dataLen,u8 *dst)
{
	queue_link top;
	u8 i;
	
	if(front != NULL)
	{   

		//检查队列是否为空
		top   = front;     //将top指向front

		if(type)
			front = front->next;   //

		for(i=0;i<dataLen;i++)
			dst[i]  = top->data[i];    //暂存输出队列数据

		if(type)
		{
			myfree(top);      //释放输出节点的内容
			QueueLenght--;
			if(QueueLenght < 0)
				QueueLenght = 0;
		}
		return 1;

	}
	else
		return 0;
}


//输出队列数据
//有数据返回1，无数据返回0
u8 delqueue(u8 dataLen,u8 *dst)
{
	queue_link top;
	u8 i;
	
	if(front != NULL)
	{ 
		//检查队列是否为空
		top   = front;     //将top指向front
		front = front->next;   //

		for(i=0;i<dataLen;i++)
			dst[i]  = top->data[i];    //暂存输出队列数据
		myfree(top);      //释放输出节点的内容

		QueueLenght--;
		if(QueueLenght < 0)
			QueueLenght = 0;
		return 1;

	}
	else
		rear = NULL;
	return 0;
}


void queue_init(void)
{
	my_mem_init();
	front = NULL;
	rear = NULL;
	QueueLenght = 0;
}

#if 0
//进行队列输出输入，并打印出队列内容
int debug_main(void)
{
 int select;        //功能选项变量
 int  temp;
 queue_link point;
 
 printf_s("(1)Input a data\n");
 printf_s("(2)Output a data\n");
 printf_s("(3)Exit\n");
 printf_s("Please select:");
 scanf("%d", &select);
 printf_s("\n");
 if(select != 3){
  do{
   switch(select){
    case 1: printf_s("Please input the input value == >");
     scanf("%d", &temp);

	 addqueue(temp);      //将数据插入队列中

	if(Keyboard_linkqueue->QueueLenght > MAX_QUEUE_LIST)
	 	printf("The queue is full.\n");

	printf_s("The content of queue:"); //输出队列内容

	point = Keyboard_linkqueue->front;
	while(point != NULL){
	printf_s("[%d]", point->data);
	point = point->next;
     }
     break;
    case 2: if((temp = delqueue()) == -1) //判断队列是否为空
       printf_s("The queue is empty!!!\n");
      else{
       printf_s("The output value is [%d]\n", temp);
       printf_s("The content of queue:"); //输出队列内容
       point = Keyboard_linkqueue->front;
       while(point != NULL){
        printf_s("[%d]", point->data);
        point = point->next;
       }
      }
      break;
   }
   printf_s("\n(1)Input a data\n");
   printf_s("\n(2)Output a data\n");
   printf_s("\n(3)Exit\n");
   printf_s("Please select:");
   scanf("%d", &select);
   printf_s("\n");
  }while(select != 3);
 }
 return 0;
}

#endif

#endif  /* _QUEUE1_C_ */

