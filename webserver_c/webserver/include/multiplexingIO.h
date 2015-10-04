/*************************************************************************
	> File Name: multiplexingIO.h
	> Author: 
	> Mail: 
	> Created Time: Sun 04 Oct 2015 03:41:10 PM CEST
 ************************************************************************/

#ifndef _MULTIPLEXINGIO_H
#define _MULTIPLEXINGIO_H
typedef struct SockNode
{
    int socknum;
    struct SockNode *next;
}SockNode;

typedef struct SockLinklist
{
    SockNode *first;
}SockNode;
#endif
