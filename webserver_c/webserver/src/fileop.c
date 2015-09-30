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
#include<sys/stat.h>
#include<pwd.h>
#include<time.h>

FILE *logfile, *errlogfile;

int record_std(char *record)
{
    if(logfile == NULL)
    {
        printf("Standard log file is not open yet\n");
        return -1;
    }
    fprintf(logfile, "%s\n", record);
}

int record_err(char *record)
{
    if(errlogfile == NULL)
    {
        printf("Error log file is not open yet\n");
        return -1;
    }
    fprintf(errlogfile, "%s\n", record);
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


int open_err_log_file(char *path)
{
    if (errlogfile != NULL)
    {
        printf("Error log file has been openned already\n");
        return 1;
    }
    else
    {
        errlogfile = fopen(path, "aw+");
        if (errlogfile == NULL)
        {
            printf("Cannot open error logfile\n");
            return -1;
        }
    }
    return 0;
}


int open_std_log_file(char *path)
{
    if (logfile != NULL)
    {
        printf("Standard log file has been openned already\n");
        return 1;
    }
    else
    {
        logfile = fopen(path, "aw+");
        if (logfile == NULL)
        {
            printf("Cannot open standard logfile\n");
            return -1;
        }
    }
    return 0;
}
