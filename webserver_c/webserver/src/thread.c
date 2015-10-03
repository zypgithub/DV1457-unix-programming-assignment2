/*************************************************************************
	> File Name: thread.c
	> Author: 
	> Mail: 
	> Created Time: Sat 03 Oct 2015 04:33:29 PM CEST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
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

