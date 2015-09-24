/*************************************************************************
	> File Name: network.c
	> Author: 
	> Mail: 
	> Created Time: Wed 23 Sep 2015 03:54:38 PM CEST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<netdb.h>
#include<netinet/in.h>
#include<arpa/inet.h>


void *get_in_addr(struct sockaddr *s)
{
    if(s->sa_family == AF_INET)
        return &(((struct sockaddr_in *)s)->sin_addr);
    else
        return &(((struct sockaddr_in6 *)s)->sin6_addr);

}
int start_linsten(char* port, int backlog)
{
    struct sockaddr_in serveraddr;
    struct addrinfo hints, *servinfo, *p;

    int rv;
    int server_socket, sockfd;
    int optval = 1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv)); // gai_streeror() gets errors information by using return code from getaddrinfo()
        return -1;
    }
    for(p = servinfo; p != NULL; p = p->ai_next)
    {
        if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
        {
            perror("server: socket\n");
            continue;
        }
        if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int)) < 0)
        {
            perror("server: setsockopt\n");
            return -1;
        }
        if(bind(sockfd, p->ai_addr, p->ai_addrlen) < 0)
        {
            close(sockfd);
            perror("server: bind\n");
            continue;
        }
        break;
    }

    if(p == NULL)
    {
        fprintf(stderr, "server: failed to start server");
        return -1;
    }

    if(listen(sockfd, backlog) < 0)
    {
        perror("server: listen\n");
        return -1;
    }
    printf("Server has started\n");
    if( p->ai_family == AF_INET6 )
    {
        printf("ipv6\n");
    }
    if( p->ai_family == AF_INET)
    {
        printf("ipv4\n");
        // char ipaddr[100];
        // inet_atop(p->ai_family, get_in_addr((struct aockaddr *)p->ai_addr), ipaddr, sizeof(ipaddr));
        struct sockaddr_in *addr;
        addr = (struct sockaddr_in *)p->ai_addr; 
        printf("local ip:%s\n", inet_ntoa((struct in_addr)addr->sin_addr));
    }
    freeaddrinfo(servinfo);
    return sockfd;
}

