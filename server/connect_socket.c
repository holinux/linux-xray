/*
 * =====================================================================================
 *
 *       Filename:  connect_socket.c
 *
 *    Description:  Communication with remote machine via socket
 *
 *        Version:  1.0
 *        Created:  07/06/2011 02:37:16 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Li Hongwang (holi), hoakee@gmail.com
 *        Company:  Novatek Microelectronics Corp.
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <log.h>
#include <list.h>
#include <protocol.h>

#define LISTEN_BACKLOG 50

static int create_socket(int* sockfd, const unsigned int port)
{
    int opt =1;
    struct sockaddr_in server_addr;

    // socket
    if ((*sockfd = socket(PF_INET,SOCK_STREAM,0)) == -1)
    {
        print_error("Create Socket Failed!");
        return -1;
    }
    setsockopt(*sockfd,SOL_SOCKET,SO_REUSEADDR,(char*)&opt,sizeof(opt));
    
    // addr
    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(port);
    
    // bind
    if (bind(*sockfd,(struct sockaddr*)&server_addr,sizeof(server_addr)) == -1)
    {
        print_error("Server Bind Port : %d Failed!", port); 
        return -1;
    }

    // listen
    if (listen(*sockfd, LISTEN_BACKLOG) == -1)
    {
        print_error("Server Listen Failed!"); 
        return -1;
    }

    return 0;
}


static int do_use_fd(int epollfd, int cfd)
{
    int nbytes;
    memset(&buf, 0, sizeof(struct imevent));

    struct imclient *clt = find_clt_by_fd(cfd);
    if (clt == NULL) {
        print_error("can not find client with sockfd = %d\n", cfd);
    }

    /* handle data from a client */
    if((nbytes = recv(cfd, &buf, sizeof(buf), 0)) <= 0)
    {
        /* got error or connection closed by client */
        if(nbytes == 0)
            print_debug("socket %d hung up\n", cfd);
        else
            print_error("recv() error");
                        
        if (epoll_ctl(epollfd, EPOLL_CTL_DEL, cfd, NULL) == -1) {
            print_error("epoll_ctl: del");
        }
        delete_client(clt);
    }
    else
    {
        dispatch_event(clt, &buf);
    }
    
    return 0;
}

#define MAX_EVENTS 16

int enter_ipc_loop()
{
    int n, flags;
    unsigned int addrlen;
    struct sockaddr_in clientaddr;
    struct epoll_event ev, events[MAX_EVENTS];
    int listen_sock, conn_sock, nfds, epollfd;
    
    if (create_socket(&listen_sock, TCP_PORT_DEFAULT) == -1) {
        print_error("create_socket\n");
        exit(EXIT_FAILURE);
    }

    if ((epollfd = epoll_create(10)) == -1) {
        print_error("epoll_create\n");
        exit(EXIT_FAILURE);
    }
    
    ev.events = EPOLLIN;
    ev.data.fd = listen_sock;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) {
        print_error("epoll_ctl: listen_sock");
        exit(EXIT_FAILURE);
    }
    
    while (1)
    {
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            print_error("epoll_pwait");
            exit(EXIT_FAILURE);
        }
        
        for (n = 0; n < nfds; ++n) {
            if (events[n].data.fd == listen_sock) {
                addrlen = sizeof(clientaddr);
                conn_sock = accept(listen_sock, (struct sockaddr *) &clientaddr, &addrlen);
                if (conn_sock == -1) {
                    print_error("accept");
                    continue;
                }
                flags = fcntl(conn_sock, F_GETFL, 0);
                fcntl(conn_sock, F_SETFL, flags|O_NONBLOCK);
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = conn_sock;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1) {
                    print_error("epoll_ctl: conn_sock");
                    continue;
                }
                register_client(conn_sock);
            } 
            else {
                do_use_fd(epollfd,  events[n].data.fd);
            }
        }
    }
}




