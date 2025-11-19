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
            //split the request by $ sign 
            if (strchr(client_request, '$')!= 0 ){
                valid_input = 1;
            }
            request_type = strtok(client_request, "$");
            
            request_content = strtok(NULL, "$");

            //if client request is connect to chat with given name
            if (valid_input = 0){
                printf("Incorrect syntax\n");
                strcat(server_response, "Incorrect input syntax");
                strcat(server_response, "\n");
            }
            else if (strncmp(request_type, "conn" , 4) == 0 ){
                    if (num_clients < Max_clients){
                        num_clients += 1;
                        clients_head = add_c(request_content,client_address,clients_head);
                        printf("%s", request_content);}
                    else {printf("Max Clients reached");};
            }
            else if (strncmp(request_type, "say" , 3) == 0){
                //broadcast to all clients
            }
            else if (strncmp(request_type, "disconn" ,7) == 0){
                strcpy(server_response, "kill");
            }
            else if (strncmp(request_type, "mute" , 4) == 0){
                // block a client from this client
            }
            else if (strncmp(request_type, "unmute" , 6) == 0){
                // remove the block of this client
            }
            else if (strncmp(request_type, "rename" , 6) == 0){
                // change the clients name 
            }
            else if (strncmp(request_type, "kick" , 4) == 0){
                // check if admin and then remove specified chlient 
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
