/*************************************************************************
	> File Name: server.c
	> Author: 
	> Mail: 
	> Created Time: Tue 15 Sep 2015 12:23:26 PM CEST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<netdb.h>

#include"network.h"
#include"process.h"

#define PORT "8080"
#define BACKLOG 30

int main(int argc, char * argv[])
{
   
    int sockfd, connfd;
    struct sockaddr_storage clientsockaddr;
    char clientip[50];

    sockfd = start_linsten(PORT, BACKLOG);
    printf("sockfd: %d\n", sockfd);
    while(1)
    {
        int clientaddrlen = sizeof(clientsockaddr); 
        connfd = accept(sockfd, (struct sockaddr *)&clientsockaddr, &clientaddrlen);
        if(connfd == -1)
        {
            perror("server: accept\n");
            continue;
        }
        
        inet_ntop(clientsockaddr.ss_family, get_in_addr((struct sockaddr *)&clientsockaddr), clientip, sizeof clientip);
        printf("server : got connection from %s\n", clientip);
        handle_request(connfd);

    }
    close(sockfd);


    return 0;
}
