#include <stdio.h>
#include <stdlib.h>
#include "udp.h"

//need a data type that ties users and there ip addresses 
#define MAX_USERNAME_LEN 32
#define Max_clients 15

typedef struct Client {
    char username[MAX_USERNAME_LEN];
    struct sockaddr_in addr;// contains IP + port
    Client* next;  
}Client;
char isconn[] = "conn";
char *request_type[10];
char *request_content [BUFFER_SIZE];
int main(int argc, char *argv[])
{
    Client clients[Max_clients];
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
            
            strcat(server_response, '\n');
            //split the request by $ sign 

            *request_type = strtok(client_request, '$');
            *request_content = strtok(NULL, '$');

            //if client request is connect to chat with given name
            if (strncmp(request_type, "conn" , 4) == 0 ){
                    if (num_clients < Max_clients){
                        num_clients += 1;
                        Client *c = &clients[num_clients];
                        strncpy(c->username, request_content, MAX_USERNAME_LEN);
                        c->username[MAX_USERNAME_LEN - 1] = '\0';
                        c->addr = client_address;}
                    else {printf("Max Clients reached");};
            };
            
            

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
