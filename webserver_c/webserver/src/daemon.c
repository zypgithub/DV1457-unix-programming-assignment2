/*************************************************************************
	> File Name: daemon.c
	> Author: 
	> Mail: 
	> Created Time: Sun 27 Sep 2015 10:31:17 PM CEST
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>

int main()
{
    int d = daemon(0, 0);
    printf("%d\n", d);
    while(1)
    {
        sleep(100000);
    }
    return 0;
}
