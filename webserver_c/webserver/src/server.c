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

#include"network.h"
#include"process.h"

#define MAXHEADLENGTH 1000

void handle_it(int clientfd)
{
    char method[10], url[4000], version[10];
    char content[2000], filelastmodify[50], res[50], buf[10000] = "", argu[200], parsedurl[1000];
    char logcontent[100], temp[1000];
    int recv_flag, datasent = 0;
    FILE *f;
    time_t t;
    struct tm *ti;

    if((recv_flag = recv_data(clientfd, buf, MAXHEADLENGTH)) < 0)
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
    get_client_ip(clientfd, temp);
    // Ignore the second and third colume. they can be added here if necessary
    sprintf(logcontent, "%s - - ", temp);
    log_get_current_time(temp);
    strcat(logcontent, temp);

    char *requirebody = get_method(buf, method, url, version);
    printf("Require method: %s\nRequire url: %s\nHttp version: %s\n", method, url, version);
    sprintf(temp, " \"%s %s %s\"", method, url, version);
    strcat(logcontent, temp);
    int statuscode;
    if ((statuscode = parse_url(url, parsedurl, argu)) == 0 )
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
                    printf("Required file %s not exist\n", parsedurl);
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
                printf("server error: get_content_type, HEAD method, unknown content_type");
                break;
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
            break;
        case 403:
            t = get_file_last_modify("./html/403.html");
            ti = localtime(&t);
            strftime(filelastmodify, 50, "%a, %e %b %Y %H:%M:%S GMT", ti);
            sprintf(content, "Content-Type: text/html\r\nContent-Length: %d\r\nLast-Modified: %s\r\nServer: Alex\r\n", get_file_size("./html/403.html"), filelastmodify);
            send_header(clientfd, 403,content);
            datasent = open_send_file(clientfd, "./html/403.html");
            break;
        case 404:
            t = get_file_last_modify("./html/404.html");
            ti = localtime(&t);
            strftime(filelastmodify, 50, "%a, %e %b %Y %H:%M:%S GMT", ti);
            sprintf(content, "Content-Type: text/html\r\nContent-Length: %d\r\nLast-Modified: %s\r\nServer: Alex\r\n", get_file_size("./html/404.html"), filelastmodify);
            send_header(clientfd, 404, content);
            datasent = open_send_file(clientfd, "./html/404.html");
            break;
        case 500:
            t = get_file_last_modify("./html/500.html");
            ti = localtime(&t);
            strftime(filelastmodify, 50, "%a, %e %b %Y %H:%M:%S GMT", ti);
            sprintf(content, "Content-Type: text/plain\r\nContent-Length: %d\r\nLast-Modified: %s\r\nServer: Alex\r\n", get_file_size("./html/500.html"), filelastmodify);
            send_header(clientfd, 500, content);
            //open_send_file(clientfd, "./html/500.html");
            break;
        case 501:
            t = get_file_last_modify("./html/501.html");
            ti = localtime(&t);
            strftime(filelastmodify, 50, "%a, %e %b %Y %H:%M:%S GMT", ti);
            sprintf(content, "Content-Type: text/plain\r\nContent-Length: %d\r\nLast-Modified: %s\r\nServer: Alex\r\n", get_file_size("./html/501.html"), filelastmodify);
            send_header(clientfd, 501, content);
            //open_send_file(clientfd, "./html/501.html");
            break;
    }
    sprintf(temp, " %d %d", statuscode, datasent);
    strcat(logcontent, temp);
    printf("%s", logcontent);
    record_std(logcontent);
    return;
}

int main(int argc, char *argv[])
{
   
    int sockfd, connfd, backlog = 30, daemonflag = 0;
    struct sockaddr clientsockaddr;
    char clientip[50];
    struct sigaction sig;
    char loginusername[20];
    struct passwd *ps;
    char webpath[200] = "./", webrealpath[200], logfilename[200] = "syslog", logfilepath[200];
    char port[10] = "8080";
    FILE *config, *logfile, *errlogfile;

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
        char res[200];
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

    // check arguements
    if (argc > 1)
    {
        int i;
        for(i = 1; i < argc; i++)
        {
            if (argv[i][0] != '-')
            {
                printf("Option error\n");
                return 1;
            }  
            switch(argv[i][1])
            {
                case 'h':
                    printf("hello, Im the help text\n");
                    break;
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
                        printf("Opition error: \"-f\" need a file name\n");
                        return 0;
                    }
                    strcpy(logfilename, argv[i + 1]);
                    i++;
                    break;
                case 's':
                    //handle_method
                    break;
                    
            }
        }
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

    // open log file
    // rwx:7, rw-:6, r-x:5, r--:4, -wx:3, -w-:2, --x:1, ---:0
    mkdir("./log", 0744);
    //umask(022);
    //open log files
    sprintf(logfilepath, "./log/%s.log", logfilename);
    if (open_std_log_file(logfilepath) < 0)
    {
        printf("server error: open standard log file failed\n");
        return -1;
    }
    sprintf(logfilepath, "./log/%s.err", logfilename);
    if (open_err_log_file(logfilepath) < 0)
    {
        printf("server error: open error log file failed\n");
        return -1;
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
    //
    //check logflag 0 means use system log, 1 means use the specified file
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
        stdlog_get_current_time(formattedtime);
        printf("%s\n", formattedtime);
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
