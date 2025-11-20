#ifndef _server_
#define _server_

#include <stdlib.h>
#include <stdio.h>


#include "udp.h"

#define MAX_USERNAME_LEN 32
#define Max_clients 15

typedef struct block_node {
    char username[MAX_USERNAME_LEN];
    struct block_node* next;
}block_node;

typedef struct client {
    char username[MAX_USERNAME_LEN];
    struct sockaddr_in addr;// contains IP + port
    block_node *block_list;
    struct client* next;  
}client;

client* add_c (char name[], struct sockaddr_in client_address, client* last);
client* find_name(client* head, char name[]);
client* find_node_before_c(client* head, client* c);
client* remove_c(client* c, client* head);
void de_all_list(client* head);
void print_all_connected(client* head);
client* find_socket(client* head, struct sockaddr_in addr);








//////// for block list ///////////

block_node* block_user(client* client_head, block_node* block_list, char name[]);

block_node* find_name_in_blocked(block_node* head, char name[]);

block_node* find_node_before_name_blocked(block_node* head, block_node* b);

block_node* remove_b(block_node* b, block_node* head);


void de_all_b_list(block_node* head);

#endif