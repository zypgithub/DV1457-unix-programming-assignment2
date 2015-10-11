/*************************************************************************
	> File Name: thread.h
	> Author: 
	> Mail: 
	> Created Time: Sat 03 Oct 2015 04:37:40 PM CEST
 ************************************************************************/

#ifndef _THREAD_H
#define _THREAD_H
#define MAXMEMBLOCK 1000
typedef struct ThreadWorker
{
    pthread_cond_t threadcond;
    pthread_t threadid;
    int clientfd;
    struct ThreadWorker * next;
}ThreadWorker;
typedef struct ThreadPool
{
    ThreadWorker *memblock[MAXMEMBLOCK];
    int blocklen;
    ThreadWorker *first;
    ThreadWorker *last;
}ThreadPool;

void *handle_it_thread(void *);
//////////////////////////////////////////
void *thread_pool_run(void *);

int thread_pool_getspace(int );

void thread_pool_init();

void thread_pool_push(ThreadWorker *);

ThreadWorker *thread_pool_pop();

ThreadWorker *thread_pool_gettop();

void thread_pool_wakeup(ThreadWorker *, int );

void thread_pool_mode(int, struct sockaddr *);

void thread_mode(int, struct sockaddr *);

void thread_pool_destory();
/////////////////////////////////////////
pthread_mutex_t inpoollock, outpoollock;
pthread_mutex_t waitlock;
ThreadPool pool;
#endif
