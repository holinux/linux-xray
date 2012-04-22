/*
 * =====================================================================================
 *
 *       Filename:  cmd_quit.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/14/2011 12:59:55 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Li Hongwang (holi), hoakee@gmail.com
 *        Company:  Novatek Microelectronics Corp.
 *
 * =====================================================================================
 */

/* The user wishes to quit using this program.  Just set DONE non-zero. */
int do_cmd_quit(char *arg)
{
  done = 1;
  return (0);
}

