#ifndef __LIST_H__
#define __LIST_H__

#include "lib_use.h"
#include "myType.h"

#define elementType_LIST int

typedef struct NODE {
	 elementType_LIST item;
	 struct NODE * next;
}Node;

typedef struct LIST{
	Node* head;
	Node* tail;
	int length;
}List;

List* createList();
int isEmpty(List* list);
void insertNode(List* list, elementType_LIST item);
Node* findNode(List* list, elementType_LIST item);
void delNode(List* list, elementType_LIST item);
void destory(List* list);
int getLength(List *list);
void displayList(List* list);
void reverse(List *list);
Node* findMidNode(List *list);
void delete_n(List *list, int n);
#endif
