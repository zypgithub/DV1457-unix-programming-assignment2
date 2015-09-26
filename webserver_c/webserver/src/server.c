/*************************************************************************
	> File Name: server.c
	> Author: 
	> Mail: 
	> Created Time: Tue 15 Sep 2015 12:23:26 PM CEST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<netdb.h>
#include<signal.h>
#include<errno.h>
#include<time.h>
#include<pwd.h>

#include"network.h"
#include"process.h"

int main(int argc, char * argv[])
{
   
    int sockfd, connfd;
    struct sockaddr_storage clientsockaddr;
    char clientip[50];
    struct sigaction sig;
    char loginusername[20];
    struct passwd *ps;
    char webpath[200] = "./", webrealpath[200];
    char port[10] = "8080";
    int  backlog = 30;
    FILE *config;
    time_t t;
    struct tm *ti;

    //Read configuration file
    printf("Loading configuration file\n");
    
    if ((config = fopen(".lab3-config", "rb")) == NULL)
    {
        printf("Configuration file is not found\n");
    }
    else
    {
        char conf_buf[2001];
        fread(conf_buf, sizeof(char), 2000, config);
        int loc;
        char res[10];
        loc = find_label(conf_buf, "PORT=", res);
        if (loc == -1)
        {
            printf("Did not find arguement: PORT\n");
        }
        else
        {
            strcpy(port, res);
        }
        
        loc = find_label(conf_buf, "BACKLOG=", res);
        if (loc == -1)
        {
            printf("Did not find arguement: BACKLOG\n");
        }
        else
        {
            sscanf(res, "%d", &backlog);
        }

        loc = find_label(conf_buf, "DOCUMENT_ROOT=", res);
        if (loc == -1)
        {
            printf("Did not find arguement: DOCUMENT_ROOT\n");
        }
        else
        {
            strcpy(webpath, res);
        }
        printf("Loading configuration file is finished\n");
    }
    
    //start listen socket
    sockfd = start_linsten(port, backlog);

    // use chroot to specify the root dictionary and then drop the root privilege
    getlogin_r(loginusername, 20);
    if((ps = getpwnam(loginusername)) == NULL)
    {
        printf("%s\n", strerror(errno));
        return -1;
    }
    if (realpath(webpath, webrealpath) == NULL)
    {
        printf("server error: %s\n", strerror(errno));
    }
    else
    {
        chdir(webrealpath);
        if(chroot(webrealpath) == -1)
        {
            printf("Cannot reset the root dir, reason: %s\n", strerror(errno));
            //return -1;
        }
        else
        {
            setuid(ps->pw_uid);
            setgid(ps->pw_gid);
        }
    }
    //prevent zombie process
    /*
    sig.sa_handler = SIG_DFL;
    sig.sa_flags = SA_NOCLDWAIT;
    if( sigaction(SIGCHLD, &sig, 0) < 0 )
    {
        printf("server: sigaction\n");
        return -1;
    }
    */
    prevent_zombie();

    printf("Server start success!\n");
    t = time(NULL);
    ti = localtime(&t);
    printf("Start time: %.2d-%.2d-%d %.2d:%.2d:%.2d\n", ti->tm_mon + 1, ti->tm_mday, ti->tm_year + 1990, ti->tm_hour, ti->tm_min, ti->tm_sec);
    while(1)
    {
        int clientaddrlen = sizeof(clientsockaddr); 
        connfd = accept(sockfd, (struct sockaddr *)&clientsockaddr, &clientaddrlen);
        if(connfd == -1)
        {
            perror("server: accept\n");
            continue;
        }
         t = time(NULL);
        ti = localtime(&t);
        inet_ntop(clientsockaddr.ss_family, get_in_addr((struct sockaddr *)&clientsockaddr), clientip, sizeof clientip);
        printf("\nGot connection from %s\n", clientip);
        printf("Require time: %.2d-%.2d-%d %.2d:%.2d:%.2d\n", ti->tm_mon + 1, ti->tm_mday, ti->tm_year + 1990, ti->tm_hour, ti->tm_min, ti->tm_sec);

       // int byteslen = recv(connfd, buf, 10000, 0);
       // printf("command: %s\n", buf);
        //send_header(connfd, 200, "text/html", 50);

        signal(SIGPIPE, SIG_IGN);
        /*
        for(rec_len = recv(connfd, buf, 512, 0); rec_len > 0; rec_len = recv(connfd, buf, 512, MSG_DONTWAIT))
        {
            sprintf(buf, "%s%s", buf, recv_data);
        }
        if(rec_len < 0)
        {
            if(errno != EAGAIN)
            {
                printf("server: recv %d\n", rec_len);
                continue;
            }
        }
        else if(rec_len == 0)
        {
            close(connfd);
            continue;
        }
        */
        handle_it(connfd);
        //printf("Im the father\n");
        close(connfd);
    }
    close(sockfd);


    return 0;
}
