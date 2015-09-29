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

int logflag;
char logpath[200];

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
    return send_file(clientfd, f);
}

void set_logflag(int flag)
{
    logflag = flag;
}

int set_logpath(char *path)
{
    if(realpath(path, logpath) == NULL)   
    {
        printf("set_logpath error: path invalid\n");
        return -1;
    }
    return 0;
}
int write_log(char *content)
{
   switch(logflag)
    {
        case 0:
        break;
    }
}

// test for chroot
/*
int main()
{
    char **p;
    char url[100] = "./";
    char newurl[100];
    char name[100];
    struct passwd *ps;
    getlogin_r(name, 100);
    realpath(url, newurl);
    ps = getpwnam(name);
    if(ps != NULL)
    {
        printf("gid: %d\nuid: %d\n", ps->pw_gid, ps->pw_uid);
    }
    else
    {
        printf("%s\n", strerror(errno));
    }
    url_is_valid("../../", newurl, p);
    setuid(ps->pw_uid);
    setgid(ps->pw_gid);
    if(chroot(newurl) == -1)
    {
        printf("%s\n", strerror(errno));
    }
   //free(ps);
    return 0;
}
*/
