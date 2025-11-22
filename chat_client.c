#include <stdio.h>
#include "udp.h"

#define CLIENT_PORT 10000
#define Max_name_length 30
#define ADMIN_PORT 6666
// client code

typedef struct w_thread_in{
    struct sockaddr_in *server_addr;
    char *client_request;
    int *sd;
}w_thread_in;

void* client_listen(void* arg){
    int port = *(int*)arg;

    char buffer[BUFFER_SIZE];

    struct sockaddr_in tmp;

    while(1){
        int rc = udp_socket_read(port, &tmp, buffer, BUFFER_SIZE);
        if (rc > 0){
            printf("%s",buffer);
        }
    }
    return NULL;
}

void* client_speak(void* arg){
    int rc;
    w_thread_in input = *(w_thread_in*)arg;
    
    while (1){

        fgets(input.client_request, BUFFER_SIZE, stdin);

        // This function writes to the server (sends request) through the socket at sd.
        if(strlen(input.client_request)>1){
            input.client_request[strlen(input.client_request)-1] = '\0';
            rc = udp_socket_write(*input.sd, input.server_addr, input.client_request, BUFFER_SIZE);
                if (rc > 0)
                {
                    
                }
        }
    
    }

    return NULL;




}


int main(int argc, char *argv[])
{
    int client_type = 1;
    int status = 1 ;
    char client_request[BUFFER_SIZE], server_response[BUFFER_SIZE];
    int sd;

    while(client_type){

        printf("Select: Join as admin? y/n: \n");
        fgets(client_request, BUFFER_SIZE, stdin);

        // This function opens a UDP socket,
        // binding it to all IP interfaces of this machine,
        // and port number CLIENT_PORT.
        // (See details of the function in udp.h)

        if(strcmp(client_request,"y\n")==0){
            client_type=0;
            sd = udp_socket_open(ADMIN_PORT);
            printf("Loaded as admin client, you can kick people, now connect with conn$ \n");

        }
        else if(strcmp(client_request,"n\n") == 0){
            client_type=0;
            sd = udp_socket_open(0);
            printf("Loaded as standard client, now connect with conn$ \n");

        }
        else{
            printf("ERROR: INVALID INPUT \n");
        }
    }





    // Variable to store the server's IP address and port
    // (i.e. the server we are trying to contact).
    // Generally, it is possible for the responder to be
    // different from the server requested.
    // Although, in our case the responder will
    // always be the same as the server.
    struct sockaddr_in server_addr, responder_addr;

    // Initializing the server's address.
    // We are currently running the server on localhost (127.0.0.1).
    // You can change this to a different IP address
    // when running the server on a different machine.
    // (See details of the function in udp.h)
    int rc = set_socket_addr(&server_addr, "127.0.0.1", SERVER_PORT);

    // Storage for request and response messages

    
    

    // client information
    char client_name[Max_name_length];

    pthread_t listener_thread, writer_thread;


    w_thread_in write_thread_in;
    write_thread_in.server_addr = &server_addr;
    write_thread_in.client_request = client_request;
    write_thread_in.sd = &sd;

    int x = pthread_create(&writer_thread,NULL,client_speak,(void*)&write_thread_in);
    if(x != 0){
        printf("writer thread creation failed\n");
    }


    x = pthread_create(&listener_thread,NULL,client_listen,(void*)&sd);
    if(x != 0){
        printf("listener thread creation failed\n");
    }

    pthread_join(listener_thread, NULL);
    pthread_join(writer_thread, NULL);


    return 0;
}


