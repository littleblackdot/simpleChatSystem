#include"queue.h"

Queue* createQueue(int Capacity)
{
	Queue *queue;
	queue = (Queue*)malloc(sizeof(Queue));
	if(queue == NULL)
	{
		printf("fail to alloc memeory\n");
		return NULL;
	}
	queue->front = NULL;
	queue->rear = NULL;
	queue->capacity = Capacity;
	queue->len = 0;
	return queue;
}

int isEmpty_queue(Queue *queue)
{
	if(queue == NULL)
	{
		printf("queue is null\n");
		return -1;
	}
	return queue->front == NULL;
}

int isFull(Queue *queue){
	if(queue == NULL){
		printf("queue is null\n");
		return -1;
	}
	return queue->len == queue->capacity;
}

void addItem(Queue *queue, elementType_QUEUE item)
{
	if(queue == NULL)
	{
		printf("queue is null\n");
		return ;
	}
	Node_Queue *node = (Node_Queue*)malloc(sizeof(Node_Queue));
	if(node == NULL)
	{
		printf("fail to alloc memory\n");
		return ;
	}
	node->item = item;
	node->next = NULL;
	if(isEmpty_queue(queue))
	{
		queue->front = queue->rear = node;
		(queue->len)++;
		return;
	}
	(queue->len)++;
	queue->rear->next = node;
	queue->rear = node;
}

elementType_QUEUE deleteItem(Queue* queue)
{
	Node_Queue *node;
	elementType_QUEUE item;
	if(queue == NULL)
	{
		printf("queue is null\n");
		return item;
	}
	if(isEmpty_queue(queue))
	{
		printf("no item in the queue\n");
		return item;
	}
	
	node = queue->front;
	queue->front = node->next;
	(queue->len)--;
	item = node->item;
	free(node);
	return item;
}

void destoryQueue(Queue** pqueue)
{
	if(pqueue == NULL || *pqueue == NULL)
	{
		printf("queue is null\n");
		return;
	}
	Queue *queue = *pqueue;
	while(!isEmpty_queue(queue))
	{
		Node_Queue *node = queue->front;
		queue->front = node->next;
		free(node);
	}
	free(queue);
	*pqueue = NULL;
}
