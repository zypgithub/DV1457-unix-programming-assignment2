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
#include<syslog.h>
#include<sys/socket.h>
#include<errno.h>
#include<pwd.h>
#include<pthread.h>

void *test(void *argv)
{
    printf("haha I'm number: %u\n", pthread_self());
}
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
    /*
    struct sockaddr syslogaddr;
    int logfile = socket(AF_UNIX, SOCK_DGRAM, 0);
    connect(logfile, &syslogaddr, sizeof(syslogaddr));
*/
    
    //FILE *f = fopen("/var/log/syslog", "aw");
    /*
    openlog("webserver", NULL, LOG_USER);
    
    if (1 == NULL)
    {
        printf("no\n");
    }
    else
    {
        printf("YES\n");
        struct passwd *ps;
        char loginusername[20];
        getlogin_r(loginusername, 20);
        if(( ps = getpwnam(loginusername)) == NULL)
        {
            printf("%s\n", strerror(errno));
            return -1;
        }
        else
        {
            syslog(LOG_ERR, "HELLO this is from syslog before chroot/n");
            chdir("./");
            if(chroot("./") == -1)
            {
                printf("Cannot reset the root dir, reason: %s\n", strerror(errno));
                //return -1;
            }
            else
            {
                syslog(LOG_ERR, "HELLO this is from syslog after chroot, before drop privilege\n");
                setuid(ps->pw_uid);
                setgid(ps->pw_gid);
            }
        }
        //fprintf(f, "hello this is from my function after chroot and dorp privilege\n");
    }
    syslog(LOG_ERR, "HELLO this is from syslog after chroot and drop privilege\n");
    char path[100];
    realpath("./", path);
    printf("%s\n", path);
*/
    int i;
    for(i = 0; i < 10; i ++)
    {
        pthread_t tid;
        pthread_create(&tid, NULL, &test, NULL);
    }

    return 0;
}
