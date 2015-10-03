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

#include"../include/thread.h"

pthread_mutex_t inpoollock, outpoollock;



void *handle_it_thread(void *argv)
{
    pthread_t me = pthread_self();
    pthread_detach(me);
    handle_it(*(int *)argv);
    close(*(int *)argv);
    free(argv);
}

void *thread_pool_run(void *argv)
{

}

void threadpool_get_space(ThreadPool *pool, int num)
{
    int i;
    ThreadWorker *temp = malloc(num * sizeof(ThreadWorker));
    pthread_mutex_lock(&inpoollock);
    pool->last->next = temp;
    pool->last = &temp[num - 1];
    pool->last->next = NULL;
    pthread_mutex_unlock(&inpoollock);
    pthread_t tid;
    for(i = 0; i < num - 1; i ++)
    {
        temp[i].next = &temp[i + 1];   
        pthread_cond_init(&(temp[i].worker), NULL);
        pthread_create(&tid, NULL, &thread_pool_run, &temp[i]);
    }
}

void init_thread_pool(ThreadPool *pool)
{
    pthread_mutex_init(&inpoollock, NULL);
    pthread_mutex_init(&outpoollock, NULL);
    ThreadWorker *temp = malloc(sizeof(ThreadWorker));
    pool->first = temp;
    pool->last = temp;
    temp->next = NULL;
    pthread_cond_init(&(temp->worker), NULL);
    pthread_t tid;
    pthread_create(&tid, NULL, &thread_pool_run, temp);
}

void thread_pool_push(ThreadPool *pool, ThreadWorker *worker)
{
    pthread_mutex_lock(&inpoollock);
    pool->last->next = worker;
    worker->next = NULL;
    pthread_mutex_unlock(&inpoollock);
}

ThreadWorker *thread_pool_pop(ThreadPool *pool)
{
    if (pool->first->next == NULL)
        return NULL;
    else
    {
        ThreadWorker *temp = pool->first;
        pool->first = temp->next;
        return temp;
    }
}

ThreadWorker *thead_pool_gettop(const ThreadPool *pool)
{
    if (pool->first->next == NULL)
        return NULL;
    else
    {
        return pool->first;
    }
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

