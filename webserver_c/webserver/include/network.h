/*************************************************************************
	> File Name: network.h
	> Author: 
	> Mail: 
	> Created Time: Wed 23 Sep 2015 04:04:39 PM CEST
 ************************************************************************/

#ifndef _NETWORK_H
#define _NETWORK_H
void *get_in_addr(struct sockaddr *);
int start_listen(char*, int);
////////////////////////////
int send_data(int, char *, int);
int get_client_ip(int , char *);
int get_content_type(char *, char *);
int recv_data(int , char *, int , int );
int get_method(char *, char *, char *, char *);
int send_header(int , int , char *);

#endif

