#ifndef _udp_
#define _udp_

// libraries needed for various functions
// use man page for details
#include <sys/types.h>  // data types like size_t, socklen_t
#include <sys/socket.h> // socket(), bind(), connect(), listen(), accept()
#include <netinet/in.h> // sockaddr_in, htons(), htonl(), INADDR_ANY
#include <arpa/inet.h>  // inet_pton(), inet_ntop()
#include <unistd.h>     // close()
#include <string.h>     // memset(), memcpy()
#include <assert.h>

#define BUFFER_SIZE 1024
#define SERVER_PORT 12000

int set_socket_addr(struct sockaddr_in *addr, const char *ip, int port);

int udp_socket_open(int port);

int udp_socket_read(int sd, struct sockaddr_in *addr, char *buffer, int n);

int udp_socket_write(int sd, struct sockaddr_in *addr, char *buffer, int n);

#endif