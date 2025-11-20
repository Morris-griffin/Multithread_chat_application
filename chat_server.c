#include <stdio.h>
#include <stdlib.h>
#include "udp.h"
#include "server_functions.h"

char isconn[] = "conn";
char *request_type;
char *request_content;
int valid_input = 0 ;
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
            /*
            strcpy(server_response, "Hi, the server has received: ");
            strcat(server_response, client_request);
            
            strcat(server_response, "\n");
            */
            request_type = strtok(client_request, "$");
            
            request_content = strtok(NULL, "$");

            requesting_client_node = find_socket(clients_head,client_address);

            //if client request is connect to chat with given name
            if(requesting_client_node == NULL){
                if (strncmp(request_type, "conn" , 4) == 0 ){
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
            }
            else{

                if(strncmp(request_type, "conn" , 4) == 0 ){
                    strcpy(server_response,"you are already connected as ");
                    strcat(server_response, requesting_client_node->username);
                }

                else if (strncmp(request_type, "say" , 3) == 0){
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
                else if (strncmp(request_type, "disconn" ,7) == 0){
                    strcpy(server_response, "session finished - see you soon!\n");
                    clients_head = remove_c(requesting_client_node,clients_head);
                }
                else if (strncmp(request_type, "mute" , 4) == 0){
                    // block a client from this client
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
                else if (strncmp(request_type, "unmute" , 6) == 0){
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
                }
                else if (strncmp(request_type, "rename" , 6) == 0){
                    if(find_name(clients_head,request_content)==NULL){
                        strcpy(requesting_client_node->username, request_content);
                        strcpy(server_response,"You are now called: ");
                        strcat(server_response,request_content);
                        strcat(server_response,"\n");
                    }
                    else{
                        strcpy(server_response,"ERROR: that name is already taken\n");

                    }
                }
                else if (strncmp(request_type, "kick" , 4) == 0){
                    // check if admin and then remove specified chlient 
                }
                else{
                    strcpy(server_response, "Incorrect command\n");
                }
            }
        

            // This function writes back to the incoming client,
            // whose address is now available in client_address, 
            // through the socket at sd.
            // (See details of the function in udp.h)
            
            rc = udp_socket_write(sd, &client_address, server_response, BUFFER_SIZE);

            // Demo code (remove later)
            printf("Request served...\n");
        }
    }

    return 0;

}
