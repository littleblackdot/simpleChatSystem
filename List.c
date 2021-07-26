#include"List.h"


List* createList(){
	List* list = (List*)malloc(sizeof(List));
	if(list == NULL){
		printf("fail to get memory\n");
		return NULL;
	}
	list->head = NULL;
	list->tail = NULL;
	list->length = 0;
	return list;
}

int isEmpty(List* list){
	if(list == NULL){
		printf("list is NULL\n");
		return -1;
	}
	return list->head == NULL;
}


void insertNode(List* list, elementType_LIST item){//头插
	if(list == NULL){
		printf("list is NULL\n");
		return ;
	}
	Node* node = (Node*)malloc(sizeof(Node));
	node->item = item;
	node->next = NULL;
	if(isEmpty(list)){
		list->head = node;
		list->tail = node;
		++(list->length);
		return;
	}
	node->next = list->head;
	list->head = node;
	++(list->length);
}

Node* findNode(List* list, elementType_LIST item){
	if(list == NULL){
		printf("Head is NULL\n");
		return NULL;
	}
	if(isEmpty(list)){
		printf("there is no items in list\n");
		return NULL;
	}
	Node* p;
	for(p=list->head; p!=NULL; p=p->next){
		if(p->item == item){
			break;
		}
	}
	return p;
}

void delNode(List* list, elementType_LIST item){
	if(list == NULL){
		printf("Head is NULL\n");
		return ;
	}
	Node *p, *pre;
	for(p = list->head; p!= NULL; pre = p,p = p->next){
		if(p->item == item){
			break;
		}
	}
	if(p == NULL){	
		printf("fail to delete item from list,it is not in the list");
		return ;
	}
	(list->length)--;
	if(p == list->head){
		list->head  = p->next;
		free(p);
		return;
	}
	if(p == list->tail){
		list->tail = pre;
		list->tail->next = NULL;
		free(p);
		return;
	}
	pre->next = p->next;
	free(p);
}
void destory(List* list){		
	if(list == NULL){
		printf("list is NULL");
		return ;
	}
	for(Node* p=list->head; p!=NULL;){
		Node* temp = p;
		p = p->next;
		free(temp);
	}
}

void displayList(List* list){
	if(list == NULL){
		printf("list is NULL");
		return;
	}
	for(Node* p=list->head; p!=NULL; p=p->next){
		 printf("%d ", p->item);//待改进，要根据elementType来做输出处理......
	}
}

void reverse(List *list){
	if(list == NULL){
		printf("list is null");
	}
	if(isEmpty(list)){
		return;
	}
	Node *curNode = list->head;
	Node *tempNode = curNode;
	curNode = curNode->next;
	tempNode->next = NULL;
	for(;curNode!=NULL;){
		tempNode = curNode;
		curNode = curNode->next;
		tempNode->next = list->head;
		list->head = tempNode;
	}
}

Node* findMidNode(List *list){
	if(list == NULL){
		printf("list is null");
		return NULL;
	}
	if(isEmpty(list)){
		return NULL;
	}
	Node *curNode1, *curNode2;
	curNode1 = list->head;
	curNode2 = list->head->next;
	while(curNode2!=NULL){
		curNode1 = curNode1->next;
		curNode2 = curNode2->next;
		if(curNode2 == NULL){
			break;
		}
		curNode2 = curNode2->next;
	}
	return curNode1;
}

void delete_n(List *list, int n){
	if(list == NULL){
		printf("list is null");
		return ;
	}
	if(getLength(list) < n){
		printf("no such more items");
		return ;
	}
	Node *curNode1, *curNode2, *tempNode;
	curNode1 = curNode2 = list->head;
	tempNode = NULL;
	for(int i=2; i<=n && curNode2 != NULL; i++){
		curNode2 = curNode2->next;
	}
	while(curNode2->next != NULL){
		tempNode = curNode1;
		curNode1 = curNode1->next;
		curNode2 = curNode2->next;
	}
	if(curNode1 == list->head){
		list->head = curNode1->next;
		free(curNode1);
		--(list->length);
		return;
	}
	tempNode->next = curNode1->next;
	free(curNode1);
	--(list->length);
}

int getLength(List *list){
	return list->length;
}
