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
#include<pwd.h>
#include<sys/stat.h>
#include<syslog.h>

#include"network.h"
#include"process.h"


void handle_it(int clientfd)
{
    char method[10], url[1000], version[10];
    char res[50];
    char buf[10000] = "";
    int recv_flag;
    char argu[200];
    char parsedurl[1000];
    FILE *f;

    if((recv_flag = recv_data(clientfd, buf, 4000)) < 0)
    {
        switch(recv_flag)
        {
            case -1:
            printf("Client closed socket already.\n");
            break;
            case -2:
            printf("Client send too large data\n");
            break;
            case -3:
            printf("server recv_data: unknown error\n");
            break;
        }
        return;
    }
    char *requirebody = get_method(buf, method, url, version);
    printf("Require method: %s\nRequire url: %s\nHttp version: %s\n", method, url, version);

    char requirecontent[10000];
    get_substr(requirebody, requirecontent, -1);
    printf("require content:\n%s", requirecontent);

    int validflag;
    struct stat filestat;
    if ((validflag = parse_url(url, parsedurl, argu)) == 0 )
    {

        if (strcmp(method, "GET") == 0)
        {
            //require method is GET
            if ((f = fopen(parsedurl, "rb")) == NULL)
            {
                validflag = 404; 
                printf("Required file %s not exist\n", parsedurl);
            }
            else
            {
                char contenttype[20];
                char type[20];
                int contenttypefg;
                get_file_type(parsedurl, type);
                if ((contenttypefg = get_content_type(type, contenttype)) == 0)
                {
                    strcat(contenttype, "; charset=utf-8");
                    stat(parsedurl, &filestat);
                    send_header(clientfd, 200, contenttype, filestat.st_size);
                    send_file(clientfd, f);
                }
                else
                {
                    switch(contenttypefg)
                    {
                        case 1:
                            validflag = 400;
                            printf("server error: get_content_type, GET method, unknown content_type");
                            break;

                    }
                }
            }
        }
        else if (strcmp(method, "HEAD") == 0)
        {
            //require method is HEAD
            if ((f = fopen(parsedurl, "rb")) == NULL)
            {
                validflag = 404; 
                printf("Required file %s not exist\n", parsedurl);
            }
            else
            {
                char contenttype[20];
                char type[20];
                struct stat filestat;
                int contenttypefg;
                get_file_type(parsedurl, type);
                if ((contenttypefg = get_content_type(type, contenttype)) == 0)
                {
                    strcat(contenttype, "; charset=utf-8");
                    stat(parsedurl, &filestat);
                    send_header(clientfd, 200, contenttype, filestat.st_size);
                }
                else
                {
                    switch(contenttypefg)
                    {
                        case 1:
                        validflag = 400;
                        printf("server error: get_content_type, HEAD method, unknown content_type");
                        break;

                    }
                }
            }
        }
        else
            validflag = 501;
    }
    switch(validflag)
    {
        case 400:
            stat("./html/400.html", &filestat);
            send_header(clientfd, 400, "text/html", filestat.st_size);
            open_send_file(clientfd, "./html/400.html");
            break;
        case 403:

            stat("./html/403.html", &filestat);
            send_header(clientfd, 403, "text/html", filestat.st_size);
            open_send_file(clientfd, "./html/403.html");
            break;
        case 404:
            stat("./html/404.html", &filestat);
            send_header(clientfd, 404, "text/html", filestat.st_size);
            open_send_file(clientfd, "./html/404.html");
            break;
        case 500:
            stat("./html/500.html", &filestat);
            send_header(clientfd, 500, "text/html", filestat.st_size);
            open_send_file(clientfd, "./html/500.html");
            break;
        case 501:
            stat("./html/501.html", &filestat);
            send_header(clientfd, 501, "text/html", filestat.st_size);
            open_send_file(clientfd, "./html/501.html");
            break;
    }
    return;
}

int main(int argc, char * argv[])
{
   
    int sockfd, connfd, backlog = 30, daemonflag = 0, logflag = 1;
    struct sockaddr clientsockaddr;
    char clientip[50];
    struct sigaction sig;
    char loginusername[20];
    struct passwd *ps;
    char webpath[200] = "./", webrealpath[200];
    char port[10] = "8080";
    FILE *config;

    //Read configuration file
    printf("Loading configuration file\n");
    
    if ((config = fopen(".lab3-config", "r")) == NULL)
    {
        printf("Configuration file is not found\n");
    }
    else
    {
        char conf_buf[2001];
        int readlen = fread(conf_buf, sizeof(char), 2048, config);
        conf_buf[readlen] = 0;
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
        loc = find_label(conf_buf, "DEFAULT_REQUEST_HANDLING_METHOD=", res);
        if (loc == -1)
        {
            printf("Did not find arguement: DEFAULT_REQUEST_HANDLING_METHOD\n");
        }
        else
        {
            if (strcmp(res, "NORMAL") == 0)
            {
                daemonflag = 0;
            }
            else if(strcmp(res, "DAEMON") == 0)
            {
                daemonflag = 1;
            }
            else
            {
                printf("The value of \"DEFAULT_REQUEST_HANDLING_METHOD\" is not valid\n");
            }
        }
        printf("Loading configuration file is finished\n");
        fclose(config);
    }
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

    //start listen socket
    sockfd = start_listen(port, backlog);
    if (sockfd < 0)
    {
        printf("Server fail to start when start listening\n");
        return -1;
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
    
    // check request handle method
    if (daemonflag == 1)
    {
        printf("Server start running in background!\n");
        daemon_printpid(webrealpath);
    }
    //daemon(1, 1);
    if (logflag > 0)
    {
        openlog("webserver", LOG_CONS, LOG_USER);
    }
   while(1)
    {
        int clientaddrlen = sizeof(clientsockaddr); 
        connfd = accept(sockfd, &clientsockaddr, &clientaddrlen);
        if(connfd == -1)
        {
            perror("server: accept\n");
            continue;
        }
        //t = time(NULL);
        //ti = localtime(&t);
        //inet_ntop(clientsockaddr.ss_family, get_in_addr((struct sockaddr *)&clientsockaddr), clientip, sizeof clientip);
        //printf("\nGot connection from %s\n", clientip);
        //printf("Require time: %.2d-%.2d-%d %.2d:%.2d:%.2d\n", ti->tm_mon + 1, ti->tm_mday, ti->tm_year + 1990, ti->tm_hour, ti->tm_min, ti->tm_sec);
        char formattedtime[50];
        log_get_current_time(formattedtime);
        printf("%s\n", formattedtime);
        return 0;
        /*
        get_client_ip(connfd);
        */
        signal(SIGPIPE, SIG_IGN);
        handle_it_process(connfd, sockfd);
        close(connfd);
    }
    close(sockfd);


    return 0;
}
