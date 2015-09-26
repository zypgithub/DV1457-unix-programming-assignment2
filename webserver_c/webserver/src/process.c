/*************************************************************************
	> File Name: process.c
	> Author: 
	> Mail: 
	> Created Time: Wed 23 Sep 2015 11:20:17 PM CEST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<signal.h>
#include<unistd.h>
#include<sys/stat.h>

int prevent_zombie()
{
    //prevent zombie process
    struct sigaction sig;

    sig.sa_handler = SIG_DFL;
    sig.sa_flags = SA_NOCLDWAIT;
    if (sigaction(SIGCHLD, &sig, 0) < 0 )
    {
        printf("server: sigaction\n");
        return -1;
    }
    return 0;
}


void handle_it(int clientfd)
{
     if (!fork())
    {
        // child process.
        char method[10], url[100], version[10];
        char res[50];
        char buf[10000] = "";
        int recv_flag;
        char argu[200];
        char parsedurl[100];
        FILE *f;

        if((recv_flag = recv_data(clientfd, buf)) < 0)
        {
            printf("server error: recv_data: %d\n", recv_flag);
            close(clientfd);
            exit(-1);
        }
        char *requirebody = get_method(buf, method, url, version);
        printf("Require method: %s\nRequire url: %s\nHttp version: %s\n", method, url, version);

        char requirecontent[10000];
        get_substr(requirebody, requirecontent, -1);
        printf("require content:\n%s", requirecontent);
        //find_label(buf, "User-Agent: ", res);

        //printf("Im the child\n");
        int validflag;
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
                    struct stat filestat;
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
            {
                validflag = 501;
            }
        }
        switch(validflag)
        {
            case 400:
                send_header(clientfd, 400, "text/html", 13);
                send_data(clientfd, "Bad Required!", 13);
                break;
            case 403:
                send_header(clientfd, 403, "text/html", 10);
                send_data(clientfd, "Forbidden!", 10);
                break;
            case 404:
                send_header(clientfd, 404, "text/html", 10);
                send_data(clientfd, "Not Found!", 10);
                break;
            case 500:
                send_header(clientfd, 500, "text/html", 23);
                send_data(clientfd, "Internal Server Error!", 23);
                break;
            case 501:
                send_header(clientfd, 501, "text/html", 16);
                send_data(clientfd, "Not Implemented!", 16);
                break;
        }
        close(clientfd);
        //printf("Im the child, I have finished\n");
        exit(0);
    }
}
