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
#include<pwd.h>

int send_file(int clientfd, FILE *f)
{
    char buf[20001]; 
    int time = 20000, size = 1;
    int len;
    while(fread(buf, size, time, f) != 0)
    {
        len = send_data(clientfd, buf, time * size);  
//        printf("%d bytes have been sent\n", len);
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
