/*************************************************************************
	> File Name: daemon.c
	> Author: 
	> Mail: 
	> Created Time: Sun 27 Sep 2015 10:31:17 PM CEST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<syslog.h>
#include<unistd.h>

int main()
{

//    openlog("daemon", 0, LOG_USER);
//    syslog(LOG_ERR, "HELLO, I accessed here by syslog after chroot function\n");
    /*
    FILE *f = fopen("../test.txt", "w+");
    fprintf(f, "Hello, Im here\n");
    chroot("./");
    char rpath[100];
    realpath("./", rpath);
    printf("%s\n", rpath);
    fprintf(f, "hello, Im still here\n");
    */
    printf("%d %d", '\r', '\n');
    return 0;
}
