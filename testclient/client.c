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

    getaddrinfo("127.0.0.1", "80", &hints, &servifo);
    sockfd = socket(servifo->ai_family, servifo->ai_socktype, servifo->ai_protocol);
    char msg[100000];
    FILE * f = fopen("msg.txt", "r");
    int s = connect(sockfd, servifo->ai_addr, servifo->ai_addrlen);
    //fread(msg, sizeof(char), 100, f);
    while(fgets(msg, 100, f) != NULL)
    {
        int len = strlen(msg);
        int a =  send(sockfd, msg, len, 0);
        printf("%d/n", a);
        
    }
    send(sockfd, "/n/t/n/t", 4, 0);
        char recvdata[2000];
        int rec_len = 0;
        int total_data = 0;
    for(rec_len = recv(sockfd, recvdata, 512, 0); rec_len > 0; rec_len = recv(sockfd, recvdata, 512, MSG_DONTWAIT))
        {
            recvdata[rec_len] = 0;
            strcat(buf,recvdata);
            total_data += rec_len;
            //    printf("%ld\n", total_data);
        } 
        printf("%s", buf);
        close(sockfd);
    freeaddrinfo(servifo);
    fclose(f);
    
}
