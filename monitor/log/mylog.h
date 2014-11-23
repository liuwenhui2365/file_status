#ifndef __LOG_H_
#define __LOG_H_

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/inotify.h>   //this header file is curial
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>

#ifdef __cplusplus
extern "C"
{
#endif
#include "zlog.h"
#ifdef __cplusplus
}
#endif

extern zlog_category_t * log_category;
// === FUNCTION ======================================================================
// Name: log_init
// Description: ¿?¿?¿?¿?¿?"log.conf"¿?¿?¿?¿?¿?¿?¿?¿?¿?¿?,¿?¿?¿?¿?category¿?¿?¿?logger
// @param category [in]: ?
// =====================================================================================
extern int log_init(const char* confile, const char *category);
// === FUNCTION ======================================================================
// Name: log_fini
// Description: 
// =====================================================================================
extern void log_fini();
//¿
#define LOG_FATAL(fmt,args...) \
        zlog(log_category, __FILE__, sizeof(__FILE__)-1, \
        __func__, sizeof(__func__)-1, __LINE__, \
        ZLOG_LEVEL_FATAL, fmt, ##args)
#define LOG_ERROR(fmt , args...) \
        zlog(log_category, __FILE__, sizeof(__FILE__)-1, \
        __func__, sizeof(__func__)-1, __LINE__, \
        ZLOG_LEVEL_ERROR, fmt, ##args)
#define LOG_WARN(fmt, args...) \
        zlog(log_category, __FILE__, sizeof(__FILE__)-1, \
        __func__, sizeof(__func__)-1, __LINE__, \
        ZLOG_LEVEL_WARN, fmt, ##args)
#define LOG_NOTICE(fmt , args...) \
        zlog(log_category, __FILE__, sizeof(__FILE__)-1, \
        __func__, sizeof(__func__)-1, __LINE__, \
        ZLOG_LEVEL_NOTICE, fmt, ##args)
#define LOG_INFO(fmt,args...) \
        zlog(log_category, __FILE__, sizeof(__FILE__)-1, \
        __func__, sizeof(__func__)-1, __LINE__, \
        ZLOG_LEVEL_INFO, fmt, ##args)
#define LOG_DEBUG(fmt , args...) \
        zlog(log_category, __FILE__, sizeof(__FILE__)-1, \
        __func__, sizeof(__func__)-1, __LINE__, \
        ZLOG_LEVEL_DEBUG, fmt, ##args)
#endif
