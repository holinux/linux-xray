/*
 * =====================================================================================
 *
 *       Filename:  do_cmd.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/14/2011 12:44:33 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Li Hongwang (holi), hoakee@gmail.com
 *        Company:  Novatek Microelectronics Corp.
 *
 * =====================================================================================
 */


int do_cmd_pageflags(char *arg)
{
    unsigned long pfn = strtol(arg);
    send_cmd(fd, pfn)


}

