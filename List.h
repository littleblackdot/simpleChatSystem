#ifndef __LIST_H__
#define __LIST_H__

#include "lib_use.h"
#include "myType.h"

#define elementType_LIST UserInfo_server

typedef struct NODE {
	 elementType_LIST item;
	 struct NODE * next;
}Node_List;

typedef struct LIST{
	Node_List* head;
	Node_List* tail;
	int length;
}List;

typedef int (*callBackFunc)(elementType_LIST, elementType_LIST);

List* createList();
int isEmpty_list(List* list);
void insertNode(List* list, elementType_LIST item);
Node_List* findNode(List* list, elementType_LIST item, callBackFunc isEqual_item);
void delNode(List* list, elementType_LIST item, callBackFunc  isEqual_item);
void destory(List* list);
int getLength(List *list);
void displayList(List* list);

void itemAssign(elementType_LIST *pLValue, elementType_LIST rValue);

int isEqual_itemID(elementType_LIST, elementType_LIST);
int isEqual_itemName(elementType_LIST, elementType_LIST);
int isEqual_itemSockID(elementType_LIST, elementType_LIST);
void showItem(elementType_LIST item);
#endif
