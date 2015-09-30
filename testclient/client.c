/*************************************************************************
	> File Name: client.c
	> Author: 
	> Mail: 
	> Created Time: Tue 29 Sep 2015 11:56:22 PM CEST
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<netdb.h>
#include<sys/socket.h>

int main()
{
    int sockfd;
    char buf[2000];
    struct addrinfo hints, *servifo;
    int rv;

    memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    getaddrinfo("0.0.0.0", "8000", &hints, &servifo);
    sockfd = socket(servifo->ai_family, servifo->ai_socktype, servifo->ai_protocol);
    connect(sockfd, servifo->ai_addr, servifo->ai_addrlen);
    freeaddrinfo(servifo);
    char msg[1000];
    FILE * f = fopen("msg.txt", "r+");
    fread(msg, sizeof(char), 999, f);
    int len = strlen(msg);
    send(sockfd, msg, len, 0);
    while(1)
    {
        char rec[2000];
        int flag = recv(sockfd, rec, 1999, 0);
        if ( flag <= 0)
        {
            printf("%d\n", flag);
            break;
        }
        printf("rec:\n%s\n", rec);
    }
    
}
