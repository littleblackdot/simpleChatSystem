#include "dataParse.h"


void formatMsgToJson_msgToServer(const msgToServer msg, char * jsonContainer){
    cJSON *json = cJSON_CreateObject();
    if(json == NULL){
        printf("format error, check your configuration\n");
        return;
    }
    char *buffer;
    cJSON_AddStringToObject(json, "name", msg.userInfo.name);
    cJSON_AddStringToObject(json, "pwd", msg.userInfo.pwd);
    cJSON_AddStringToObject(json, "message", msg.message);
    cJSON_AddNumberToObject(json, "action", msg.action);
    cJSON_AddNumberToObject(json, "id", msg.userInfo.id);
    buffer = cJSON_Print(json);
    if(strlen(buffer) == 0){
        printf("format error\n");
        return;
    }
    strncpy(jsonContainer, buffer, strlen(buffer));
    free(buffer);
    cJSON_Delete(json);
}


void parseJsonData_Server(msgToServer *pmsg, const char *jsonData){
    cJSON *json = NULL;
    json = cJSON_Parse(jsonData);
    if(json == NULL){
        printf("json is NULL\n");
    }

    cJSON *node1 = cJSON_GetObjectItemCaseSensitive(json, "action");
    if(node1 != NULL){
        if(cJSON_IsNumber){
            pmsg->action = (ActionType_Client)node1->valueint;
        }
    }

    cJSON *node2 = cJSON_GetObjectItemCaseSensitive(json, "id");
    if(node2 != NULL){
        if(cJSON_IsNumber){
            pmsg->userInfo.id = node2->valueint;
        }
    }
    cJSON *node3 = cJSON_GetObjectItemCaseSensitive(json, "name");
    if(node3 != NULL){
       if(cJSON_IsString(node3) && node3->valuestring != NULL){
            memset(pmsg->userInfo.name, 0, sizeof(pmsg->userInfo.name));
            strncpy(pmsg->userInfo.name, node3->valuestring, strlen(node3->valuestring));
        }
    }

    cJSON *node4 = cJSON_GetObjectItemCaseSensitive(json, "pwd");
    if(node4 != NULL){
        if(cJSON_IsString(node4) && node4->valuestring != NULL){
            memset(pmsg->userInfo.pwd, 0, sizeof(pmsg->userInfo.pwd));
            strncpy(pmsg->userInfo.pwd, node4->valuestring, strlen(node4->valuestring));
        }
    }

    cJSON *node5 = cJSON_GetObjectItemCaseSensitive(json, "message");
    if(node5 != NULL){
        if(cJSON_IsString(node4) && node5->valuestring != NULL){
            memset(pmsg->message, 0, sizeof(pmsg->message));
            strncpy(pmsg->message, node5->valuestring, strlen(node5->valuestring));
        }
    }
    cJSON_Delete(json);
}

void showMsg_msgToServer(const msgToServer msg){
    printf("action: %d\n", msg.action);
    printf("id: %d\n", msg.userInfo.id);
    printf("name: %s\n", msg.userInfo.name);
    printf("pwd: %s\n", msg.userInfo.pwd);
    printf("message: %s\n", msg.message);
}