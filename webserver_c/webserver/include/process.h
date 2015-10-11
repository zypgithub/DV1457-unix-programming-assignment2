/*************************************************************************
	> File Name: ../include/process.h
	> Author: 
	> Mail: 
	> Created Time: Wed 23 Sep 2015 11:21:03 PM CEST
 ************************************************************************/

#ifndef _PROCESS_H
#define _PROCESS_H
void handle_request(int, char *);

int prevent_zombie();

void handle_it_process(int, int );

void process_mode(int, struct sockaddr *);

int daemon_printpid(char *);
#endif
