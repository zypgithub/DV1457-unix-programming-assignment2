/*************************************************************************
	> File Name: process.c
	> Author: 
	> Mail: 
	> Created Time: Wed 23 Sep 2015 11:20:17 PM CEST
 ************************************************************************/

#include<stdio.h>
#include<string.h>
#include<signal.h>

int prevent_zombie()
{
    //prevent zombie process
    struct sigaction sig;

    sig.sa_handler = SIG_DFL;
    sig.sa_flags = SA_NOCLDWAIT;
    if( sigaction(SIGCHLD, &sig, 0) < 0 )
    {
        printf("server: sigaction\n");
        return -1;
    }
    return 0;
}
// get require method, uri and http version
int get_method(char *buf, char *method, char *uri, char *version)
{
    strcpy(uri, "./");
    sscanf(buf, "%s %s %s", method, uri + 2, version);
    printf("method: %s\nuri: %s\nversion: %s\n", method, uri, version);

}

//get head content, if found a name matched in the content, return the end index of the required line.
int get_head_content(char *buf, char *name, char *res)
{
    int loc = myKMP(buf, name);
    int endind;
    if(loc >= 0)
    {
        endind = get_line(buf + loc + 2, res);
        printf("User-Agent:%s\n",res);   
        return endind;
    }
    else
        return -1;
}

void handle_request(int clientfd, char *buf)
{
    char method[10], uri[100], version[10];
    printf("%s", buf);
    get_method(buf, method, uri, version);
}
