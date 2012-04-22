/*
 * =====================================================================================
 *
 *       Filename:  log.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年07月06日 21时55分34秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Li Hongwang (mn), hoakee@gmail.com
 *        Company:  University of Science and Technology of China
 *
 * =====================================================================================
 */

#ifndef _LOG_H
#define _LOG_H

#ifdef RUN_AS_DAEMON
#include <syslog.h>
#endif

#ifdef SHOW_DEBUG_INFO

#ifdef RUN_AS_DAEMON
#define print_debug(args...)    syslog(LOG_DEBUG, args)
#else
#define print_debug(args...)    printf(args)
#endif

#else

#define print_debug(args...)

#endif

#ifdef RUN_AS_DAEMON
#define print_info(args...)     syslog(LOG_INFO, args)
#define print_error(args...)    syslog(LOG_ERR,  args)
#else
#define print_info(args...)     printf(args)
#define print_error(args...)    printf(args)
#endif

#endif
