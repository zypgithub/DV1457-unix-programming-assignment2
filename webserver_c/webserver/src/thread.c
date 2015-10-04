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
#include<signal.h>
#include<errno.h>

#include"../include/thread.h"

void *handle_it_thread(void *argv)
{
    pthread_t me = pthread_self();
    pthread_detach(me);
    handle_it(*(int *)argv, 0);
    close(*(int *)argv);
    free(argv);
}
void *thread_pool_run(void *argv)
{
    pthread_t me = pthread_self();
    pthread_detach(me);
    ThreadWorker *worker = (ThreadWorker *)argv;
    while(1)
    {
        pthread_mutex_lock(&waitlock);
        pthread_cond_wait(&(worker->threadcond), &waitlock);
        pthread_mutex_unlock(&waitlock);
//        printf("%u is in serving\n", me);
        handle_it(worker->clientfd, 0);
        close(worker->clientfd);
        thread_pool_push(worker);
    }
}

void thread_pool_getspace(int num)
{
    int i;
    //ThreadWorker *temp = malloc(num * sizeof(ThreadWorker));
    ThreadWorker *temp;
    pthread_mutex_lock(&inpoollock);
    pthread_t tid;
    for(i = 0; i < num; i ++)
    {
        temp = (ThreadWorker *)malloc(sizeof(ThreadWorker));
        pthread_cond_init(&(temp->threadcond), NULL);
        temp->next = NULL;
        pool.last->next = temp;
        pool.last = temp;
        pthread_create(&tid, NULL, &thread_pool_run, temp);
        temp->threadid = tid;
        
    }
    pthread_mutex_unlock(&inpoollock);
}

void thread_pool_init()
{
    pthread_mutex_init(&inpoollock, NULL);
    pthread_mutex_init(&outpoollock, NULL);
    pthread_mutex_init(&waitlock, NULL);
    ThreadWorker *temp = malloc(sizeof(ThreadWorker));
    pool.first = temp;
    pool.last = temp;
    temp->next = NULL;
    pthread_cond_init(&(temp->threadcond), NULL);
    pthread_t tid;
    pthread_create(&tid, NULL, &thread_pool_run, temp);
    temp->threadid = tid;
}

void thread_pool_push(ThreadWorker *worker)
{
    pthread_mutex_lock(&inpoollock);
    pool.last->next = worker;
    worker->next = NULL;
    pthread_mutex_unlock(&inpoollock);
}

ThreadWorker *thread_pool_pop()
{
    if (pool.first->next == NULL)
        return NULL;
    else
    {
        ThreadWorker *temp = pool.first;
        pool.first = temp->next;
        return temp;
    }
}

ThreadWorker *thread_pool_gettop()
{
    if (pool.first->next == NULL)
        return NULL;
    else
        return pool.first;
}

void thread_pool_wakeup(ThreadWorker *worker, int clientfd)
{
    worker->clientfd = clientfd;
    //pthread_mutex_lock(&waitlock);
    pthread_cond_signal(&(worker->threadcond));
    //pthread_mutex_unlock(&waitlock);
}

void thread_pool_mode(int sockfd, struct sockaddr *clientsockaddr)
{
    int *conn;
    int connfd;
    pthread_t tid;
    int clientaddrlen = sizeof(*clientsockaddr);
    printf("Server started in thread pool mode.\n");
    signal(SIGHUP, thread_pool_destory);
    signal(SIGINT, thread_pool_destory);
    signal(SIGQUIT, thread_pool_destory);
    signal(SIGTERM, thread_pool_destory);
    thread_pool_init();
    thread_pool_getspace(10);

    while(1)
    {
        connfd = accept(sockfd, clientsockaddr, &clientaddrlen);
        if(connfd == -1)
        {
            perror("server: accept\n");
            continue;
        }
        pthread_mutex_lock(&outpoollock);
        ThreadWorker *temp = thread_pool_pop(&pool);
        if(temp == NULL)
        {    
            thread_pool_getspace(1);
            temp = thread_pool_pop(&pool);
        }
        pthread_mutex_unlock(&outpoollock);
        thread_pool_wakeup(temp, connfd);
    }
    close(sockfd);
}

void thread_mode(int sockfd, struct sockaddr *clientsockaddr)
{
    int *conn;
    int connfd;
    pthread_t tid;
    printf("Server started in mutiple thread mode.\n");
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

void thread_pool_destory()
{
    //printf("start destorying thread pool\n");
    pthread_mutex_destroy(&outpoollock);
    pthread_mutex_destroy(&inpoollock);
    pthread_mutex_destroy(&waitlock);
    ThreadWorker *temp = pool.first;
    while(temp != NULL)
    {
        pool.first = temp->next;
        pthread_cancel(temp->threadid);
        free(temp);
        temp = pool.first;
    }
    printf("\n");
    exit(0);
}
