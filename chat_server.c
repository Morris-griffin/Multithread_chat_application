#include <stdio.h>
#include <stdlib.h>
#include "udp.h"
#include "server_functions.h"

char isconn[] = "conn";
char *request_type;
char *request_content;
int valid_input = 0 ;



void initlist(fixedlistH *list){
    list -> head = NULL;
    list -> tail = NULL;
    list -> capacity = 15;
    list -> count = 0;
}

int main(int argc, char *argv[])
{
    sem_init(&read_sem, 0, 1);
    sem_init(&write_sem, 0,1 );
    sem_init(&write_blocker, 0, 1);
    sem_init(&writer_priority_block, 0,1 );
    sem_init(&h_lock,0,1);
    sem_init(&heaps_lock, 0,1 );
    client** pointer_to_head_pointer = malloc(sizeof(client*));
    *pointer_to_head_pointer = NULL;
    
    srand(time(NULL));
    unsigned int key = rand();
    char key_str[20];
    snprintf(key_str, sizeof(key_str), "%u", key);
    

    
    int num_clients = 0;
    // This function opens a UDP socket,
    // binding it to all IP interfaces of this machine,
    // and port number SERVER_PORT
    // (See details of the function in udp.h)
    int sd = udp_socket_open(SERVER_PORT);

    assert(sd > -1);
    client *requesting_client_node;

    int num_readers;


    ////// FOR CHAT MEMORY
    fixedlistH listh ;
    initlist (&listh);

    ////////////////    
    client_heap heap;

    client_heap pong_heap;
    
    init_heap(&heap);

    init_heap(&pong_heap);

    // Server main loop
    while (1) 
    {
        char client_request[BUFFER_SIZE], server_response[BUFFER_SIZE];        
        printf("Server is listening on port %d\n", SERVER_PORT);       
        print_all_connected(*pointer_to_head_pointer);
        // Variable to store incoming client's IP address and port
        struct sockaddr_in client_address;
    
        // This function reads incoming client request from
        // the socket at sd.
        // (See details of the function in udp.h)
        int rc = udp_socket_read(sd, &client_address, client_request, BUFFER_SIZE);
        
        printf("port %d\n", client_address.sin_port);


        
        pthread_t t;

        struct response_thread_struct *thread_in = malloc(sizeof(response_thread_struct));

        thread_in -> client_request = malloc(rc);
        strcpy(thread_in->client_request,client_request);
        thread_in -> client_address = malloc(sizeof(struct sockaddr_in));
        *(thread_in -> client_address) = client_address;
        thread_in -> pointer_to_head_pointer = pointer_to_head_pointer;
        thread_in->sd = &sd;
        thread_in->key = &key;
        thread_in-> listh = &listh;
        thread_in->heap = &heap;
        thread_in->pong = &pong_heap;

        pthread_create(&t, NULL, response_thread, (void*)thread_in);
        pthread_detach(t);
    }
    free(pointer_to_head_pointer);
}
