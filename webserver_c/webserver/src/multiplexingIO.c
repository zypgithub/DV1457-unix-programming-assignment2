/*************************************************************************
	> File Name: multiplexingIO.c
	> Author: 
	> Mail: 
	> Created Time: Sun 04 Oct 2015 03:07:59 PM CEST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/select.h>
#include<sys/socket.h>
#include<signal.h>
#include<../include/multiplexingIO.h>

//#define debug
#define linklist_way
//#define loop_way
void sock_linklist_init();
void sock_linklist_insert(SockNode *);
void sock_linklist_destroy();
void sock_linklist_rm(SockNode *, SockNode **);

#ifdef linklist_way
void multiplexing_IO_mode(int connfd, struct sockaddr *clientsockaddr)
{
    fd_set read_fds;
    FD_ZERO(&read_fds);
    signal(SIGHUP, sock_linklist_destroy);
    signal(SIGINT, sock_linklist_destroy);
    signal(SIGQUIT, sock_linklist_destroy);
    signal(SIGTERM, sock_linklist_destroy);
    printf("Server started in multiplexing I/O mode\n");
    SockNode *node = (SockNode *)malloc(sizeof(SockNode));
    node->socknum = connfd;
    sock_linklist_init();
    sock_linklist_insert(node);
    SockNode *curr, *prev, *temp;
    while(1)
    {

        read_fds = linklist.fdset;
        if (select(linklist.nfds, &read_fds, NULL, NULL, NULL) == -1)
        {
            perror("select");
            sock_linklist_destroy();
            return;
        }
        prev = NULL;
        curr = linklist.first;
        while(curr != NULL)
        {
            if (FD_ISSET(curr->socknum, &read_fds))
            {
                if(curr->socknum == connfd)
                {
                    //connect
                    int len = sizeof(*clientsockaddr);
                    int clientfd = accept(connfd, clientsockaddr, &len);
                    if(clientfd < 0)
                    {
                        perror("multiplexingIO clientfd < 0");
                    }
                    else
                    {
                        FD_SET(clientfd, &linklist.fdset);
                        temp = (SockNode *)malloc(sizeof(SockNode));
                        temp->socknum = clientfd;
                        sock_linklist_insert(temp);
                    }
                }
                else
                {
                    handle_it(curr->socknum, MSG_DONTWAIT);
                    close(curr->socknum);
                    sock_linklist_rm(prev, &curr);
                }
            }
            prev = curr;
            curr = curr->next;
        }
        
    }
} 
#endif

#ifdef loop_way
void multiplexing_IO_mode(int connfd, struct sockaddr *clientsockaddr)
{
    fd_set read_fds, origin;
    FD_ZERO(&read_fds);
    FD_ZERO(&origin);
    printf("Server started in multiplexing I/O mode\n");
    FD_SET(connfd, &origin);
    int maxnum = connfd + 1;
    while(1)
    {
        read_fds = origin;
        if (select(maxnum, &read_fds, NULL, NULL, NULL) == -1)
        {
            perror("select");
            return;
        }
        int i;
        for(i = 0; i < maxnum; i++)
        {
            if (FD_ISSET(i, &read_fds))
            {
                if(i == connfd)
                {
                    //connect
                    int len = sizeof(*clientsockaddr);
                    int clientfd = accept(connfd, clientsockaddr, &len);
                    if(clientfd < 0)
                    {
                        perror("multiplexingIO clientfd < 0");
                    }
                    else
                    {
                        FD_SET(clientfd, &origin);
                        if(clientfd >= maxnum)
                            maxnum = clientfd + 1;
                    }
                }
                else
                {
                    handle_it(i, MSG_DONTWAIT);
                    close(i);
                    FD_CLR(i, &origin);
                }
            }
        }
        
    }
}
#endif



void sock_linklist_insert(SockNode *node)
{
    node->next = linklist.first;
    linklist.first = node;
    if (node->socknum >= linklist.nfds)
    {
        linklist.nfds = node->socknum + 1;
    }
    FD_SET(node->socknum, &linklist.fdset);
}

void sock_linklist_init()
{
    linklist.nfds = 1;
    linklist.first = NULL;
    FD_ZERO(&linklist.fdset);
}

void sock_linklist_rm(SockNode *prev, SockNode **curr)
{
    FD_CLR((*curr)->socknum, &linklist.fdset);
    if (prev == NULL)
    {
        linklist.first = (*curr)->next;
        free(*curr);
        *curr = linklist.first;
    }
    else
    {
        prev->next = (*curr)->next;
        free(*curr);
        *curr = prev->next;
    }
}

void sock_linklist_destroy()
{
    SockNode *temp = linklist.first;
    while(temp != NULL)
    {
        linklist.first = temp->next;
        free(temp);
        temp = linklist.first;
    }
    printf("\n");
    exit(0);
}
