#include "server_functions.h"

client* add_c (char name[], struct sockaddr_in client_address, client* last){
    client* head = malloc(sizeof(client));

    if (head == NULL) {
        printf("malloc failed\n");
        exit(1);  // malloc failed
    }
    strncpy(head -> username, name, MAX_USERNAME_LEN);
    (head -> username)[MAX_USERNAME_LEN-1] = '\0';
    head->addr = client_address;
    head->next = last;
    head->block_list = NULL;

    return head;

}

client* find_name(client* head, char name[]){
    if(head == NULL){
        return NULL;
    }

    if(strcmp(head->username,name) == 0){
        return head;
    }

    else{
        return find_name(head->next, name);
    }
}

client* find_socket(client* head, struct sockaddr_in addr){
    if(head == NULL){
        return NULL;
    }

    if(head->addr.sin_addr.s_addr == addr.sin_addr.s_addr &&
            head->addr.sin_port == addr.sin_port){
        return head;
    }

    else{
        return find_socket(head->next, addr);
    }
}

client* find_node_before_c(client* head, client* c){
    if(c==head){
        return NULL;
    }

    if(head->next == c){
        return head;
    }
    else{
        return find_node_before_c(head->next, c);
    }
}

client* remove_c(client* c, client* head){
    client* node_before_c = find_node_before_c(head,c);

    if(node_before_c == NULL){
        client* new_head = head->next;
        de_all_b_list(head->block_list);
        free(head);
        return new_head;

    }
    else{
        client* downstream_node = c->next;
        de_all_b_list(c->block_list);
        free(c);
        node_before_c -> next = downstream_node;
        return head;
    }

}

void de_all_list(client* head){
    if(head != NULL){
        de_all_list(head->next);
        de_all_b_list(head->block_list);
        free(head);
    }
}

void print_all_connected(client* head){
    while(head != NULL){
        printf(head -> username, SERVER_PORT);
        printf("port is %d\n", head->addr.sin_port);
        head = head -> next;
    }
}

block_node* block_user(client* client_head, block_node* block_list, char name[]){
    if(find_name(client_head,name) != NULL){
    
        
        block_node* head = malloc(sizeof(block_node));

        if (head == NULL) {
            printf("malloc failed\n");
            exit(1);  // malloc failed
        }

        head -> next = block_list;
        strncpy(head -> username, name, MAX_USERNAME_LEN);
        (head -> username)[MAX_USERNAME_LEN-1] = '\0';

        return head;
    }
    else{
        return NULL; //name doesnt exist
    }
}


NodeH* createNode(char* value) {
    NodeH *nodeH = malloc(sizeof(NodeH));
    if (!nodeH) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    strcpy(nodeH-> data, value);
    nodeH->next = NULL;
    return nodeH;
}

// Remove the oldest element (from the head)
void removeOldest(fixedlistH *list) {
    if (list->head == NULL) return;

    NodeH *temp = list->head;
    list->head = list->head->next;

    // If list becomes empty, tail must also be NULL
    if (list->head == NULL) {
        list->tail = NULL;
    }

    free(temp);
    list->count--;

}

// Add a new element, maintaining max size of 30
void addtolist(fixedlistH *list, char* name, char* message) {
    // If already at max size, drop the oldest
    char value [100];
    strcpy(value, name);
    strcat(value, ": ");
    strcat(value, message);

    printf("adding %s to history \n", value);
    if (list->count == list->capacity) {
        removeOldest(list);
    }

    NodeH *nodeH = createNode(value);

    if (list->tail == NULL) {
        // List is empty
        list->head = nodeH;
        list->tail = nodeH;
    } else {
        list->tail->next = nodeH;
        list->tail = nodeH;
    }
    printf("countinc\n");
    list->count++;
}

//////// for block list ///////////



block_node* find_name_in_blocked(block_node* head, char name[]){
    if(head == NULL){
        return NULL;
    }

    if(strcmp(head->username,name) == 0){
        return head;
    }

    else{
        return find_name_in_blocked(head->next, name);
    }
}

block_node* find_node_before_name_blocked(block_node* head, block_node* b){
    if(b==head){
        return NULL;
    }

    if(head->next == b){
        return head;
    }
    else{
        return find_node_before_name_blocked(head->next, b);
    }
}

block_node* remove_b(block_node* b, block_node* head){
    block_node* node_before_b = find_node_before_name_blocked(head,b);

    if(node_before_b == NULL){
        block_node* new_head = head->next;
        free(head);
        return new_head;

    }
    else{
        block_node* downstream_node = b->next;
        free(b);
        node_before_b -> next = downstream_node;
        return head;
    }

}

void de_all_b_list(block_node* head){
    if(head != NULL){
        de_all_b_list(head->next);
        free(head);
    }
}



/////////////////////////////////////////////
/////////////////// locks ///////////////////
/////////////////////////////////////////////
sem_t read_sem, write_sem, write_blocker, writer_priority_block;

int reader_num = 0;
int writer_num = 0;



void read_lock() {
    sem_wait(&writer_priority_block);
    sem_wait(&read_sem); 

     
    reader_num++;
    if (reader_num == 1) {
        // first reader blocks writers
        sem_wait(&write_sem);
    }
    sem_post(&writer_priority_block);
    sem_post(&read_sem);           // unlock reader_count

    // critical section
    //do_reading();
}

void read_unlock() {
    sem_wait(&read_sem);         // lock reader_count
    reader_num--;
    if (reader_num == 0) {
        // last reader unblocks writers
        sem_post(&write_sem);
    }
    sem_post(&read_sem);           // unlock reader_count
}


void write_lock(){
    

    sem_wait(&write_blocker);

    writer_num++;
    if(writer_num==1){
        sem_wait(&writer_priority_block);
    }

    


    sem_post(&write_blocker);

    sem_wait(&write_sem);  // wait until no readers and no other writer
    // critical section
    //do_writing();


}
void write_unlock(){    
    sem_wait(&write_blocker);
    sem_post(&write_sem);
    writer_num--;
    if(writer_num==0){
        sem_post(&writer_priority_block);
    }
    sem_post(&write_blocker);      
}

//////////////////////////////////////
//////////////// auth ////////////////
////////////////////////////////////



void* response_thread(void* arg){
    response_thread_struct *thread_input = (response_thread_struct*)arg;
    char* client_request = thread_input -> client_request;
    client** pointer_to_head_pointer = thread_input -> pointer_to_head_pointer;
    struct sockaddr_in *client_address = thread_input -> client_address;

    int sd = *(thread_input->sd);
    int session_key = *(thread_input->key);

    ///////////// msg history inputs
    fixedlistH *list = thread_input -> listh;
    //////////////
    
    int rc;

    client *requesting_client_node;

    char* token;
    char *request_type;
    char *request_content;

    char server_response[BUFFER_SIZE];

    token = strtok(client_request,"#");
    
    request_type = strtok(NULL, "$");
            
    request_content = strtok(NULL, "$");

    if(request_type!=NULL){




                            //if client request is connect to chat with given name

                                    if((atoi(token)) != session_key){
                                        if (strcmp(request_type, "conn") == 0 ){
                                            write_lock();
                                            if(find_name(*pointer_to_head_pointer,request_content)==NULL){
                                                *pointer_to_head_pointer = add_c(request_content,*client_address,*pointer_to_head_pointer);
                                                
                                                
                                                
                                                
                                                strcpy(server_response,"key#");
                                                char tmp[15];
                                                snprintf(tmp, sizeof(tmp), "%u\n", session_key);
                                                strcat(server_response,tmp);
                                                rc = udp_socket_write(sd, client_address, server_response, BUFFER_SIZE);
                                                
                                                //PRINT HISTORY
                                                NodeH *curr = list -> head;
                                                /*printf("getting there? %d \n", listH.count);*/
                                                for (int i = 0 ; i < list -> count; i++){
                                                    printf("history");
                                                    strcpy(server_response, curr->data);
                                                    strcat(server_response,"\n");
                                                    rc = udp_socket_write(sd, client_address, server_response, BUFFER_SIZE);
                                                    curr = curr->next;
                                                }
                       
                                                strcpy(server_response,"Welcome: ");
                                                strcat(server_response,request_content);
                                                strcat(server_response,"\n");
                                            }
                                            else{
                                                
                                                strcpy(server_response,"ERROR: that name is already taken\n");
                                                printf("that name is taken\n");
                                                
                                            }

                                        }
                                        else{
                                            strcpy(server_response,"You are an unauthorised user - you must use conn$ to connect, set name, and get a unique access token\n");
                                        }
                                        printf("%s\n", server_response);
                                        rc = udp_socket_write(sd, client_address, server_response, BUFFER_SIZE);
                                        write_unlock();
                                    }
                                    else{

                                        if(strcmp(request_type, "conn") == 0 ){
                                            read_lock();
                                            requesting_client_node = find_socket(*pointer_to_head_pointer,*client_address);
                                            read_lock();

                                            strcpy(server_response,"you are already connected as ");
                                            strcat(server_response, requesting_client_node->username);
                                            rc = udp_socket_write(sd, client_address, server_response, BUFFER_SIZE);
                                            
                                        }

                                        else if (strcmp(request_type, "say") == 0){
                                            read_lock();
                                            requesting_client_node = find_socket(*pointer_to_head_pointer,*client_address);
                                            //broadcast to all clients
                                            client* c = *pointer_to_head_pointer;
                                            int i = 0;
                                            strcpy(server_response,requesting_client_node->username);
                                            strcat(server_response, ": ");
                                            strcat(server_response, request_content);
                                            strcat(server_response,"\n");
                                            while(c != NULL){
                                                if(find_name_in_blocked(c->block_list,requesting_client_node->username) == NULL){

                                                    rc = udp_socket_write(sd, &(c->addr), server_response, BUFFER_SIZE); 
                                                }



                                                c= c->next;
                                                i++;
                                            }
                                            //put in list here
                                            addtolist(list,requesting_client_node -> username, request_content);
                                            read_unlock();
                                        }
                                        else if (strcmp(request_type, "disconn") == 0){
                                            write_lock();
                                            requesting_client_node = find_socket(*pointer_to_head_pointer,*client_address);
                                            strcpy(server_response, "session finished - see you soon!\n");
                                            rc = udp_socket_write(sd, client_address, server_response, BUFFER_SIZE);
                                            strcpy(server_response,"$kill$\n");
                                            rc = udp_socket_write(sd, client_address, server_response, BUFFER_SIZE);
                                            *pointer_to_head_pointer = remove_c(requesting_client_node,*pointer_to_head_pointer);
                                            write_lock();
                                        }
                                        else if (strcmp(request_type, "mute") == 0){
                                            // block a client from this client
                                            write_lock();
                                            requesting_client_node = find_socket(*pointer_to_head_pointer,*client_address);
                                            if(strcmp(request_content,requesting_client_node->username)==0){
                                                strcpy(server_response,"ERROR: Trying to mute yourself\n");
                                            }
                                            else{
                                                block_node* tmp = block_user(*pointer_to_head_pointer,requesting_client_node->block_list,request_content);
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
                                            write_unlock();
                                            rc = udp_socket_write(sd, client_address, server_response, BUFFER_SIZE);
                                        }
                                        else if (strcmp(request_type, "unmute") == 0){
                                            write_lock();
                                            requesting_client_node = find_socket(*pointer_to_head_pointer,*client_address);
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
                                            write_unlock();
                                            rc = udp_socket_write(sd, client_address, server_response, BUFFER_SIZE);
                                        }
                                        else if (strcmp(request_type, "rename") == 0){
                                            write_lock();
                                            requesting_client_node = find_socket(*pointer_to_head_pointer,*client_address);
                                            if(find_name(*pointer_to_head_pointer,request_content)==NULL){


                                                client* tmp = *pointer_to_head_pointer;

                                                block_node* blocked;

                                                while(tmp != NULL){
                                                    blocked = find_name_in_blocked(tmp->block_list,requesting_client_node->username);

                                                    if(blocked != NULL){
                                                        strcpy(blocked -> username, request_content);
                                                        strcpy(server_response, requesting_client_node->username);
                                                        strcat(server_response," who is on your blocked list, has now changed name to ");
                                                        strcat(server_response,request_content);
                                                        strcat(server_response," they will remain blocked\n");

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
                                            write_unlock();
                                            rc = udp_socket_write(sd, client_address, server_response, BUFFER_SIZE);
                                        }
                                        else if (strcmp(request_type, "kick") == 0){
                                            write_lock();
                                            requesting_client_node = find_socket(*pointer_to_head_pointer,*client_address);
                                            // check if admin and then remove specified chlient
                                            if(strcmp(request_content,requesting_client_node->username)==0){
                                                strcpy(server_response,"ERROR: Trying to kick yourself\n");
                                                rc = udp_socket_write(sd, client_address, server_response, BUFFER_SIZE);
                                            } 
                                            else{
                                                if(htons(requesting_client_node->addr.sin_port) == 6666){
                                                    client* kick_c;
                                                    kick_c = find_name(*pointer_to_head_pointer,request_content);
                                                    if(kick_c == NULL){
                                                        strcpy(server_response,"ERROR: ");
                                                        strcat(server_response,request_content);
                                                        strcat(server_response, " is not a member of the chat.\n");
                                                    }
                                                    else{

                                                        strcpy(server_response,"You have been removed from the chat\n");
                                                        rc = udp_socket_write(sd, &(kick_c->addr), server_response, BUFFER_SIZE);
                                                        strcpy(server_response,"$kill$\n");
                                                        rc = udp_socket_write(sd, &(kick_c->addr), server_response, BUFFER_SIZE);
                                                        *pointer_to_head_pointer = remove_c(kick_c,*pointer_to_head_pointer); // this isnt working





                                                        client* c = *pointer_to_head_pointer;


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
                                                    rc = udp_socket_write(sd, client_address, server_response, BUFFER_SIZE);
                                                }
                                            }
                                            write_unlock();

                                        }
                                        else if (strcmp(request_type, "sayto") ==0){
                                            read_lock();
                                            requesting_client_node = find_socket(*pointer_to_head_pointer,*client_address);
                                            client* found;

                                            char* recipient_name = strtok(request_content, " ");

                                    
                                            char* msg = recipient_name + strlen(recipient_name) + 1;  // points right after '\0'
                                            if (*msg == '\0') {
                                                msg = NULL;  // no message, only recipient
                                            }



                                            if(msg == NULL){
                                                strcpy(server_response,"ERROR - wrong sayto input format\n");
                                                rc = udp_socket_write(sd, &(requesting_client_node->addr), server_response, BUFFER_SIZE);

                                            }
                                            else{






                                                found = find_name(*pointer_to_head_pointer,recipient_name);
                                                if(strcmp(recipient_name,requesting_client_node->username) == 0){
                                                    strcpy(server_response, "cant private messag yourself\n");
                                                    rc = udp_socket_write(sd, &(requesting_client_node->addr), server_response, BUFFER_SIZE);
                                                }
                                                else if(found == NULL){
                                                    strcpy(server_response,"ERROR: user not found\n");
                                                    rc = udp_socket_write(sd, &(requesting_client_node->addr), server_response, BUFFER_SIZE);
                                                }
                                                else if (find_name_in_blocked(found->block_list,requesting_client_node->username) != NULL){
                                                    strcpy(server_response, "This user has blocked you\n");
                                                    rc = udp_socket_write(sd, &(requesting_client_node->addr), server_response, BUFFER_SIZE);
                                                }
                                                else{









                                                    strcpy(server_response,requesting_client_node->username);
                                                    strcat(server_response, "(private with ");
                                                    strcat(server_response,recipient_name);
                                                    strcat(server_response,"): ");
                                                    strcat(server_response, msg);
                                                    strcat(server_response,"\n");
                                                    rc = udp_socket_write(sd, &(found->addr), server_response, BUFFER_SIZE);
                                                    rc = udp_socket_write(sd, client_address, server_response, BUFFER_SIZE);
                                                    
                                                }
                                            }
                                            read_unlock();
                                        }
                                        else{

                                            strcpy(server_response, "Incorrect command\n");
                                            rc = udp_socket_write(sd, client_address, server_response, BUFFER_SIZE);
                                        }
                                        //rc = udp_socket_write(sd, &client_address, server_response, BUFFER_SIZE);
                                    }
                        

                            // This function writes back to the incoming client,
                            // whose address is now available in client_address, 
                            // through the socket at sd.
                            // (See details of the function in udp.h)
            
            //rc = udp_socket_write(sd, &client_address, server_response, BUFFER_SIZE);
    }
    else{
        strcpy(server_response, "ERROR: You do not have acces token, start your conection request with conn$\n");
        rc = udp_socket_write(sd, client_address, server_response, BUFFER_SIZE);
        


    }



            // Demo code (remove later)
    printf("Request served...\n");
    free(client_request);
    free(client_address);
    free(thread_input);

    return NULL;
}



