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
#include<signal.h>
#include<errno.h>

#include"network.h"
#include"process.h"

#define PORT "8080"
#define BACKLOG 30

int main(int argc, char * argv[])
{
   
    int sockfd, connfd;
    struct sockaddr_storage clientsockaddr;
    char clientip[50];
    struct sigaction sig;

    sockfd = start_linsten(PORT, BACKLOG);
    printf("sockfd: %d\n", sockfd);

    //prevent zombie process
    /*
    sig.sa_handler = SIG_DFL;
    sig.sa_flags = SA_NOCLDWAIT;
    if( sigaction(SIGCHLD, &sig, 0) < 0 )
    {
        printf("server: sigaction\n");
        return -1;
    }
    */
    prevent_zombie();
    char recv_data[513];
    while(1)
    {
        char buf[10000] = "";
        int clientaddrlen = sizeof(clientsockaddr); 
        connfd = accept(sockfd, (struct sockaddr *)&clientsockaddr, &clientaddrlen);
        if(connfd == -1)
        {
            perror("server: accept\n");
            continue;
        }
        
        inet_ntop(clientsockaddr.ss_family, get_in_addr((struct sockaddr *)&clientsockaddr), clientip, sizeof clientip);
        printf("server : got connection from %s\n", clientip);

       // int byteslen = recv(connfd, buf, 10000, 0);
       // printf("command: %s\n", buf);
        //send_header(connfd, 200, "text/html", 50);
        int rec_len;

        signal(SIGPIPE, SIG_IGN);
        for(rec_len = recv(connfd, buf, 512, 0); rec_len > 0; rec_len = recv(connfd, buf, 512, MSG_DONTWAIT))
        {
            sprintf(buf, "%s%s", buf, recv_data);
        }
        if(rec_len < 0)
        {
            if(errno != EAGAIN)
            {
                printf("server: recv %d\n", rec_len);
                continue;
            }
        }
        else if(rec_len == 0)
        {
            close(connfd);
            continue;
        }
        handle_request(connfd, buf);
        close(connfd);

    }
    close(sockfd);


    return 0;
}
