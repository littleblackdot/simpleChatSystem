#ifndef __DATAPARSE_H__
#define __DATAPARSE_H__

#include "lib_use.h"

#include "cJSON.h"
#include "myType.h"




void formatMsgToJson_msgToServer(const msgToServer msg, char * jsonContainer);
void parseJsonData_Server(msgToServer *pmsg, const char *jsonData);
void showMsg_msgToServer(const msgToServer msg);

void formatMsgToJson_msgToClient(const msgToClient msg, char * jsonContainer);
void parseJsonData_Client(msgToClient *pmsg, const char *jsonData);         
void showMsg_msgToClient(const msgToClient msg);


#endif