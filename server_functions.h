#ifndef _server_
#define _server_
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <time.h>
#include "udp.h"

#define MAX_USERNAME_LEN 32
#define MAX_CLIENTS 15

typedef struct block_node {
    char username[MAX_USERNAME_LEN];
    struct block_node* next;
}block_node;
typedef struct NodeH {
    char data[100]; 
    struct NodeH *next;
} NodeH;

typedef struct {
    NodeH *head;
    NodeH *tail;
    int capacity;
    int count;
}fixedlistH;


typedef struct client {
    char username[MAX_USERNAME_LEN];
    struct sockaddr_in addr;
    time_t time;
    int heap_index;
    block_node *block_list;
    struct client* next;  
}client;

typedef struct response_thread_struct{
    char* client_request;
    client **pointer_to_head_pointer;
    struct sockaddr_in *client_address;
    int* sd;
    int* key;
    fixedlistH *listh;
    struct client_heap *heap;
    struct client_heap *pong;
}response_thread_struct;

typedef struct client_heap{
    client* client_pointer[MAX_CLIENTS];
    int connected_clients;
} client_heap;

client* add_c (char name[], struct sockaddr_in client_address, client* last);
client* find_name(client* head, char name[]);
client* find_node_before_c(client* head, client* c);
client* remove_c(client* c, client* head);
void de_all_list(client* head);
void print_all_connected(client* head);
client* find_socket(client* head, struct sockaddr_in addr);

void* response_thread(void* arg);


//////// for block list ///////////

block_node* block_user(client* client_head, block_node* block_list, char name[]);

block_node* find_name_in_blocked(block_node* head, char name[]);

block_node* find_node_before_name_blocked(block_node* head, block_node* b);

block_node* remove_b(block_node* b, block_node* head);


void de_all_b_list(block_node* head);


////////// locks ///////

extern sem_t read_sem, write_sem, write_blocker, writer_priority_block, h_lock, heaps_lock;
extern int reader_num;
extern int writer_num;

void read_lock();
void read_unlock();


void write_lock();
void write_unlock();


void init_heap(client_heap* heap);


void add_to_heap(client* c, client_heap* heap);

void move_down(int index, client_heap* heap);

void remove_from_heap(client_heap* heap, int index);

void print_heap(client_heap* heap,int index);
char* whitespace_trim(char* start);




#endif
