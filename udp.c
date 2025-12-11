#include "udp.h"

int set_socket_addr(struct sockaddr_in *addr, const char *ip, int port)
{
    memset(addr, 0, sizeof(*addr));
    addr->sin_family = AF_INET;  
    addr->sin_port = htons(port);

    if (ip == NULL)
    { 
        addr->sin_addr.s_addr = INADDR_ANY;
    }
    else
    {
        if (inet_pton(AF_INET, ip, &addr->sin_addr) <= 0)
        {
            return -1; // invalid IP string
        }
    }
    return 0;
}

int udp_socket_open(int port)
{
    int sd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in this_addr;
    set_socket_addr(&this_addr, NULL, port);
    bind(sd, (struct sockaddr *)&this_addr, sizeof(this_addr));

    return sd; 
}

int udp_socket_read(int sd, struct sockaddr_in *addr, char *buffer, int n)
{
    socklen_t len = sizeof(struct sockaddr_in);
    return recvfrom(sd, buffer, n, 0, (struct sockaddr *)addr, &len);
}

int udp_socket_write(int sd, struct sockaddr_in *addr, char *buffer, int n)
{
    int addr_len = sizeof(struct sockaddr_in);
    return sendto(sd, buffer, n, 0, (struct sockaddr *)addr, addr_len);
}
