#include <stdio.h>        // for printf
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>        // for exit
#include <string.h>        // for bzero
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>    // for socket
#include <netinet/in.h>    // for sockaddr_in
#include <pthread.h>

#include <list.h>
#include <protocol.h>
#include <log.h>

#ifdef RUN_AS_DAEMON
#include <become_daemon.h>
#endif

int main(int argc, char **argv)
{
#ifdef RUN_AS_DAEMON
    become_daemon(0);
#endif

    //register_im();
   gpinyin_init();

   enter_ipc_loop();
   
   return 0;
}

