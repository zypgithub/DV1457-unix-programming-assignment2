/*************************************************************************
	> File Name: thread.c
	> Author: 
	> Mail: 
	> Created Time: Sat 03 Oct 2015 04:33:29 PM CEST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<pthread.h>

void *handle_it_thread(void *argv)
{
    pthread_t me = pthread_self();
    pthread_detach(me);
    handle_it(*(int *)argv);
    close(*(int *)argv);
    free(argv);
}

void thread_mode(int sockfd, struct sockaddr *clientsockaddr)
{
    int *conn;
    int connfd;
    pthread_t tid;
    int clientaddrlen = sizeof(*clientsockaddr);
    while(1)
    {
        connfd = accept(sockfd, clientsockaddr, &clientaddrlen);
        if(connfd == -1)
        {
            perror("server: accept\n");
            continue;
        }
        conn = malloc(sizeof(int));
        *conn = connfd;
        pthread_create(&tid, NULL, &handle_it_thread, conn);
    }
    close(sockfd);
}

