
int main()
{
    log_init("log.conf","my_monitor");
        LOG_ERROR("error");
        LOG_WARN("warn");
        LOG_NOTICE("notice");
        LOG_DEBUG("debug");
        LOG_INFO("info");
        LOG_FATAL("fatal");
    log_fini();
    return 0;
}
