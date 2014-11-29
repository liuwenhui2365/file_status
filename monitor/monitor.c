#include "monitor.h"

void monitor_dirs_init(struct monitor_dirs **pNode)//two level point
{
    //be careful! because *pNode is empty,so it can't read **pNode
    *pNode = malloc(sizeof(struct monitor_dirs));
    if (*pNode == NULL)
    {
        ZLOG_ERROR("malloc error\n");
        exit(EXIT_FAILURE);
    }
    memset(*pNode, 0, sizeof(struct monitor_dirs));
}

struct monitor_dirs* monitor_dirs_travel(struct monitor_dirs* head)
{
    struct monitor_dirs* p = NULL;
    p = head->next;  //curial sentence
    while(NULL != p)
    {
        head = p;     //this is very important
        p = p->next;
    }

    return head;
}

char* monitor_dirs_search(struct monitor_dirs* head,int dir_wd)
{
    struct monitor_dirs* p = NULL;
    p = head;  //curial sentence
    while(NULL != p)
    {
        if(dir_wd==p->dir_wd)  //think about "=" and "=="
        {
      //    ZLOG_DEBUG("find abs_path: %s, dir_wd:%d\n",p->abs_path,p->dir_wd);
            return p->abs_path;
        }
        else
        {
             p = p->next;
        }
    }
    ZLOG_WARN("can't find the abs_path!\n");
    return NULL;
}

void* monitor_dirs_remove(struct monitor_dirs* head,int fd,char* path)
{
    struct monitor_dirs* p = NULL;
    p = head->next;  //curial sentence
    while(NULL != p)
    {
        if(strcmp(path,p->abs_path)==0)  //think about "=" and "=="
        {
            ZLOG_NOTICE("remove a monitor_node's abs_path: %s, dir_wd:%d\n",p->abs_path,p->dir_wd);
            inotify_rm_watch(fd, p->dir_wd);
            head->next = p->next;
            free(p);
            p = NULL;
         }
        else
        {
             head = p;
             p = p->next;
        }
    }

    return head;
}

void monitor_bydirs_remove(struct monitor_dirs *head,int fd,char* path)
{
    struct monitor_dirs* cur = NULL;
    struct monitor_dirs* cur_temp = NULL;
    struct monitor_dirs* pre = NULL;
    pre = head;
    cur = head->next;  //curial sentence
    while(NULL != cur)
    {
        if(strlen(cur->abs_path)<strlen(path))
        {
            pre = cur;
            cur = cur->next;
            cur_temp = cur;
        }
        else if(strlen(cur->abs_path)>=strlen(path))
        {
            int path_len = strlen(path);
            if(strncmp(path,cur->abs_path,path_len) == 0) //think about "=" and "=="
            {
                ZLOG_INFO("abs_path: %s,dir_wd: %d\n",cur->abs_path,cur->dir_wd);
                inotify_rm_watch(fd, cur->dir_wd);
                //deep understand them
                cur = cur->next;
                pre->next = cur;
                free(cur_temp);
                cur_temp = NULL;
            }
            else
            {
                pre = cur;
                cur = cur->next;
                cur_temp = cur;
            }
        }
    }
}
void monitor_dirs_dump(struct monitor_dirs *head)
{
    ZLOG_DEBUG("dump monitor dirs: \n");
    struct monitor_dirs* p = NULL;
    p = head->next;
    while(NULL != p)
    {
        ZLOG_DEBUG("abs_path: %s, dir_wd:%d\n",p->abs_path,p->dir_wd);
        //head = p;
        p = p->next;
    }
}

void monitor_dirs_delete(struct monitor_dirs *head)
{
    struct monitor_dirs* cur = NULL;
    if(head == NULL)
    {
        return ;
    }
    cur = head->next;  //curial sentence
    while(NULL != cur)
    {
        free(head);
        head = cur;
        cur = cur->next;
    }
    monitor_dirs_dump(head);
}


void* monitor_dirs_add(struct monitor_dirs* head,
                       struct monitor_dirs* new_node,
                       char   *abs_path,int dir_wd)
{
    struct monitor_dirs* tail = monitor_dirs_travel(head);
    tail->next = new_node;
    strncpy(new_node->abs_path,abs_path,strlen(abs_path));//how about "="
    new_node->dir_wd = dir_wd;
    new_node->next = NULL;

    return ;
}

void monitor_list_init(struct monitor_node **pNode,char *name)//two level point
{
    //be careful! because *pNode is empty,so it can't read **pNode
    *pNode = malloc(sizeof(struct monitor_node));
    if (*pNode == NULL)
    {
        ZLOG_ERROR("malloc error\n");
        exit(EXIT_FAILURE);
    }
    memset(*pNode, 0, sizeof(struct monitor_node));

   // ZLOG_DEBUG("name is %s\n",name);
}

struct monitor_node* TravelList(struct monitor_node *head)
{
    struct monitor_node* p = NULL;
    p = head->next;  //curial sentence
    while(NULL != p)
    {
        head = p;     //this is very important
        p = p->next;
    }

    return head;
}

void moniitor_node_remove(struct monitor_node *head,char* path)
{
    struct monitor_node* p = NULL;
    p = head->next;  //curial sentence
    while(NULL != p)
    {
        if(strcmp(path,p->path) == 0)  //think about "=" and "=="
        {
            ZLOG_INFO("path : %s, type : %c\n",p->path,p->type);
            head->next = p->next;
            free(p);
            p = NULL;
         }
        else
        {
             head = p;
             p = p->next;
        }
    }
}

void moniitor_node_remove_bydir(struct monitor_node *head,char* path)
{
    struct monitor_node* cur = NULL;
    struct monitor_node* cur_temp = NULL;
    struct monitor_node* pre = NULL;
    cur = head->next;  //curial sentence
    pre = head;
    while(NULL != cur)
    {
        if(strlen(cur->path)<strlen(path))
        {
            pre = cur;
            cur = cur->next;
            cur_temp = cur;
        }
        else if(strlen(cur->path)>=strlen(path))
        {
            int a = strlen(path);
            if(strncmp(path,cur->path,a) == 0) //think about "=" and "=="
            {
                ZLOG_INFO("path : %s, type : %c\n",cur->path,cur->type);
                cur = cur->next;
                pre->next = cur;
                free(cur_temp);
                cur_temp = NULL;
            }
            else
            {
                pre = cur;
                cur = cur->next;
                cur_temp = cur;
            }
        }
    }
}

void* monitor_list_dump(struct monitor_node *head)
{
    struct monitor_node* p = NULL;
    p = head->next;
    while(NULL != p)
    {
        ZLOG_DEBUG("path: %s\ttype: %c\n",p->path,p->type);
        head = p;
        p = p->next;
    }

    return ;
}


void monitor_delete(struct monitor_node *head)
{
    struct monitor_node* cur = NULL;
    if(head != NULL)
    {
        cur = head->next;  //curial sentence
        while(NULL != cur)
        {
            free(head);
            head = cur;
            cur = cur->next;
        }
    }
    monitor_list_dump(head);
}


char _search_file_type(struct monitor_node* head,char* path)
{
    struct monitor_node* p = NULL;
    p = head;  //curial sentence
    while(NULL != p)
    {
        if(strcmp(path,p->path)==0)  //be careful character comparsion,can't use '=='
        {
      //    ZLOG_DEBUG("find abs_path: %s, dir_wd:%d\n",p->abs_path,p->dir_wd);
            return p->type;
        }
        else
        {
            head = p;
            p = p->next;
        }
    }
    return 0;
}

char search_file_type(struct monitor_node* head,const char* path)
{
    char p[MAX_PATH_LENGTH] ={0};
    strcpy(p,path);
    //ZLOG_DEBUG("path is %s\n",p);
    char r = _search_file_type(head,p);
    if(r=='f')
    {
        return r;
    }
    else
    {
        strcat(p,"/");  //this is "" not ''
       // ZLOG_DEBUG("after the path add '/' is %s\n",p);
        char r = _search_file_type(head,p);
        if(r=='d')
        {
            return r;
        }
        else
        {
            return 0;
        }
    }
}


void* monitor_node_add(struct monitor_node* h, struct monitor_node *Node,char *path,char type)
{
    struct monitor_node *tail = TravelList(h);
    tail->next = Node;
/*
    Node->type = malloc(sizeof(char*));
    if(Node->type == NULL)
    {
        ZLOG_DEBUG("malloc error\n");
        exit(EXIT_FAILURE);
    }
 */
    strncpy(Node->path,path,strlen(path));//how about "="
    Node->type = type;
    Node->next = NULL;

    return ;
}

char listDir(const char *path,struct monitor_node *head,
             char *sign,int fd,
             struct monitor_dirs* monitor_dirs_head)
{
    DIR              *pDir ;
    struct dirent    *ent  ;
    char             abspath[512];
    char             childpath[512];
    int              dir_wd;
    pDir=opendir(path);
 //   memset(childpath,0,sizeof(childpath));
 //   memset(abspath,0,sizeof(abspath));
 //   realpath(path,childpath);

    while((ent=readdir(pDir))!=NULL)
    {
        memset(childpath,0,sizeof(childpath));
        memset(abspath,0,sizeof(abspath));
        realpath(path,childpath);

        if(ent->d_type & DT_DIR)
        {

            if(strcmp(ent->d_name,".")==0 || strcmp(ent->d_name,"..")==0)
            {
                continue;
            }
         //    ZLOG_DEBUG("dir:%s/%s\n",childpath,ent->d_name);
            strcat(abspath,path);
            strcat(abspath,"/");
            strcat(abspath,ent->d_name);
     //       ZLOG_DEBUG("%s",abspath);
            strcat(childpath,"/");
            strcat(childpath,ent->d_name);
            if(sign=="add")
            {
                //it's directory
                dir_wd = inotify_add_watch(fd,childpath,\
                       IN_CREATE | IN_ISDIR | IN_DELETE |\
                       IN_MOVED_TO | IN_MOVED_FROM | IN_MODIFY);
                if(dir_wd == -1)
                {
                    perror("inotify_add_watch failed\n");
                    ZLOG_INFO("system will continue.\n");
                  //  goto exit;
                }
                ZLOG_INFO("new watch dir_wd :%d\n",dir_wd);
                strcat(childpath,"/");
                ZLOG_NOTICE("watch dir:%s\n",childpath);
                struct monitor_dirs* new_node;
                monitor_dirs_init(&new_node);
                monitor_dirs_add(monitor_dirs_head,new_node,childpath,dir_wd);
                struct monitor_node *name;
                monitor_list_init(&name,ent->d_name);
                if(sign=="add")
                {
                    monitor_node_add(head,name,childpath,'d');
                    listDir(abspath,head,"add",fd,monitor_dirs_head);
                }
                }
                if(sign=="sub")
                {
                    moniitor_node_remove(head,childpath);
                    listDir(abspath,head,"sub",fd,monitor_dirs_head);
                }
     //        ZLOG_DEBUG("list directory.\n");
      //       ZLOG_DEBUG(head);

        }
        else
        {
            //it's file
             //ZLOG_DEBUG(childpath,"%s\n",ent->d_name);
             //ZLOG_DEBUG("file:%s/%s\n",childpath,ent->d_name);
            struct monitor_node *name;
            monitor_list_init(&name,ent->d_name);
            strcat(childpath,"/");
            strcat(childpath,ent->d_name);
            if(sign=="add")
            {
                monitor_node_add(head,name,childpath,'f');
            }
            if(sign=="sub")
            {
                moniitor_node_remove(head,childpath);
            }
            ZLOG_DEBUG("list file.\n");
            monitor_list_dump(head);
        }
        ZLOG_DEBUG("while interior.\n");
        monitor_list_dump(head);
    }
    ZLOG_DEBUG("result is\n");
    monitor_list_dump(head);
}

int start_monitor(const char *dirname)
{

    //add my_monitor_log
    int ret = log_init("./log/log.conf","my_monitor");
    if (ret < 0)
    {
        ZLOG_ERROR("init log failed.");
        exit(1);
    }
    ZLOG_INFO("init log success.");

    int len = 0;
    int fd,dir_wd;
    char buf[EVENT_BUFSIZE];
    char *cur = buf;
    char *end;
    char abs_curdir[MAX_PATH_LENGTH];

//    getcwd(abs_curdir,MAX_PATH_LENGTH);
    realpath(dirname,abs_curdir);
    strncat(abs_curdir,"/",2);

    ZLOG_INFO("\nwelcome to use monitor filesystem!\n\n");
    ZLOG_INFO("our monitor system is used to monitor the file or directory.\n\n");

    struct monitor_node* monitor_head;
    monitor_list_init(&monitor_head,"first_node");
    ZLOG_INFO("first node monitor_header success.\n");

    struct monitor_dirs* monitor_dirs_head;
    monitor_dirs_init(&monitor_dirs_head);
    ZLOG_INFO("monitor_dirs first node create success.\n");


    fd = inotify_init();
    if(fd == 1)
    {
        perror("inotify_init failed\n");
        return -1;
    }
    ZLOG_NOTICE("fd is %d\n",fd);

    dir_wd = inotify_add_watch(fd,dirname,
        IN_CREATE | IN_ISDIR | IN_DELETE |
        IN_MOVED_TO | IN_MOVED_FROM |
        IN_MODIFY | IN_CLOSE_WRITE );
    if(dir_wd == -1)
    {
        perror("inotify_add_watch failed\n");
        goto exit;
    }
    ZLOG_DEBUG("new watch dir_wd :%d\n",dir_wd);
    struct monitor_dirs* new_monitor_dirs;
    monitor_dirs_init(&new_monitor_dirs);
    //add monitor_dirs of first node
    monitor_dirs_add(monitor_dirs_head,new_monitor_dirs,abs_curdir,dir_wd);
    listDir(dirname,monitor_head,"add",fd,monitor_dirs_head);
    ZLOG_INFO("The directory has some file or directory already:\n");
    monitor_list_dump(monitor_head);
    monitor_dirs_dump(monitor_dirs_head);
    ZLOG_DEBUG("the directory of monitor is empty!\n");

    struct stat s;
    ZLOG_INFO("filesystem init finish.\n\n");

    while(1)
    {
        len = read(fd,cur,EVENT_BUFSIZE - len);
        if(len <= 0)
        {
            perror("read inotify event failed\n");
            goto exit;
        }

        end = cur + len;
        ZLOG_DEBUG("end is %s\n",end);
        ZLOG_DEBUG("cur is %s\n",cur);
        ZLOG_DEBUG("len is %d\n",len);

        while(cur + sizeof(struct inotify_event) <= end)
        {
            struct inotify_event* e = (struct inotify_event*)cur;
            if(cur + sizeof(struct inotify_event) + e->len > end)
            {
                break;
            }
            ZLOG_DEBUG("cur : %p\n",cur + sizeof(struct inotify_event));
            ZLOG_DEBUG("end : %p\n",end);

            //get the abs_path of current operation accord to wd
            //use a middle variable to store the value is imporant
            ZLOG_INFO("wd is %d\n",e->wd);
            char* match_dir = monitor_dirs_search(monitor_dirs_head,e->wd);
            //curial step or will exit the system
            if (match_dir == NULL)
            {
                ZLOG_DEBUG("search is empty, continue\n");
                break;
            }
            //  monitor_dirs_dump(monitor_dirs_head);
            char relat[MAX_PATH_LENGTH] = {0};
            strcat(relat,match_dir);
            strcat(relat,e->name);
            //the '/' is imporant to add dir_wd
            //ZLOG_DEBUG("{relat}:%s\n",relat);
            if(e->mask & IN_CREATE)
            {
                ZLOG_INFO("CREATE\n");
                //ZLOG_INFO("relat is %s\n",relat);
                if( stat(relat,&s) == 0 )
                {
        //            ZLOG_DEBUG("Mode: %lo (octal)\n", (unsigned long) s.st_mode);
                    if( s.st_mode & S_IFDIR )
                    {
                        ZLOG_DEBUG("is  dir\n");
                        //be careful position
                        strcat(relat,"/");
                        //it's a directory
                        //get the abspath of create directory
                        //add watch and get the dir_wd
                        dir_wd = inotify_add_watch(fd,relat,
                                IN_CREATE | IN_ISDIR | IN_DELETE |\
                                IN_MOVED_TO | IN_MOVED_FROM |\
                                IN_MODIFY);
                        if(dir_wd == -1)
                        {
                            perror("inotify_add_watch failed\n");
                           // goto exit;
                        }
                      //create new file_node and add to the list
                        struct monitor_node *name;
                        monitor_list_init(&name,e->name);
                      //  ZLOG_DEBUG("wd is %d\n",e->wd);
                        monitor_node_add(monitor_head,name,relat,'d');
                        ZLOG_DEBUG("add directory's path: %s, type: %c\
                            success.\n",name->path,name->type);
                        //create new monitor_node and add to the monitor_dirs
                       // ZLOG_DEBUG("new watch dir_wd :%d\n",dir_wd);
                     //   ZLOG_DEBUG("watch dir_path: %s\n",relat);
                        struct monitor_dirs* new_monitor_dirs;
                        monitor_dirs_init(&new_monitor_dirs);
                        //add new abs_path of creat directory
                        monitor_dirs_add(monitor_dirs_head,new_monitor_dirs,relat,dir_wd);
                       // ZLOG_DEBUG("wd is %d\n",e->wd);
                      //  ZLOG_DEBUG("add directory_monitor success!\n");
                      //  monitor_dirs_dump(monitor_dirs_head);
                    }
                    else if( s.st_mode & S_IFREG )
                    {
                        //it's a file
                        //create new directory_node and add to the list
                        ZLOG_DEBUG("is file\n");
                        struct monitor_node *name;
                        monitor_list_init(&name,e->name);
                    //    ZLOG_DEBUG("relat is %s\n",relat);
                        monitor_node_add(monitor_head,name,relat,'f');
                        ZLOG_INFO("add path: %s\t type: %c success.\n",
                               name->path,name->type);
                    }
                    else
                    {
                       //something else
                        ZLOG_NOTICE("unknown types.\n");
                    }
                }
                else
                {
                    //error
                    perror("stat is wrong when create.\n");
                  //  return 1;
                }
            }
            if(e->mask & IN_DELETE)
            {
                ZLOG_DEBUG("DELETE\n");
                ZLOG_DEBUG("relat is %s\n",relat);
                if(search_file_type(monitor_head,relat)=='d')
                {
                    //it's a directory
                    ZLOG_DEBUG("is dir\n");
                    strcat(relat,"/");
                    ZLOG_DEBUG("delete the directory's ");
                //  ZLOG_DEBUG("delete the dir_wd is %d\n",dir_wd);
                //  ZLOG_DEBUG("e->wd is %d\n",e->wd);
                    moniitor_node_remove_bydir(monitor_head,relat);
                    monitor_bydirs_remove(monitor_dirs_head,fd,relat);
                    ZLOG_INFO("delete monitor_node of directory_monitor success!\n");
                    monitor_dirs_dump(monitor_dirs_head);
                }
                else if(search_file_type(monitor_head,relat)=='f')
                {
                    //it's a file
                    //create new directory_node and add to the list
                    ZLOG_INFO("is file\n");
                    ZLOG_INFO("delete the file's ");
                    moniitor_node_remove(monitor_head,relat);
                }
                else
                {
                    //something else
                  //  goto exit;
                    ZLOG_WARN("can't recogninze the filetype.\n");
                    ZLOG_NOTICE("system will continue.");
                }
            }
            if(e->mask & IN_MOVED_TO)
            {
                ZLOG_DEBUG("MOVED FROM OTHER PLACE.\n");
                //strcat(relat,"/");
                //it's a directory
                //get the abspath of create directory
                //add watch and get the dir_wd
                dir_wd = inotify_add_watch(fd,relat,
                        IN_CREATE | IN_ISDIR | IN_DELETE |\
                        IN_MOVED_TO | IN_MOVED_FROM |\
                        IN_MODIFY );
                if(dir_wd == -1)
                {
                    perror("inotify_add_watch failed\n");
                    goto exit;
                }
              //  ZLOG_DEBUG("relat is %s\n",relat);
                if( stat(relat,&s) == 0 )
                {
                    if( s.st_mode & S_IFDIR )
                    {
                        //it's a directory
                        ZLOG_DEBUG("is dir\n");
                        strcat(relat,"/");

                        struct monitor_node *name;
                        monitor_list_init(&name,e->name);
                        monitor_node_add(monitor_head,name,relat,'d');
                        ZLOG_DEBUG("add directory's path: %s, type: %c success.\n",name->path,name->type);

                        struct monitor_dirs* new_monitor_dirs;
                        monitor_dirs_init(&new_monitor_dirs);
                        monitor_dirs_add(monitor_dirs_head,new_monitor_dirs,relat,dir_wd);
                        listDir(relat,monitor_head,"add",fd,monitor_dirs_head);
                    }
                    else if( s.st_mode & S_IFREG )
                    {
                        //it's a file
                        ZLOG_DEBUG("is file\n");
                        struct monitor_node *name;
                        monitor_list_init(&name,e->name);
                        monitor_node_add(monitor_head,name,relat,'f');
                    }
                    else
                    {
                       //something else
                        ZLOG_ERROR("unknown types.\n");
                    }
                }
                else
                {
                    //error
                    perror("stat is wrong when moved.\n");
                    return 1;
                }
                ZLOG_INFO("moved here result is:\n");
                monitor_dirs_dump(monitor_dirs_head);
                monitor_list_dump(monitor_head);
            }
            if(e->mask & IN_MOVED_FROM)
            {
                ZLOG_DEBUG("MOVED TO OTHER PLACE.\n");
                //ZLOG_DEBUG("relat is %s\n",relat);
                //strcat(relat,"/");
                if(search_file_type(monitor_head,relat)=='d')
                {
                    //it's a directory
                    ZLOG_DEBUG("is dir\n");
                    strcat(relat,"/");
                    ZLOG_INFO("moved the directory's ");
                 //  ZLOG_DEBUG("delete the dir_wd is %d\n",dir_wd);
                 //  ZLOG_DEBUG("e->wd is %d\n",e->wd);
                    moniitor_node_remove_bydir(monitor_head,relat);
                    monitor_bydirs_remove(monitor_dirs_head,fd,relat);
                    ZLOG_INFO("moved out monitor_node of directory_monitor success!\n");
                    monitor_dirs_dump(monitor_dirs_head);
                }
                else if(search_file_type(monitor_head,relat)=='f')
                {
                    //it's a file
                    //create new directory_node and add to the list
                    ZLOG_INFO("is file\n");
                    ZLOG_DEBUG("moved out the file's ");
                    moniitor_node_remove(monitor_head,relat);
                }
                else
                {
                    //something else
                    goto exit;
                }
                ZLOG_DEBUG("moved out result is:\n");
                monitor_dirs_dump(monitor_dirs_head);
                monitor_list_dump(monitor_head);
            }
            if(e->mask & IN_MODIFY)
            {
               // ZLOG_DEBUG("relat is %s\n",relat);
                ZLOG_DEBUG("file %s is modified.\n",relat);
            }
           /* if(e->mask & IN_CLOSE_WRITE)
            {
               // ZLOG_DEBUG("relat is %s\n",relat);
                ZLOG_DEBUG("file %s is modified success.\n",relat);
            }
              else
                {
                    //error
                    ZLOG_DEBUG("others stat is wrong.\n");
                // getcwd(e->name,MAX_PATH_LENGTH));
                }*/
            //    ZLOG_DEBUG("after add name {relat} is %s\n",relat);
            cur += sizeof(struct inotify_event) + e->len;
        }

        if(cur >= end)
        {
            cur = buf;
            len =0;
        }
        else
        {
            len = end - cur;
            memmove(buf, cur,len);
            cur = buf + len;
        }
    }

exit:    //be careful this format.
    close(fd);
    monitor_dirs_delete(monitor_dirs_head);                     //clear the list
    monitor_delete(monitor_head);
    log_fini();
    return 0;
}

