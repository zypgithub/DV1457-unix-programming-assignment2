/*************************************************************************
	> File Name: tools.c
	> Author: 
	> Mail: 
	> Created Time: Fri 25 Sep 2015 01:26:09 AM CEST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

void get_current_time(struct tm **ti)
{
    time_t t;
    t = time(NULL);
    *ti = localtime(&t);
}

void stdlog_get_current_time(char *formattedtime)
{
    struct tm *ti;
    get_current_time(&ti);
    strftime(formattedtime, 50, "[%d/%b/%Y:%H:%M:%S %z]", ti);
}

void errlog_get_current_time(char *formattedtime)
{
    struct tm *ti;
    get_current_time(&ti);
    strftime(formattedtime, 50, "[%a %b %d %H:%M:%S %Y]", ti);
}
