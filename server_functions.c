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