/*************************************************************************
	> File Name: stringop.c
	> Author: 
	> Mail: 
	> Created Time: Fri 25 Sep 2015 11:42:14 PM CEST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>


int get_file_type(char *path, char *type)
{
    int i = 0;
    while(path[i] != 0)
    {
        if(path[i] == '.')
        {
            if(strlen(&path[i + 1]) > 20)
            {
                printf("server error: filetype is too long\n");
                return -2;
            }
            strcpy(type, &path[i + 1]);
            return 0;
        }
        i++;
    }
    return 1;
}

int parse_url(char *url, char *realurl, char *argu)
{
    char *temp;
    char filetype[21] = "";
    int filetypeflag;
    if((temp = strstr(url, "?")) != NULL)
    {
        strcpy(argu, temp + 1);
        *temp = 0;
    }
    realpath(url, realurl);
    if(strcmp(url, "./") == 0)
    {
        strcat(realurl, "/index.html"); 
        return 0;
    }

    if((filetypeflag = get_file_type(realurl, filetype)) == 1)
    {
        int len = strlen(realurl);
        if(realurl[len - 1] == '/')
        {
            realurl[len - 1] = 0;
        }
        strcat(realurl, ".html");
    }
    else if(filetypeflag < 0)
    {
        return filetypeflag;
    }
    return 0;
}

//find the content label such as: User-Agent: xxxx , if found a name matched in the content, return the end index of the required line.
int find_label(char *buf, char *name, char *res)
{
//    int loc = myKMP(buf, name);
    char *loc = strstr(buf, name);
    if (loc == NULL)
        return -1;
    loc = loc + strlen(name);
    int endind;
    if (*loc != 0 )
    {
        endind = get_substr(loc, res, '\n');
        return endind;
    }
    else
        return 1;
}
