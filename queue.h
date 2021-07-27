#ifndef __QUEUE_H__
#define __QUEUE_H__


#include "lib_use.h"
#include "myType.h"

#define elementType_QUEUE Task


typedef struct Node
{
	elementType_QUEUE item;
	struct Node *next;
}Node_Queue;


typedef struct Queue
{
	Node_Queue *front;//队列头
	Node_Queue *rear;//队列尾
	int capacity;//容量
	int len;//长度
}Queue;

Queue* createQueue(int Capacity);
void addItem(Queue* queue, elementType_QUEUE item);
int isEmpty_queue(Queue* queue);
int isFull(Queue* queue);
elementType_QUEUE deleteItem(Queue* queue);
void destoryQueue(Queue** pqueue);

#endif
