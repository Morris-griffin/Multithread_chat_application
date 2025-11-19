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

    return head;

}

client* find_c(client* head, char name[]){
    if(head == NULL){
        return NULL;
    }

    if(strcmp(head->username,name) == 0){
        return head;
    }

    else{
        return find_c(head->next, name);
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
        free(head);
        return new_head;

    }
    else{
        client* downstream_node = c->next;
        free(c);
        node_before_c -> next = downstream_node;
        return head;
    }

}

void de_all_list(client* head){
    if(head != NULL){
        de_all_list(head->next);
        free(head);
    }
}