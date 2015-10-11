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
#include"string.h"


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

int split_filename_path(char *url, char *filename, char *path)
{
    int len = strlen(url);
    int i = len - 1;
    if (url[i] == '/')
        return 1;
    for (; i > 0; i--)
    {
        if (url[i] == '/') 
        {
            sprintf(filename, "%s", &url[i + 1]);
            int j;
            for (j = 0; j < i; j++)
            {
                path[j] = url[j];
            }
            path[j] = 0;
        }
    }
}

int parse_url(char *url, char *realurl, char *argu)
{
    char *temp;
    char tempurl[200];
    char filetype[21] = "";
    int filetypeflag;
    char workpath[200];
    sprintf(tempurl, ".%s", url);
    if ((temp = strstr(tempurl, "?")) != NULL)
    {
        strcpy(argu, temp + 1);
        *temp = 0;
        return 400;
    }
    realpath(tempurl, realurl);
    realpath("./", workpath);

    if(strstr(realurl, workpath) == NULL)
    {
        //printf("Parse_url: client is trying to access a path out of area\n");
        return 403;
    }


    if (strcmp(tempurl, "./") == 0)
    {
        strcat(realurl, "/html/index.html"); 
        return 0;
    }

    if ((filetypeflag = get_file_type(realurl, filetype)) == 1)
    {
        int len = strlen(realurl);
        if(realurl[len - 1] == '/')
        {
            realurl[len - 1] = 0;
        }
        strcat(realurl, ".html");
    }
    else if (filetypeflag < 0)
    {
        return filetypeflag;
    }
    
    return 0;
}


//get substring from start to a \n or any character, return the starting index where the end of the line. 
int get_substr(char *str, char *res, char endat)
{
    int i = 0;
    while(str[i] != 0 && str[i] != endat)
    {
        res[i] = str[i];
        i ++;
    }
    res[i] = 0;
    //printf("%s\n", res)
    return i;
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

