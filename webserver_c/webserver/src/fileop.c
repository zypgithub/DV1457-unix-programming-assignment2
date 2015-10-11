/*************************************************************************
	> File Name: fileop.c
	> Author: 
	> Mail: 
	> Created Time: Fri 25 Sep 2015 05:01:38 PM CEST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<syslog.h>
#include<sys/stat.h>
#include<pthread.h>
#include<pwd.h>
#include<time.h>
#include"fileop.h"
#include"network.h"

FILE *logfile = NULL, *errlogfile = NULL;
pthread_mutex_t errlogmutexlock, stdlogmutexlock, sysloglock;

void record_std(char *record)
{
    if(logfile == NULL)
    {
        pthread_mutex_lock(&sysloglock);
        syslog(LOG_INFO, record);
        pthread_mutex_unlock(&sysloglock);
    }
    else
    {
        pthread_mutex_lock(&stdlogmutexlock);
        fprintf(logfile, "%s\n", record);
        pthread_mutex_unlock(&stdlogmutexlock);
    }
}

void record_err(char *record)
{
    if(errlogfile == NULL)
    {
        pthread_mutex_lock(&sysloglock);
        syslog(LOG_ERR,record);
        pthread_mutex_unlock(&sysloglock);
    }
    else
    {
        pthread_mutex_lock(&errlogmutexlock);
        fprintf(errlogfile, "%s\n", record);
        pthread_mutex_unlock(&errlogmutexlock);
    }
}

int get_file_size(char *path)
{
    struct stat filestat;
    if (stat(path, &filestat) == -1)
    {
        perror("get_file_size");
        return -1;
    }
    return filestat.st_size;
}

time_t get_file_last_modify(char *path)
{
    struct stat filestat;
    if (stat(path, &filestat) == -1)
    {
        perror("get_file_last_modify");
        return -1;
    }
    return filestat.st_mtime;
}

int send_file(int clientfd, FILE *f)
{
    char buf[100001]; 
    int len;
    int total_len = 0;
    while((len = fread(buf, sizeof(char), 100000, f)) != 0)
    {
        len = send_data(clientfd, buf, len);  
        total_len += len;
    }
    return total_len;
}

int open_send_file(int clientfd, char *path)
{
    FILE *f;

    f = fopen(path, "rb");
    if (f == NULL)
    {
        printf("open_send_file: file not found\n");
        return -1;
    }
    int temp = send_file(clientfd, f);
    fclose(f);
    return temp;
}


int open_err_log_file(char *name)
{
    if (name[0] == 0)
    {
        pthread_mutex_init(&sysloglock, NULL);
        openlog("webserver", NULL, LOG_USER);
        return 1;
    }
    else if (errlogfile != NULL)
        return 0;
    else
    {
        char logfilepath[200];
        pthread_mutex_init(&errlogmutexlock, NULL);
        sprintf(logfilepath, "./log/%s.err", name);
        errlogfile = fopen(logfilepath, "aw+");
        setlinebuf(errlogfile);
        if (errlogfile == NULL)
        {
            printf("Cannot open error logfile\n");
            return -1;
        }
    }
    return 0;
}


int open_std_log_file(char *name)
{
    if (name[0] == 0)
    {
        pthread_mutex_init(&sysloglock, NULL);
        openlog("webserver", NULL, LOG_USER);
        return 1;
    }
    else if (logfile != NULL)
    {
        return 0;
    }
    else
    {
        char logfilepath[200];
        pthread_mutex_init(&stdlogmutexlock, NULL);
        sprintf(logfilepath, "./log/%s.log", name);
        logfile = fopen(logfilepath, "aw+");
        setlinebuf(logfile);
        if (logfile == NULL)
        {
            printf("Cannot open standard logfile\n");
            return -1;
        }
    }
    return 0;
}
