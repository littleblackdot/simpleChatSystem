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

int isEmpty_list(List* list){
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
	Node_List* node = (Node_List*)malloc(sizeof(Node_List));
	itemAssign(&node->item, item);
	node->next = NULL;
	if(isEmpty_list(list)){
		list->head = node;
		list->tail = node;
		++(list->length);
		return;
	}
	node->next = list->head;
	list->head = node;
	++(list->length);
}

Node_List* findNode(List* list, elementType_LIST item, callBackFunc isEqual_item){
	if(list == NULL){
		printf("Head is NULL\n");
		return NULL;
	}
	if(isEmpty_list(list)){
		printf("there is no items in list\n");
		return NULL;
	}
	Node_List* p;
	for(p=list->head; p!=NULL; p=p->next){
		if(isEqual_item(p->item , item)){
			break;
		}
	}
	return p;
}

void delNode(List* list, elementType_LIST item, callBackFunc isEqual_item){
	if(list == NULL){
		printf("Head is NULL\n");
		return ;
	}
	Node_List *p, *pre;
	for(p = list->head; p!= NULL; pre = p,p = p->next){
		if(isEqual_item(p->item , item)){
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
	for(Node_List* p=list->head; p!=NULL;){
		Node_List* temp = p;
		p = p->next;
		free(temp);
	}
}

void displayList(List* list){
	if(list == NULL){
		printf("list is NULL");
		return;
	}
	for(Node_List* p=list->head; p!=NULL; p=p->next){
		 showItem(p->item);
		 //printf("%d ", p->item);//待改进，要根据elementType来做输出处理......
	}
}



int getLength(List *list){
	return list->length;
}

void itemAssign(elementType_LIST *pLValue, elementType_LIST rValue){
	pLValue->sockid = rValue.sockid;
	pLValue->status = rValue.status;
	pLValue->userType = rValue.userType;
	pLValue->userInfo_c.id = rValue.userInfo_c.id;
	memset(pLValue->userInfo_c.name, 0, sizeof(pLValue->userInfo_c.name));
	memset(pLValue->userInfo_c.pwd, 0, sizeof(pLValue->userInfo_c.pwd));
	strncpy(pLValue->userInfo_c.name, rValue.userInfo_c.name, strlen(rValue.userInfo_c.name));
	strncpy(pLValue->userInfo_c.pwd,  rValue.userInfo_c.pwd, strlen(rValue.userInfo_c.pwd));
}

int isEqual_itemID(elementType_LIST itemA, elementType_LIST itemB){
	return itemA.userInfo_c.id == itemB.userInfo_c.id;
}
int isEqual_itemName(elementType_LIST itemA, elementType_LIST itemB){
	return strncmp(itemA.userInfo_c.name, itemB.userInfo_c.name,
	 strlen(itemA.userInfo_c.name)) == 0 ? 1 : 0;
}

int isEqual_itemSockID(elementType_LIST itemA, elementType_LIST itemB){
	return itemA.sockid == itemB.sockid;
}

void showItem(elementType_LIST item){
	printf("sockid: %d\n", item.sockid);
	printf("userName: %s\n", item.userInfo_c.name);
	printf("pwd: %s\n", item.userInfo_c.pwd);
	printf("status: %d\n", item.status);
	printf("userType:%d\n", item.userType);
}

