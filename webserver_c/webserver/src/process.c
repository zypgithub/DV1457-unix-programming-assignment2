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

void handle_it_process(int clientfd, int listenfd)
{
    if (!fork())
    {
        // child process.
        close(listenfd);
        handle_it(clientfd);
        close(clientfd);
        exit(0);
    }
}

int daemon_printpid(char *path)
{
    // first step: fork off a child process
    pid_t sid, f = fork();
    if(f > 0)
    {
        //father process
        exit(0);
    }
    else if(f < 0)
    {
        // fail to fork
        perror("daemon_printpid: f < 0\n");
        exit(-1);
    }
    else
    {
        //child process;
        //Second step: Set the File opearting privilege
        //if umask set to 0, it means every body can read and write on output files. 
        //so I set it as 022, it means only owner can read and write, others only can read
        umask(022);

        //Third step: Opening Logs 

        //Forth step: Set SID
        sid = setsid();
        //fifth setp: Set working path
        chdir(path);
        // additional step: print pid
        
        pid_t pid = getpid();
        printf("The daemon process ID: %d\n", pid);
//      sixth step: close standard file descriptors
//      close(STDIN_FILENO);
//      close(STDOUT_FILENO);
//      close(STDERR_FILENO);
        freopen("./output", "awb+", stdout);
        freopen("./erroutput", "awb+", stderr);
        // final step: handle siginals
        // SIGCHLD: when a process terminate, this signal will sent to the father process
        // SIGTSTP: usually, this signal is get from ctrl + z from keyboard.
        // SIGTTOU: when a background process trying to print to terminal 
        // SIGTTIN: when a background process trying to read from terminal
        // SIGHUP: when the rerminal is disconnected, this signal will be sent to the process which has the terminal controlling 
        // SIG_IGN: ingore the catched signal
        signal(SIGCHLD,SIG_IGN); 
        signal(SIGTSTP,SIG_IGN);
        signal(SIGTTOU,SIG_IGN);
        signal(SIGTTIN,SIG_IGN);
        signal(SIGHUP,SIG_IGN);

    }
}

