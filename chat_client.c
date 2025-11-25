#include <stdio.h>
#include "udp.h"

#include <ncurses.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <stdio.h>


#define CLIENT_PORT 10000
#define Max_name_length 30
#define ADMIN_PORT 6666
#define max_history 20
#define max_input 30
// client code
typedef struct interface_dimensions {
    int rows, cols;
    int input_height ; // one line + border
    int chat_height ;
    WINDOW *chat_win;
    WINDOW *input_win;
    // Input buffer
    char input_buf[max_input];
    int input_len;
} interface_dimensions;

interface_dimensions nwindow;

typedef struct w_thread_in{
    struct sockaddr_in *server_addr;
    char *client_request;
    int *sd;
}w_thread_in;

void* client_listen(void* arg){
    int port = *(int*)arg;

    char buffer[BUFFER_SIZE];

    struct sockaddr_in tmp;
   
    char* thread_history [20];

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
    
    nwindow.input_len = 0;

    while (1){
        strcpy(input.client_request, refreshw());

        //fgets(input.client_request, BUFFER_SIZE, stdin);

        // This function writes to the server (sends request) through the socket at sd.
        if(strlen(input.client_request)>1){
            input.client_request[strlen(input.client_request)-1] = '\0';
            rc = udp_socket_write(*input.sd, input.server_addr, input.client_request, BUFFER_SIZE); //could do strlen(client_request)
                if (rc > 0)
                {
                    memset(nwindow.input_buf, 0, max_input);
                    nwindow.input_len = 0;
                }
        }
    
    }

    return NULL;




}

char* refreshw(){



    // For select()
    int fd_stdin = fileno(stdin);

    //handle keyboard inputs 
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(fd_stdin, &readfds);//reading from ui

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000;
    int ret = select(fd_stdin + 1, &readfds, NULL, NULL, &tv);

    int ret = select(fd_stdin + 1, &readfds, NULL, NULL, &tv);
    // For real socket: use max_fd + 1 above

    wmove(nwindow.chat_win, nwindow.chat_height - 2, 1);
            
    wprintw(nwindow.chat_win, "You: %s\n", nwindow.input_buf);
    
    wrefresh(nwindow.chat_win);

    // --- Handle keyboard input ---
    
    if (ret > 0 && FD_ISSET(fd_stdin, &readfds)) {
        int ch = wgetch(nwindow.input_win);  // read from input window

        if (ch == '\n' || ch == '\r') {
            // User pressed Enter -> send the message
            if (nwindow.input_len > 0) {
                nwindow.input_buf[nwindow.input_len] = '\0';
                
                // Show it in chat window (local echo)
                wmove(nwindow.chat_win, nwindow.chat_height - 2, 1);
                wprintw(nwindow.chat_win, "You: %s\n", nwindow.input_buf);
                wrefresh(nwindow.chat_win);

                // Send to server
                // send_to_server(input_buf);
                
                // Clear input
                memset(nwindow.input_buf, 0, sizeof(nwindow.input_buf));
                nwindow.input_len = 0;
                werase(nwindow.input_win);
                box(nwindow.input_win, 0, 0);
                mvwprintw(nwindow.input_win, 0, 2, " Message ");
                wmove(nwindow.input_win, 1, 1);
                wrefresh(nwindow.input_win);
            }
        } else if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
            if (nwindow.input_len > 0) {
                nwindow.input_len--;
                nwindow.input_buf[nwindow.input_len] = '\0';
                // Redraw input line
                werase(nwindow.input_win);
                box(nwindow.input_win, 0, 0);
                mvwprintw(nwindow.input_win, 0, 2, " Message ");
                mvwprintw(nwindow.input_win, 1, 1, "%s", nwindow.input_buf);
                wmove(nwindow.input_win, 1, 1 + nwindow.input_len);
                wrefresh(nwindow.input_win);
            }
        } else if (ch == 27) { // ESC to quit, for example
            return -1; //needs work
        } else if (ch >= 32 && ch < 127) { // printable
            if (nwindow.input_len < max_input - 1) {
                nwindow.input_buf[nwindow.input_len++] = (char)ch;
                waddch(nwindow.input_win, ch);
                wrefresh(nwindow.input_win);
            }
        }
    }
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

    
    initscr();              // start ncurses
    cbreak();               // no line buffering
    noecho();               // don't echo typed chars automatically
    keypad(stdscr, TRUE);   // enable arrow keys etc.
    curs_set(1);            // show cursor

    
    getmaxyx(stdscr, nwindow.rows, nwindow.cols);

    int input_height = 3; // one line + border
    int chat_height = nwindow.rows - input_height;

    // Create windows
    nwindow.chat_win = newwin(chat_height, nwindow.cols, 0, 0);
    nwindow.input_win = newwin(input_height, nwindow.cols, chat_height, 0);

    scrollok(nwindow.chat_win, TRUE);  // allow scrolling
    box(nwindow.chat_win, 0, 0);
    box(nwindow.input_win, 0, 0);

    mvwprintw(nwindow.chat_win, 0, 2, " Chat ");
    mvwprintw(nwindow.input_win, 0, 2, " Message ");
    wrefresh(nwindow.chat_win);
    wmove(nwindow.input_win, 1, 1);
    wrefresh(nwindow.input_win);


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


