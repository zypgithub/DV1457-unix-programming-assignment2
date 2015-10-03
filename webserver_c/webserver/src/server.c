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
#include<syslog.h>
#include<pthread.h>

#include"network.h"
#include"process.h"
#include"../include/thread.h"

#define MAXHEADLENGTH 2000

char webpath[200] = "./";


void handle_it(int clientfd)
{
    char method[10], url[4000], version[20];
    char content[2000], filelastmodify[50], buf[10000] = "", argu[200], parsedurl[1000], loglevel[10];
    char errlogcontent[1000], logcontent[1000], temp[1000], temp2[1000], abrequestpath[1000], clientip[20], errmsg[200] = "";
    int statuscode;
    int recv_flag, datasent = 0;
    FILE *f;
    time_t t;
    struct tm *ti;

    signal(SIGPIPE, SIG_IGN);
    recv_flag = recv_data(clientfd, buf, MAXHEADLENGTH);

    if(recv_flag < 0)
    {
        errlog_get_current_time(temp);
        get_client_ip(clientfd, clientip);
        printf("buf: %s", buf);
        switch(recv_flag)
        {
            case -1:
                sprintf(errmsg, "Client send too large data");
                sprintf(errlogcontent, "%s [error] [client %s] %s: /", temp, clientip, errmsg);
                break;
            case -2:
                sprintf(errmsg, "file(%s) function(%s) line(%d) %s", __FILE__, __FUNCTION__, __LINE__, strerror(errno));
                sprintf(errlogcontent, "%s [error] [client %s] %s: /", temp, clientip, errmsg);
                break;
            case -3:
                sprintf(errmsg, "Client didn't send any data");
                sprintf(errlogcontent, "%s [warn] [client %s] %s: /", temp, clientip, errmsg);
                break;
        }
        record_err(errlogcontent);
        printf("%s\n", errlogcontent);
        return;
    }
    get_client_ip(clientfd, clientip);
    // Ignore the second and third colume. they can be added here if necessary
    sprintf(logcontent, "%s - - ", clientip);
    stdlog_get_current_time(temp);//temp is ip addr 
    strcat(logcontent, temp);
    statuscode = get_method(buf, method, url, version);
//    printf("%s %s %s %d\n", method, url, version, statuscode);
    if(statuscode == 0)
    {
        sprintf(temp, " \"%s %s %s\"", method, url, version);
        strcat(logcontent, temp);
        statuscode = parse_url(url, parsedurl, argu);
        sprintf(temp, "%s%s", webpath, url);
        realpath(temp, abrequestpath);
        //printf("method: %s, url: %s, version: %s\n", method, url, version);
        if (statuscode == 0 )
        {
            char contenttype[50];
            char type[20];
            int contenttypefg;
            get_file_type(parsedurl, type);
            if ((contenttypefg = get_content_type(type, contenttype)) == 0)
            {
                if (!strcmp(method, "GET") || !strcmp(method, "HEAD"))
                {
                    f = fopen(parsedurl, "rb");
                    if (f == NULL)
                    {
                        statuscode = 404; 
                    }
                    else
                    {
                        strcat(contenttype, "; charset=utf-8");
                        t = get_file_last_modify(parsedurl);
                        ti = localtime(&t);
                        strftime(filelastmodify, 50, "%a, %e %b %Y %H:%M:%S GMT", ti);
                        sprintf(content, "Content-Type: %s\r\nContent-Length: %d\r\nLast-Modified: %s\r\nServer: Alex\r\n", contenttype, get_file_size(parsedurl), filelastmodify);
                        send_header(clientfd, 200, content);
                        if (strcmp(method, "GET") == 0)
                        {    
                            datasent = send_file(clientfd, f);
                        }
                        statuscode = 200;
                        fclose(f);
                    }
                }
                else
                {
                    statuscode = 501;
                }
            }
            else
            {
                switch(contenttypefg)
                {
                    case 1:
                    statuscode = 400;
                    sprintf(errmsg, "unknown file type %s: %s", type, abrequestpath);
                    break;
                } 
            }
        }
    }
    switch(statuscode)
    {
        case 400:
            t = get_file_last_modify("./html/400.html");
            ti = localtime(&t);
            strftime(filelastmodify, 50, "%a, %e %b %Y %H:%M:%S GMT", ti);
            sprintf(content, "Content-Type: text/html\r\nContent-Length: %d\r\nLast-Modified: %s\r\nServer: Alex\r\n", get_file_size("./html/400.html"), filelastmodify);
            send_header(clientfd, 400, content);
            datasent = open_send_file(clientfd, "./html/400.html");
            strcpy(loglevel, "notice");
            if(errmsg[0] == 0)
                sprintf(errmsg, "Bad Request: %s", abrequestpath);
            break;
        case 403:
            t = get_file_last_modify("./html/403.html");
            ti = localtime(&t);
            strftime(filelastmodify, 50, "%a, %e %b %Y %H:%M:%S GMT", ti);
            sprintf(content, "Content-Type: text/html\r\nContent-Length: %d\r\nLast-Modified: %s\r\nServer: Alex\r\n", get_file_size("./html/403.html"), filelastmodify);
            send_header(clientfd, 403,content);
            datasent = open_send_file(clientfd, "./html/403.html");
            strcpy(loglevel, "notice");
            if(errmsg[0] == 0)
                sprintf(errmsg, "Forbidden: %s", abrequestpath);
            break;
        case 404:
            t = get_file_last_modify("./html/404.html");
            ti = localtime(&t);
            strftime(filelastmodify, 50, "%a, %e %b %Y %H:%M:%S GMT", ti);
            sprintf(content, "Content-Type: text/html\r\nContent-Length: %d\r\nLast-Modified: %s\r\nServer: Alex\r\n", get_file_size("./html/404.html"), filelastmodify);
            send_header(clientfd, 404, content);
            datasent = open_send_file(clientfd, "./html/404.html");
            strcpy(loglevel, "notice");
            if(errmsg[0] == 0)
                sprintf(errmsg, "Not Found: %s", abrequestpath);
            break;
        case 500:
            t = get_file_last_modify("./html/500.html");
            ti = localtime(&t);
            strftime(filelastmodify, 50, "%a, %e %b %Y %H:%M:%S GMT", ti);
            sprintf(content, "Content-Type: text/plain\r\nContent-Length: %d\r\nLast-Modified: %s\r\nServer: Alex\r\n", get_file_size("./html/500.html"), filelastmodify);
            send_header(clientfd, 500, content);
            //open_send_file(clientfd, "./html/500.html");
            strcpy(loglevel, "error");
            if(errmsg[0] == 0)
                sprintf(errmsg, "Internal server Error: %s", abrequestpath);
            break;
        case 501:
            t = get_file_last_modify("./html/501.html");
            ti = localtime(&t);
            strftime(filelastmodify, 50, "%a, %e %b %Y %H:%M:%S GMT", ti);
            sprintf(content, "Content-Type: text/plain\r\nContent-Length: %d\r\nLast-Modified: %s\r\nServer: Alex\r\n", get_file_size("./html/501.html"), filelastmodify);
            sprintf(errmsg, "Method %s is nor implemented: %s", method, url);
            send_header(clientfd, 501, content);
            //open_send_file(clientfd, "./html/501.html");
            strcpy(loglevel, "error");
            if(errmsg[0] == 0)
                sprintf(errmsg, "Not Implemented: %s", abrequestpath);
            break;
        
    }
    if (errmsg[0] != 0)
    {
        errlog_get_current_time(temp);
        sprintf(errlogcontent, "%s [%s] [client %s] %s", temp, loglevel, clientip, errmsg); 
        printf("%s\n", errlogcontent);
        record_err(errlogcontent);
    }
    sprintf(temp, " %d %d", statuscode, datasent);
    strcat(logcontent, temp);
    printf("%s\n", logcontent);
    record_std(logcontent);
    return;
}


int main(int argc, char *argv[])
{
    int sockfd, connfd, backlog = 30, daemonflag = 0;
    int modeflag = 2;
    struct sockaddr clientsockaddr;
    char loginusername[20];
    struct passwd *ps;
    char  webrealpath[200], logfilename[200] = "";
    char port[10] = "8080";
    FILE *config;


    realpath("./", webpath);
    //Read configuration file
    
    if ((config = fopen(".lab3-config", "r")) == NULL)
    {
        printf("Configuration file is not found\n");
    }
    else
    {
        char conf_buf[2001];
        int readlen = fread(conf_buf, sizeof(char), 2048, config);
        int loc;
        char res[200];

        conf_buf[readlen] = 0;
        loc = find_label(conf_buf, "PORT=", res);
        if (loc == -1)
        {
            printf("Did not find the arguement: PORT\n");
        }
        else
        {
            strcpy(port, res);
        }
        
        loc = find_label(conf_buf, "BACKLOG=", res);
        if (loc == -1)
        {
            printf("Did not find the arguement: BACKLOG\n");
        }
        else
        {
            sscanf(res, "%d", &backlog);
        }

        loc = find_label(conf_buf, "DOCUMENT_ROOT=", res);
        if (loc == -1)
        {
            printf("Did not find the arguement: DOCUMENT_ROOT\n");
        }
        else
        {
            strcpy(webpath, res);
        }
        loc = find_label(conf_buf, "DEFAULT_REQUEST_HANDLING_METHOD=", res);
        if (loc == -1)
        {
            printf("Did not find the arguement: DEFAULT_REQUEST_HANDLING_METHOD\n");
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
        loc = find_label(conf_buf, "HANDLE_METHOD=", res);
        if (loc == -1)
        {
            printf("Did not find the arguement: HANDLE_METHOD");
        }
        else
        {
            if(strcmp(res, "PROCESSES") == 0)
            {
                modeflag = 1;
            }
            else if(strcmp(res, "THREADS") == 0)
            {
                modeflag = 2;
            }
            else if(strcmp(res, "SINGLE") == 0)
            {
                modeflag = 0;
            }
            else
            {
                printf("The value of \"HANDLE_METHOD\" is not valid\n");
            }
        }
        fclose(config);
    }

    // check arguements
    if (argc > 1)
    {
        int i;
        for(i = 1; i < argc; i++)
        {
            if (argv[i][0] != '-')
            {
                printf("Option error, Please use -h to get some help.\n");
                return 1;
            }  
            switch(argv[i][1])
            {
                case 'h':
                    printf("User manual.\n");
                    printf("-h, Get help information.\n");
                    printf("-p portnumber, Set server port.\n");
                    printf("-d, Run as a daemon process.\n");
                    printf("-l <logfilename>, Record log file to <logfilename>.log and <logfilename>.err. The path of log files is WEBSERVERROOT/log.\n");
                    printf("-s <single>|<processes>|<threads>, Set handling method.\n");
                    return 0;
                case 'p':
                    strcpy(port, argv[i + 1]);
                    i++;
                    break;
                case 'd':
                    daemonflag = 1;
                    break;
                case 'l':
                    if (i + 1 >= argc || argv[i + 1][0] == '-')
                    {
                        printf("Opition error: \"-f\" need a file name.\n");
                        return 0;
                    }
                    strcpy(logfilename, argv[i + 1]);
                    i++;
                    break;
                case 's':
                    //handle_method
                    if (i + 1 >= argc || argv[i + 1][0] == '-')
                    {
                        printf("Opition error: \"-s\" need a file name.\n");
                        return 0;
                    }
                    if (strcmp(argv[i + 1], "single") == 0)
                        modeflag = 0;
                    else if (strcmp(argv[i + 1], "processes") == 0)
                        modeflag = 1;
                    else if (strcmp(argv[i + 1], "threads") == 0)
                        modeflag = 2;
                    else
                    {
                        printf("unknown handle method, use -h to get some help.\n");
                        return -1;
                    }
                        
                break;
                default:
                    printf("Option error, Please use -h to get some help.\n");
                    return 1;
            }
        }
    }

    // open log file
    // rwx:7, rw-:6, r-x:5, r--:4, -wx:3, -w-:2, --x:1, ---:0
    mkdir("./log", 0744);
    //umask(022);
    //open log files
    if (open_std_log_file(logfilename) < 0)
    {
        printf("server error: open standard log file failed\n");
        return -1;
    }
    if (open_err_log_file(logfilename) < 0)
    {
        printf("server error: open error log file failed\n");
        return -1;
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
        printf("realpath error: %s\n", strerror(errno));
        printf("server root path is invalid\n");
        return -1;
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
    prevent_zombie();

    //start listen socket
    sockfd = start_listen(port, backlog);
    if (sockfd < 0)
    {
        printf("Server fail to start when start listening\n");
        return -1;
    }


    printf("Server start success!\n");
    
    // check request handle method
    if (daemonflag == 1)
    {
        printf("Server start running in background!\n");
        daemon_printpid(webrealpath);
    }
    //daemon(1, 1);
    //
    //check logflag 0 means use system log, 1 means use the specified file
    int i = 0;
    int *conn;
    //pthread_t *tid;
    pthread_t tid;
    int clientaddrlen = sizeof(clientsockaddr); 
    while(1)
    {
        connfd = accept(sockfd, &clientsockaddr, &clientaddrlen);
        if(connfd == -1)
        {
            perror("server: accept\n");
            continue;
        }
        switch(modeflag)
        {
            case 0:// single process
                handle_it(connfd);
                close(connfd);
                break;;
            case 1: // mutipule process
                handle_it_process(connfd, sockfd);
                close(connfd);
                break;
            case 2: // mutipule thread
                conn = malloc(sizeof(int));
                *conn = connfd;
                pthread_create(&tid, NULL, &handle_it_thread, conn);
                break;
        }
    }
    close(sockfd);
    return 0;
}
