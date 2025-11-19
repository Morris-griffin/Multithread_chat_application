#ifndef _server_
#define _server_


#include "udp.h"

#define MAX_USERNAME_LEN 32
#define Max_clients 15

typedef struct client {
    char username[MAX_USERNAME_LEN];
    struct sockaddr_in addr;// contains IP + port
    struct client* next;  
}client;

#endif