/*************************************************************************
	> File Name: ../include/fileop.h
	> Author: 
	> Mail: 
	> Created Time: Wed 23 Sep 2015 11:21:03 PM CEST
 ************************************************************************/

#ifndef _FILEOP_H
#define _FILEOP_H
void handle_request(int, char *);
//////////////////////////////////
void record_std(char *);

void record_err(char *);

int get_file_size(char *);

time_t get_file_last_modify(char *);

int send_file(int, FILE *);

int open_send_file(int, char *);

int open_err_log_file(char *);

int open_std_log_file(char *);


#endif
