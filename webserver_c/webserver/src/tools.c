/*************************************************************************
	> File Name: tools.c
	> Author: 
	> Mail: 
	> Created Time: Fri 25 Sep 2015 01:26:09 AM CEST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//make the next array for KMP
int KMP_get_next(char *template, int *next, int len)
{
    int i = 0, j = -1;
    memset(next, 0, sizeof(int) * len);
    next[0] = -1;
    while(i < len - 1)
    {
        if(j == -1 || template[i] == template[j])
            next[++ i] = ++ j;
        else
            j = next[j];
    }
}
// KMP matching algrightm, find the ending index where template appears in motherstring
int myKMP(char *motherstr, char *template)
{
    int i, j;
    int len_temp = strlen(template);
    int *next;
    next = (int *)malloc(sizeof(int) * len_temp);
    KMP_get_next(template, next, len_temp);
    for(i = 0, j = 0; motherstr[i] != 0 && j < len_temp; )
    {
        if(j == -1 || motherstr[i] == template[j])
        {
            i ++;
            j ++;
        }
        else
            j = next[j];
    }

    free(next);
    if(j == len_temp)
        return i - 1;
    else
        return -1;
    
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
    res[i + 1] = 0;
    return i;
}
