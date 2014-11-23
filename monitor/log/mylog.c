#include <zlog.h>
#include <assert.h>
#include "mylog.h"
zlog_category_t * log_category = NULL;
// === FUNCTION
// ======================================================================
// Name: log_init
// Description:
// 从配置文件"log.conf"中读取配置信息到内存,使用分类category初始化logger
// @param category [in]: 分类
// =====================================================================================
int log_init(const char* confile, const char *category)
{
    //初始化.配置文件名是固定的log.conf
    if (zlog_init(confile) == 1)
    {
        return -1;
    }
    //找到分类,在配置文件中的category
    log_category = zlog_get_category(category);
    if (!log_category) {
        printf("get cat fail\n");
        zlog_fini();
        return -2;
    }
    return 0 ;
}
// === FUNCTION
// ======================================================================
// Name: log_fini
// Description: 清理所有申请的内存，关闭它们打开的文件
// =====================================================================================
void log_fini()
{
    zlog_fini();
}
