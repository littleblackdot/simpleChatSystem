#ifndef __MYNET_H__
#define __MYNET_H__

#include "lib_use.h"
#define SERVER_IPADDRESS "192.168.124.131"
#define SERVER_LISTENPORT 11234
#define SERVER_LISTEN_CAPACITY 10



int socketInit_TCP(int port, const char* addr);





#endif