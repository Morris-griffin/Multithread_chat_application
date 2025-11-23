#include <stdio.h>
#include <stdlib.h>
#include "udp.h"
#include "server_functions.h"

char isconn[] = "conn";
char *request_type;
char *request_content;
int valid_input = 0 ;

sem_t read_sem, write_sem;
int reader_num = 0;


void read_lock() {
    sem_wait(&read_sem);      
    reader_num++;
    if (reader_num == 1) {
        // first reader blocks writers
        sem_wait(&write_sem);
    }
    sem_post(&read_sem);           // unlock reader_count

    // critical section
    //do_reading();
}
void read_unlock(){
    

    sem_wait(&read_sem);         // lock reader_count
    reader_num--;
    if (reader_num == 0) {
        // last reader unblocks writers
        sem_post(&write_sem);
    }
    sem_post(&read_sem);           // unlock reader_count
}


void write_lock(){
    sem_wait(&write_sem);  // wait until no readers and no other writer
    // critical section
    //do_writing();
}
void write_unlock(){    
    sem_post(&write_sem);      
}



int main(int argc, char *argv[])
{
    sem_init(&read_sem, 1, 1);
    sem_init(&write_sem, 1,1 );
    client** pointer_to_head_pointer = malloc(sizeof(client*));
    *pointer_to_head_pointer = NULL;
    
    
    int num_clients = 0;
    // This function opens a UDP socket,
    // binding it to all IP interfaces of this machine,
    // and port number SERVER_PORT
    // (See details of the function in udp.h)
    int sd = udp_socket_open(SERVER_PORT);

    assert(sd > -1);
    client *requesting_client_node;

    int num_readers;

    // Server main loop
    while (1) 
    {
        
        // Storage for request and response messages
        char client_request[BUFFER_SIZE], server_response[BUFFER_SIZE];

        // Demo code (remove later)
        printf("Server is listening on port %d\n", SERVER_PORT);
        printf("client list\n");
        print_all_connected(*pointer_to_head_pointer);

        // Variable to store incoming client's IP address and port
        struct sockaddr_in client_address;
    
        // This function reads incoming client request from
        // the socket at sd.
        // (See details of the function in udp.h)
        int rc = udp_socket_read(sd, &client_address, client_request, BUFFER_SIZE);
        
       
        
        printf("port %d\n", client_address.sin_port);
        //printf("the ip address is %d/n", client_address.sin_addr);
        // Successfully received an incoming request

        
        pthread_t t;

        struct response_thread_struct *thread_in = malloc(sizeof(response_thread_struct));

        thread_in -> client_request = malloc(rc);
        strcpy(thread_in->client_request,client_request);
        thread_in -> client_address = malloc(sizeof(struct sockaddr_in));
        *(thread_in -> client_address) = client_address;
        thread_in -> pointer_to_head_pointer = pointer_to_head_pointer;
        thread_in->sd = &sd;


        pthread_create(&t, NULL, response_thread, (void*)thread_in);
        pthread_detach(t);



    }

    free(pointer_to_head_pointer);

}











/*
int main(int argc, char *argv[])
{
    client* clients_head = NULL;
    int num_clients = 0;
    // This function opens a UDP socket,
    // binding it to all IP interfaces of this machine,
    // and port number SERVER_PORT
    // (See details of the function in udp.h)
    int sd = udp_socket_open(SERVER_PORT);

    assert(sd > -1);
    client *requesting_client_node;

    // Server main loop
    while (1) 
    {
        
        // Storage for request and response messages
        char client_request[BUFFER_SIZE], server_response[BUFFER_SIZE];

        // Demo code (remove later)
        printf("Server is listening on port %d\n", SERVER_PORT);
        printf("client list\n");
        print_all_connected(clients_head);

        // Variable to store incoming client's IP address and port
        struct sockaddr_in client_address;
    
        // This function reads incoming client request from
        // the socket at sd.
        // (See details of the function in udp.h)
        int rc = udp_socket_read(sd, &client_address, client_request, BUFFER_SIZE);
        
       
        
        printf("port %d\n", client_address.sin_port);
        //printf("the ip address is %d/n", client_address.sin_addr);
        // Successfully received an incoming request
        if (rc > 0)
        {
            // Demo code (remove later)
            
            strcpy(server_response, "Hi, the server has received: ");
            strcat(server_response, client_request);
            
            strcat(server_response, "\n");
            
            request_type = strtok(client_request, "$");
            
            request_content = strtok(NULL, "$");

            requesting_client_node = find_socket(clients_head,client_address);

            //if client request is connect to chat with given name
            if(requesting_client_node == NULL){
                if (strcmp(request_type, "conn") == 0 ){
                    if (num_clients < Max_clients){
                        if(find_name(clients_head,request_content)==NULL){
                            num_clients += 1;
                            clients_head = add_c(request_content,client_address,clients_head);
                            strcpy(server_response,"Welcome: ");
                            strcat(server_response,request_content);
                            strcat(server_response,"\n");
                            printf("%s", request_content);
                        }
                        else{
                            strcpy(server_response,"ERROR: that name is already taken\n");
                            
                        }
                    }
                    else {printf("Max Clients reached");};
                }
                else{
                    strcpy(server_response,"unrecognised user - use conn$ to connect and set name\n");
                }
                rc = udp_socket_write(sd, &client_address, server_response, BUFFER_SIZE);
            }
            else{

                if(strcmp(request_type, "conn") == 0 ){
                    strcpy(server_response,"you are already connected as ");
                    strcat(server_response, requesting_client_node->username);
                    rc = udp_socket_write(sd, &client_address, server_response, BUFFER_SIZE);
                }

                else if (strcmp(request_type, "say") == 0){
                    //broadcast to all clients
                    client* c = clients_head;
                    int i = 0;
                    strcpy(server_response,requesting_client_node->username);
                    strcat(server_response, ": ");
                    strcat(server_response, request_content);
                    strcat(server_response,"\n");
                    while(c != NULL){
                        if(find_name_in_blocked(c->block_list,requesting_client_node->username) == NULL){
                            printf("broadcast #%d to %s:%d\n",
                                i,
                                inet_ntoa(c->addr.sin_addr),
                                ntohs(c->addr.sin_port));

                            rc = udp_socket_write(sd, &(c->addr), server_response, BUFFER_SIZE);
                            printf("rc:  %d\n",rc);    
                        }



                        c= c->next;
                        i++;
                    }
                }
                else if (strcmp(request_type, "disconn") == 0){
                    strcpy(server_response, "session finished - see you soon!\n");
                    rc = udp_socket_write(sd, &client_address, server_response, BUFFER_SIZE);
                    clients_head = remove_c(requesting_client_node,clients_head);
                }
                else if (strcmp(request_type, "mute") == 0){
                    // block a client from this client
                    if(strcmp(request_content,requesting_client_node->username)==0){
                        strcpy(server_response,"ERROR: Trying to mute yourself\n");
                    }
                    else{
                        block_node* tmp = block_user(clients_head,requesting_client_node->block_list,request_content);
                        if(tmp == NULL){
                            strcpy(server_response,"ERROR: there is no ");
                            strcat(server_response,request_content);
                            strcat(server_response,"\n");

                        }
                        else{
                            requesting_client_node->block_list = tmp;
                            strcpy(server_response,"SUCCESS: blocked ");
                            strcat(server_response,request_content);
                            strcat(server_response,"\n");

                        }
                    }
                    rc = udp_socket_write(sd, &client_address, server_response, BUFFER_SIZE);
                }
                else if (strcmp(request_type, "unmute") == 0){
                    // remove the block of this client
                    block_node *unmute_user = find_name_in_blocked(requesting_client_node->block_list,request_content);
                    if(unmute_user==NULL){
                        strcpy(server_response,"ERROR: you are not currently blocking ");
                        strcat(server_response,request_content);
                        strcat(server_response,"\n");
                    }
                    else{
                        strcpy(server_response,"unblocking ");
                        strcat(server_response,request_content);
                        strcat(server_response,"\n");
                        requesting_client_node->block_list = remove_b(unmute_user,requesting_client_node->block_list);
                    }
                    rc = udp_socket_write(sd, &client_address, server_response, BUFFER_SIZE);
                }
                else if (strcmp(request_type, "rename") == 0){
                    if(find_name(clients_head,request_content)==NULL){


                        client* tmp = clients_head;

                        block_node* blocked;

                        while(tmp != NULL){
                            blocked = find_name_in_blocked(tmp->block_list,requesting_client_node->username);

                            if(blocked != NULL){
                                strcpy(blocked -> username, request_content);
                                strcpy(server_response, requesting_client_node->username);
                                strcat(server_response,"who is on your blocked list, has now changed name to ");
                                strcat(server_response,request_content);
                                strcat(server_response,"they will remain blocked\n");

                                rc = udp_socket_write(sd, &(tmp->addr), server_response, BUFFER_SIZE);




                            }




                            tmp = tmp->next;
                        }






                        strcpy(requesting_client_node->username, request_content);
                        strcpy(server_response,"You are now called: ");
                        strcat(server_response,request_content);
                        strcat(server_response,"\n");
                    }
                    else{
                        strcpy(server_response,"ERROR: that name is already taken\n");

                    }
                    rc = udp_socket_write(sd, &client_address, server_response, BUFFER_SIZE);
                }
                else if (strcmp(request_type, "kick") == 0){
                    // check if admin and then remove specified chlient
                    if(strcmp(request_content,requesting_client_node->username)==0){
                        strcpy(server_response,"ERROR: Trying to kick yourself\n");
                        rc = udp_socket_write(sd, &client_address, server_response, BUFFER_SIZE);
                    } 
                    else{
                        if(htons(requesting_client_node->addr.sin_port) == 6666){
                            client* kick_c;
                            kick_c = find_name(clients_head,request_content);
                            if(kick_c == NULL){
                                strcpy(server_response,"ERROR: ");
                                strcat(server_response,request_content);
                                strcat(server_response, " is not a member of the chat.\n");
                            }
                            else{

                                strcpy(server_response,"You have been removed from the chat\n");
                                rc = udp_socket_write(sd, &(kick_c->addr), server_response, BUFFER_SIZE);
                                clients_head = remove_c(kick_c,clients_head); // this isnt working





                                client* c = clients_head;


                                strcpy(server_response,request_content);
                                strcat(server_response," has been removed from the chat\n");

                                while(c != NULL){
                                    rc = udp_socket_write(sd, &(c->addr), server_response, BUFFER_SIZE);

                                    c = c -> next;
                                }

                            }
                        }
                        else{
                            strcpy(server_response,"You cannot kick, you are not admin\n");
                            rc = udp_socket_write(sd, &client_address, server_response, BUFFER_SIZE);
                        }
                    }

                }
                else if (strcmp(request_type, "sayto") ==0){
                    client* found;

                    char* recipient_name = strtok(request_content, " ");

            
                    char* msg = strtok(NULL, " ");

                    if(msg == NULL){
                        strcpy(server_response,"ERROR - wrong sayto input format\n");
                        rc = udp_socket_write(sd, &(requesting_client_node->addr), server_response, BUFFER_SIZE);

                    }
                    else{



                        printf("Searching for '%s'\n",recipient_name);
                        printf("wanting to say '%s'\n",msg);




                        found = find_name(clients_head,recipient_name);
                        if(strcmp(recipient_name,requesting_client_node->username) == 0){
                            strcpy(server_response, "cant private messag yourself\n");
                            rc = udp_socket_write(sd, &(found->addr), server_response, BUFFER_SIZE);
                        }
                        else if(found == NULL){
                            strcpy(server_response,"ERROR: user not found\n");
                            rc = udp_socket_write(sd, &(found->addr), server_response, BUFFER_SIZE);
                        }
                        else if (find_name_in_blocked(found->block_list,requesting_client_node->username) != NULL){
                            strcpy(server_response, "This user has blocked you");
                            rc = udp_socket_write(sd, &(found->addr), server_response, BUFFER_SIZE);
                        }
                        else{

                            printf("private message happening\n");
                            printf("to %s\n",recipient_name);







                            strcpy(server_response,requesting_client_node->username);
                            strcat(server_response, "(private with ");
                            strcat(server_response,recipient_name);
                            strcat(server_response,"): ");
                            strcat(server_response, msg);
                            strcat(server_response,"\n");
                            rc = udp_socket_write(sd, &(found->addr), server_response, BUFFER_SIZE);
                            rc = udp_socket_write(sd, &client_address, server_response, BUFFER_SIZE);
                            
                        }
                    }
                }
                else{
                    strcpy(server_response, "Incorrect command\n");
                }
                //rc = udp_socket_write(sd, &client_address, server_response, BUFFER_SIZE);
            }
        

            // This function writes back to the incoming client,
            // whose address is now available in client_address, 
            // through the socket at sd.
            // (See details of the function in udp.h)
            
            //rc = udp_socket_write(sd, &client_address, server_response, BUFFER_SIZE);

            // Demo code (remove later)
            printf("Request served...\n");
        }
    }

    return 0;

}
*/