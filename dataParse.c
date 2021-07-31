#include "dataParse.h"


void formatMsgToJson_msgToServer(msgToServer msg, char * jsonContainer){
    cJSON *json = cJSON_CreateObject();
    if(json == NULL){
        printf("format error, check your configuration\n");
        return;
    }
    char *buffer;
    cJSON *userInfo= cJSON_CreateObject();
    cJSON_AddNumberToObject(userInfo, "id", msg.userInfo.id);
    cJSON_AddStringToObject(userInfo, "name", msg.userInfo.name);
    cJSON_AddStringToObject(userInfo, "pwd", msg.userInfo.pwd);
    cJSON_AddItemToObject(json, "userInfo", userInfo);
    cJSON_AddStringToObject(json, "message", msg.message);
    cJSON_AddNumberToObject(json, "action", msg.action);
    cJSON_AddNumberToObject(json, "actionOption", msg.actionOption);
    
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
        return;
    }
 
    cJSON *action = cJSON_GetObjectItemCaseSensitive(json, "action");
    if(action != NULL){
        if(cJSON_IsNumber(action)){
            pmsg->action = (ActionType_Client)action->valueint;
        }
    }
    cJSON *actionOption = cJSON_GetObjectItemCaseSensitive(json, "actionOption");
    if(actionOption != NULL){
        if(cJSON_IsNumber(actionOption)){
            pmsg->actionOption = actionOption->valueint;
        }
    }
    cJSON *userInfo = cJSON_GetObjectItemCaseSensitive(json, "userInfo");
    cJSON *id = cJSON_GetObjectItemCaseSensitive(userInfo, "id");
    if(id != NULL){
        if(cJSON_IsNumber(id)){
            pmsg->userInfo.id = id->valueint;
        }
    }
    cJSON *name = cJSON_GetObjectItemCaseSensitive(userInfo, "name");
    if(name != NULL){
       if(cJSON_IsString(name) && name->valuestring != NULL){
            memset(pmsg->userInfo.name, 0, sizeof(pmsg->userInfo.name));
            strncpy(pmsg->userInfo.name, name->valuestring, strlen(name->valuestring));
        }
    }

    cJSON *pwd = cJSON_GetObjectItemCaseSensitive(userInfo, "pwd");
    if(pwd != NULL){
        if(cJSON_IsString(pwd) && pwd->valuestring != NULL){
            memset(pmsg->userInfo.pwd, 0, sizeof(pmsg->userInfo.pwd));
            strncpy(pmsg->userInfo.pwd, pwd->valuestring, strlen(pwd->valuestring));
        }
    }

    cJSON *mess = cJSON_GetObjectItemCaseSensitive(json, "message");
    if(mess != NULL){
        if(cJSON_IsString(mess) && mess->valuestring != NULL){
            memset(pmsg->message, 0, sizeof(pmsg->message));
            strncpy(pmsg->message, mess->valuestring, strlen(mess->valuestring));
        }
    }
    cJSON_Delete(json);
}


void formatMsgToJson_msgToClient(msgToClient msg, char *jsonContainer){
    char *buffer;
    cJSON *json = cJSON_CreateObject();
    if(json == NULL){
        printf("cjson create error\n");
        return;
    }
    cJSON_AddNumberToObject(json, "result", msg.result);
    cJSON_AddNumberToObject(json, "reasonCode", msg.reasonCode);
    cJSON_AddNumberToObject(json, "nameNum", msg.nameNum);
    cJSON_AddNumberToObject(json, "action", (int)msg.action);
    cJSON *names = cJSON_AddArrayToObject(json, "names");
    for(int i = 0; i <msg.nameNum; i++){
        cJSON_AddStringToObject(names, "name",msg.names[i]);
    } 
    cJSON_AddStringToObject(json, "message", msg.message);
    buffer = cJSON_Print(json);
    strncpy(jsonContainer, buffer, strlen(buffer));
    free(buffer);
    cJSON_Delete(json);
}


void parseJsonData_Client(msgToClient *pmsg, const char *jsonData){
    cJSON *json = cJSON_Parse(jsonData);
    if(json == NULL){
        printf("json is null\n");
        return ;
    }
    cJSON *result = cJSON_GetObjectItemCaseSensitive(json, "result");
    if(result != NULL){
        if(cJSON_IsNumber(result)){
            pmsg->result = result->valueint;
        }
    }
    cJSON *reasonCode = cJSON_GetObjectItemCaseSensitive(json, "reasonCode");
    if(reasonCode != NULL){
        if(cJSON_IsNumber(reasonCode)){
            pmsg->reasonCode = reasonCode->valueint;
        }
    }
    cJSON *nameNum = cJSON_GetObjectItemCaseSensitive(json, "nameNum");
    if(nameNum != NULL){
        if(cJSON_IsNumber(nameNum)){
            pmsg->nameNum = nameNum->valueint;
        }
    }
    cJSON *action = cJSON_GetObjectItemCaseSensitive(json, "action");
    if(action != NULL){
        if(cJSON_IsNumber(action)){
            pmsg->action = (ActionType_Server)action->valueint;
        }
    }
    cJSON *names = cJSON_GetObjectItemCaseSensitive(json, "names");
    cJSON *name = NULL;
    int i=0;
    pmsg->names = (char (*)[40])malloc(sizeof(char[40]) * pmsg->nameNum);
    memset(pmsg->names, 0, sizeof(char[40])*pmsg->nameNum);
    cJSON_ArrayForEach(name, names){
        if(cJSON_IsString(name) && name->valuestring != NULL){
            memset(pmsg->names[i], 0, 40);
            strncpy(pmsg->names[i], name->valuestring, strlen(name->valuestring));
            i++;
        }
    }

    cJSON *message = cJSON_GetObjectItemCaseSensitive(json, "message");
    if(message != NULL){
        if(cJSON_IsString(message) && message->valuestring != NULL){
            memset(pmsg->message, 0, sizeof(pmsg->message));
            strncpy(pmsg->message, message->valuestring, strlen(message->valuestring));
        }
    }
    cJSON_Delete(json);
}


void formatFileInfoToJson(FileInfo fileInfo, char * jsonContainer){
    char *buffer;
    cJSON *json = cJSON_CreateObject();
    if(json == NULL){
        printf("cjson create error\n");
        return;
    }
    cJSON_AddStringToObject(json, "name", fileInfo.name);
    cJSON_AddNumberToObject(json, "size", fileInfo.size);
    cJSON_AddNumberToObject(json, "blockSize", fileInfo.blockSize);
    cJSON_AddNumberToObject(json, "count", fileInfo.count);
    buffer = cJSON_Print(json);
    strncpy(jsonContainer, buffer, strlen(buffer));
    free(buffer);
    cJSON_Delete(json);
}

void parseJsonData_FileInfo(FileInfo *pfileInfo, const char *jsonData){
    cJSON *json = NULL;
    json = cJSON_Parse(jsonData);
    if(json == NULL){
        printf("json is NULL\n");
        return;
    }
 
    cJSON *name = cJSON_GetObjectItemCaseSensitive(json, "name");
    if(name != NULL){
        if(cJSON_IsString(name)){
           bzero(pfileInfo->name, sizeof(pfileInfo->name));
           strncpy(pfileInfo->name, name->valuestring, strlen(name->valuestring));
        }
    }
    cJSON *size = cJSON_GetObjectItemCaseSensitive(json, "size");
    if(size != NULL){
        if(cJSON_IsNumber(size)){
            pfileInfo->size = size->valueint;
        }
    }
    
    cJSON *blockSize = cJSON_GetObjectItemCaseSensitive(json, "blockSize");
    if(blockSize != NULL){
        if(cJSON_IsNumber(blockSize)){
            pfileInfo->blockSize = blockSize->valueint;
        }
    }
    cJSON *count = cJSON_GetObjectItemCaseSensitive(json, "count");
    if(count != NULL){
        if(cJSON_IsNumber(count)){
            pfileInfo->count = count->valueint;
        }
    }
    cJSON_Delete(json);
}

void formatPartOfFileInfoToJson(PartOfFile part, char *jsonContainer){
    char *buffer;
    cJSON *json = cJSON_CreateObject();
    if(json == NULL){
        printf("cjson create error\n");
        return;
    }
    cJSON_AddNumberToObject(json, "offset", part.offset);
    cJSON_AddNumberToObject(json, "seq", part.seq);
    cJSON_AddNumberToObject(json, "realsize", part.realsize);
    cJSON_AddStringToObject(json, "body", part.body);
    buffer = cJSON_Print(json);
    strncpy(jsonContainer, buffer, strlen(buffer));
    free(buffer);
    cJSON_Delete(json);
}


void parseJsonData_PartOfFile(PartOfFile *part, const char *jsonData){
    cJSON *json = NULL;
    json = cJSON_Parse(jsonData);
    if(json == NULL){
        printf("json is NULL\n");
        return;
    }
 
    cJSON *body = cJSON_GetObjectItemCaseSensitive(json, "body");
    if(body != NULL){
        if(cJSON_IsString(body)){
           bzero(part, sizeof(part->body));
           strncpy(part->body, body->valuestring, strlen(body->valuestring));
        }
    }
    cJSON *seq = cJSON_GetObjectItemCaseSensitive(json, "seq");
    if(seq != NULL){
        if(cJSON_IsNumber(seq)){
            part->seq = seq->valueint;
        }
    }
    
    cJSON *realsize = cJSON_GetObjectItemCaseSensitive(json, "realsize");
    if(realsize != NULL){
        if(cJSON_IsNumber(realsize)){
            part->realsize = realsize->valueint;
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

void showMsg_msgToClient(const msgToClient msg){
    printf("result: %d\n", msg.result);
    printf("reasonCode %d\n", msg.reasonCode);
    printf("nameNum %d\n", msg.nameNum);
    printf("action %d\n", msg.action);
    for(int i=0; i<msg.nameNum; i++){
        printf("name[%d]: %s\n", i, msg.names[i]);
    }
    printf("message: %s\n", msg.message);
}