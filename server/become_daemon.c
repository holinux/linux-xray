/*
 * =====================================================================================
 *
 *       Filename:  become_daemon.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/05/2011 12:43:43 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Li Hongwang (mn), hoakee@gmail.com
 *        Company:  University of Science and Technology of China
 *
 * =====================================================================================
 */

#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "become_daemon.h"

int become_daemon(int flags)
{
    int maxfd, fd;
    
    switch (fork()) {                           //become background process
        case -1: return -1;
        case 0:  break;                         //child falls through
        default: _exit(EXIT_SUCCESS);           //while parent terminates
    }

    if (setsid() == -1) return -1;              //become leader of new session

    switch (fork()) {                           //ensure we are not session leader
        case -1: return -1;
        case 0:  break;
        default: _exit(EXIT_SUCCESS);
    }

    if (!(flags & BD_NO_UMASK0))
        umask(0);                               //clear file mode creation mask

    if (!(flags & BD_NO_CHDIR))
        if (chdir("/") == -1) 
            return -1;                             //change to root directory

    if (!(flags & BD_NO_CLOSE_FILES)) {         //close all open files
        maxfd = sysconf(_SC_OPEN_MAX);
        if (maxfd == -1)                        //limit is indeterminate
            maxfd = BD_MAX_CLOSE;               //so takes a guess
        for (fd = 0; fd < maxfd; fd++)
            close(fd);
    }

    if (!(flags & BD_NO_REOPEN_STD_FDS)) {
        close(STDIN_FILENO);
        fd = open("/dev/null", O_RDWR);         //reopen standard fd's to /dev/null
        if (fd != STDIN_FILENO)                 //fd should be 0
            return -1;                          
        if (dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO)
            return -1;
        if (dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO)
            return -1;
    }

    return 0;
}




