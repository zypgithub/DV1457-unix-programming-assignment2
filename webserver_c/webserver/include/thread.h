/*************************************************************************
	> File Name: thread.h
	> Author: 
	> Mail: 
	> Created Time: Sat 03 Oct 2015 04:37:40 PM CEST
 ************************************************************************/

#ifndef _THREAD_H
#define _THREAD_H

typedef struct ThreadWorker
{
    pthread_cond_t worker;
    int clientfd;
    struct ThreadWorker * next;
}ThreadWorker;
typedef struct ThreadPool
{
    ThreadWorker *first;
    ThreadWorker *last;
}ThreadPool;


void *handle_it_thread(void *);
#endif
