#include <stdio.h>
#include <stdlib.h>
#include "udp.h"

#define CLIENT_PORT 10000
#define Max_name_length 30
#define ADMIN_PORT 6666
// client code

typedef struct w_thread_in{
    struct sockaddr_in *server_addr;
    char *client_request;
    int *sd;
    int* key_found;
    char* key; 
    struct sockaddr_in *this_port;
}w_thread_in;

typedef struct l_thread_in{
    int* port;
    int* key_found;
    char* key;
} l_thread_in;

void* client_listen(void* arg){
    l_thread_in *in_data = (l_thread_in*)arg;

    unsigned int key;

    char buffer[BUFFER_SIZE];

    struct sockaddr_in tmp;
    char* key_check;
    char* key_num;
    char session_key[15];




    while(1){
        int rc = udp_socket_read(*(in_data->port), &tmp, buffer, BUFFER_SIZE);
        if(!*(in_data->key_found)){
            key_check = strtok(buffer,"#");
            key_num = strtok(NULL,"#");
            if(strcmp(key_check,"key") == 0){
                key = (unsigned int) strtoul(key_num, NULL, 10);
                printf("Acquired token\n");
                *(in_data->key_found) = 1;


                snprintf(session_key, sizeof(session_key), "%u", key);


                strcpy(in_data -> key, session_key);


                *(in_data->key_found) = 1;


            }
            else{
                printf("%s",key_check);    
            }

            

        }
        else{
            if (rc > 0){
                if(strcmp(buffer,"$kill$\n") == 0){
                    *(in_data->key_found) = 0;
                    strcpy(in_data -> key, "0");

                }
                else{
                    printf("%s",buffer);
                }
            }
        }
    
    }
    return NULL;
}

void* client_speak(void* arg){
    int rc;
    w_thread_in input = *(w_thread_in*)arg;

    char output_request[BUFFER_SIZE];
    
    while (1){

        fgets(input.client_request, BUFFER_SIZE, stdin);


        // This function writes to the server (sends request) through the socket at sd.
        if(*(input.key_found)){

            if(strlen(input.client_request)>1){
                input.client_request[strlen(input.client_request)-1] = '\0';
                strcpy(output_request,input.key);
                strcat(output_request,"#");
                strcat(output_request,input.client_request);
                rc = udp_socket_write(*input.sd, input.server_addr, output_request, BUFFER_SIZE);
                
                    if (rc > 0)
                    {
                        
                    }
            }
        } 
        else{

            if(strlen(input.client_request)>1){
                input.client_request[strlen(input.client_request)-1] = '\0';
                strcpy(output_request,"11#");

                strcat(output_request,input.client_request);
                rc = udp_socket_write(*input.sd, input.server_addr, output_request, BUFFER_SIZE);
                    if (rc > 0)
                    {
                        
                    }
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

    
    struct sockaddr_in my_port;
    socklen_t my_port_size = sizeof(my_port);

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
    int rc = set_socket_addr(&server_addr, "10.255.255.254", SERVER_PORT);

    // Storage for request and response messages

    int key_acquired = 0;

    char key_str[15];
    
    

    // client information
    char client_name[Max_name_length];

    pthread_t listener_thread, writer_thread;

    struct sockaddr_in port; 


    w_thread_in write_thread_in;
    write_thread_in.server_addr = &server_addr;
    write_thread_in.client_request = client_request;
    write_thread_in.sd = &sd;
    write_thread_in.key_found = &key_acquired;
    write_thread_in.key = key_str;
    write_thread_in.this_port = &port;

    int x = pthread_create(&writer_thread,NULL,client_speak,(void*)&write_thread_in);
    if(x != 0){
        printf("writer thread creation failed\n");
    }


    l_thread_in listen_input;
    listen_input.port = &sd;
    listen_input.key_found = &key_acquired;
    listen_input.key = key_str;
    x = pthread_create(&listener_thread,NULL,client_listen,(void*)&listen_input);
    if(x != 0){
        printf("listener thread creation failed\n");
    }

    pthread_join(listener_thread, NULL);
    pthread_join(writer_thread, NULL);



    return 0;
}


