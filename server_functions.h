#ifndef _server_
#define _server_

#include <stdlib.h>
#include <stdio.h>


#include "udp.h"

#define MAX_USERNAME_LEN 32
#define Max_clients 15

typedef struct client {
    char username[MAX_USERNAME_LEN];
    struct sockaddr_in addr;// contains IP + port
    struct client* next;  
}client;

client* add_c (char name[], struct sockaddr_in client_address, client* last);
client* find_name(client* head, char name[]);
client* find_node_before_c(client* head, client* c);
client* remove_c(client* c, client* head);
void de_all_list(client* head);
void print_all_connected(client* head);

#endif