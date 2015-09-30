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
#include<signal.h>
#include<errno.h>

void *get_in_addr(struct sockaddr *s)
{
    if(s->sa_family == AF_INET)
        return &(((struct sockaddr_in *)s)->sin_addr);
    else
        return &(((struct sockaddr_in6 *)s)->sin6_addr);

}

// start server listener 
int start_listen(char* port, int backlog)
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

    char hostname[100];
    gethostname(hostname, 99);

    //if((rv = getaddrinfo(hostname, port, &hints, &servinfo)) != 0)
    if((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv)); // gai_streeror() gets errors information by using return code from getaddrinfo()
        return -1;
    }
    for(p = servinfo; p != NULL; p = p->ai_next)
    {
        if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
        {
            perror("server: socket");
            continue;
        }
        if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int)) < 0)
        {
            perror("server: setsockopt");
            return -1;
        }
        if(bind(sockfd, p->ai_addr, p->ai_addrlen) < 0)
        {
            close(sockfd);
            perror("server: bind");
            continue;
        }
        break;
    }

    if(p == NULL)
    {
        fprintf(stderr, "server: failed find a node can be used to create a socket and bind\n");
        return -1;
    }

    if(listen(sockfd, backlog) < 0)
    {
        perror("server: listen\n");
        return -1;
    }
    //printf("Server has started\n");
    if( p->ai_family == AF_INET6 )
    {
        printf("IP version: ipv6\n");
    }
    if( p->ai_family == AF_INET)
    {
        printf("IP version: ipv4\n");
        // char ipaddr[100];
        // inet_atop(p->ai_family, get_in_addr((struct aockaddr *)p->ai_addr), ipaddr, sizeof(ipaddr));
        struct sockaddr_in *addr;
        addr = (struct sockaddr_in *)p->ai_addr; 
        printf("local ip: http://%s:%s\n", inet_ntoa((struct in_addr)addr->sin_addr), port);
    }
    freeaddrinfo(servinfo);
    return sockfd;
}
// send any data to client. 
int send_data(int clientfd, char *buf, int len)
{
    int lenleft = len; 
    int sentlen_total = 0;
    int sentlen_onetime = 0;
    signal(SIGPIPE, SIG_IGN);
    while(sentlen_total < len)
    {
        sentlen_onetime = send(clientfd, buf+sentlen_total, lenleft, 0);
        //printf("%d bytes data has been sent!\n", sentlen_onetime);
        if(sentlen_onetime < 0)
        {
            if(errno == SIGPIPE)
            {
                printf("client socket has closed\n");
                close(clientfd);
                return sentlen_total;
            }
//            printf("server error: send %s", buf);
            return sentlen_onetime;
        }
        sentlen_total += sentlen_onetime;
        lenleft -= sentlen_onetime;
    }
    return sentlen_total;
}
// send response header to client
int send_header(int clientfd, int status_code, char *content)
{
    char head[300];
    switch(status_code)
    {
        case 200:
            sprintf(head, "HTTP/1.1 200 OK\r\n");
            break;
        case 400:
            sprintf(head, "HTTP/1.1 400 Bad Request\r\n");
            break;
        case 403:
            sprintf(head, "HTTP/1.1 403 Forbidden\r\n");
            break;
        case 404:
            sprintf(head, "HTTP/1.1 404 Not Found\r\n");
            break;
        case 500:
            sprintf(head, "HTTP/1.1 500 Internal Server Error\r\n");
            break;
        case 501:
            sprintf(head, "HTTP/1.1 501 Not Implemented\r\n");
            break;
    }
    int len = strlen(head);
    sprintf(head, "%s%s\r\n", head, content);
    send_data(clientfd, head , strlen(head));
}
// get require method, url and http version, return a pointer that point at the begining of next line
char *get_method(char *buf, char *method, char *url, char *version)
{
    char nextline[50];
    sscanf(buf, "%s %s %s %s", method, url, version, nextline);
   // printf("method: %s\nurl: %s\nversion: %s\n", method, url, version);
    return strstr(buf, nextline);

}

// receive data
// return code : -1 connection broken; -2 Request too large; -3 other error 
// clientfd: connect socket, buf: data received, maxlen: maxlen expected, if this value is -1, means no limited
int recv_data(int clientfd, char *buf, int maxlen)
{
    int rec_len;
    char recv_data[513];
    int total_data = 0;
    errno = 0;

    for(rec_len = recv(clientfd, buf, 512, 0); rec_len > 0; rec_len = recv(clientfd, buf, 512, MSG_DONTWAIT))
    {
        sprintf(buf, "%s%s", buf, recv_data);
        total_data += rec_len;
        if(total_data > maxlen && maxlen != -1)
            return -2;
    }
    if(rec_len < 0)
    {
        if(errno != EAGAIN && errno != EWOULDBLOCK)
        {
            printf("Client: recv %d\n", rec_len);
            return -3;
        }
    }
    else if(rec_len == 0)
    {
        printf("Client: connection break\n%s\n", strerror(errno));
        return -1;
    }
    return 0;
}

int get_content_type(char *type, char *contenttype)
{
    if (!strcmp(type, "html"))
    {
        strcpy(contenttype, "text/html"); 
        return 0;
    }
    else if (!strcmp(type, "jpg"))
    {
        strcpy(contenttype, "application/x-jpg"); 
        return 0;
    }
    else if(!strcmp(type, "gif"))
    {
        strcpy(contenttype, "image/gif"); 
        return 0;
    }
    else if(!strcmp(type, "css"))
    {
        strcpy(contenttype, "text/css"); 
        return 0;
    }
    else
        return 1;
}

int get_client_ip(int clientfd, char *addr)
{
    struct sockaddr_storage clientsockaddr;
    int socketlen = sizeof(clientsockaddr);
    if ( getpeername(clientfd, (struct sockaddr *)&clientsockaddr, &socketlen) == -1)
    {
        perror("getpeername");
    }
    char ip[50];
    if ( inet_ntop(clientsockaddr.ss_family, get_in_addr((struct sockaddr *)&clientsockaddr), ip, sizeof ip) == NULL)
    { 
        perror("get_client_ip");
        return -1;
    }
    strcpy(addr, ip);
    return 0;
}

