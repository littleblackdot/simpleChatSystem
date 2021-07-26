#ifndef __QUEUE_H__
#define __QUEUE_H__


#include "lib_use.h"
#include "myType.h"

#define elementType Task


typedef struct Node
{
	elementType item;
	struct Node *next;
}Node;


typedef struct Queue
{
	Node *front;//队列头
	Node *rear;//队列尾
	int capacity;//容量
	int len;//长度
}Queue;

Queue* createQueue(int Capacity);
void addItem(Queue* queue, elementType item);
int isEmpty(Queue* queue);
int isFull(Queue* queue);
elementType deleteItem(Queue* queue);
void destoryQueue(Queue** pqueue);

#endif
