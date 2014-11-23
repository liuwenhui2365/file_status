#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/inotify.h>   //this header file is curial
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include "./log/mylog.h"

#define EVENT_BUFSIZE 4096
#define MAX_PATH_LENGTH 256

struct monitor_dirs
{
    char abs_path[MAX_PATH_LENGTH];
    int dir_wd;
    struct monitor_dirs *next;
};   //The Node of List

void monitor_dirs_init(struct monitor_dirs **pNode)//two level point
{
    //be careful! because *pNode is empty,so it can't read **pNode
    *pNode = malloc(sizeof(struct monitor_dirs));
    if (*pNode == NULL)
    {
        LOG_ERROR("malloc error\n");
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
      //    LOG_DEBUG("find abs_path: %s, dir_wd:%d\n",p->abs_path,p->dir_wd);
            return p->abs_path;
        }
        else
        {
             p = p->next;
        }
    }
    LOG_WARN("can't find the abs_path!\n");
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
            LOG_NOTICE("remove a monitor_node's abs_path: %s, dir_wd:%d\n",p->abs_path,p->dir_wd);
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
                LOG_INFO("abs_path: %s,dir_wd: %d\n",cur->abs_path,cur->dir_wd);
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
    LOG_INFO("dump monitor dirs: \n");
    struct monitor_dirs* p = NULL;
    p = head->next;
    while(NULL != p)
    {
        LOG_INFO("abs_path: %s, dir_wd:%d\n",p->abs_path,p->dir_wd);
        //head = p;
        p = p->next;
    }
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


struct monitor_node
{
    char path[MAX_PATH_LENGTH];
    char type;
    struct monitor_node *next;
};   //The Node of List

void monitor_list_init(struct monitor_node **pNode,char *name)//two level point
{
    //be careful! because *pNode is empty,so it can't read **pNode
    *pNode = malloc(sizeof(struct monitor_node));
    if (*pNode == NULL)
    {
        LOG_ERROR("malloc error\n");
        exit(EXIT_FAILURE);
    }
    memset(*pNode, 0, sizeof(struct monitor_node));

   // LOG_DEBUG("name is %s\n",name);
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
            LOG_INFO("path : %s, type : %c\n",p->path,p->type);
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
                LOG_INFO("path : %s, type : %c\n",cur->path,cur->type);
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

char _search_file_type(struct monitor_node* head,char* path)
{
    struct monitor_node* p = NULL;
    p = head;  //curial sentence
    while(NULL != p)
    {
        if(strcmp(path,p->path)==0)  //be careful character comparsion,can't use '=='
        {
      //    LOG_DEBUG("find abs_path: %s, dir_wd:%d\n",p->abs_path,p->dir_wd);
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
    //LOG_DEBUG("path is %s\n",p);
    char r = _search_file_type(head,p);
    if(r=='f')
    {
        return r;
    }
    else
    {
        strcat(p,"/");  //this is "" not ''
       // LOG_DEBUG("after the path add '/' is %s\n",p);
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


void* print_monitor_list(struct monitor_node *head)
{
    struct monitor_node* p = NULL;
    p = head->next;
    while(NULL != p)
    {
        LOG_INFO("path: %s\ttype: %c\n",p->path,p->type);
        head = p;
        p = p->next;
    }

    return ;
}


void* monitor_node_add(struct monitor_node* h, struct monitor_node *Node,char *path,char type)
{
    struct monitor_node *tail = TravelList(h);
    tail->next = Node;
/*
    Node->type = malloc(sizeof(char*));
    if(Node->type == NULL)
    {
        LOG_DEBUG("malloc error\n");
        exit(EXIT_FAILURE);
    }
 */
    strncpy(Node->path,path,strlen(path));//how about "="
    Node->type = type;
    Node->next = NULL;

    return ;
}

char listDir(char *path,struct monitor_node *a,
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
         //    LOG_DEBUG("dir:%s/%s\n",childpath,ent->d_name);
            strcat(abspath,path);
            strcat(abspath,"/");
            strcat(abspath,ent->d_name);
     //       LOG_DEBUG("%s",abspath);
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
                    LOG_INFO("system will continue.\n");
                  //  goto exit;
                }
                LOG_INFO("new watch dir_wd :%d\n",dir_wd);
                strcat(childpath,"/");
                LOG_NOTICE("watch dir:%s\n",childpath);
                struct monitor_dirs* new_node;
                monitor_dirs_init(&new_node);
                monitor_dirs_add(monitor_dirs_head,new_node,childpath,dir_wd);
                struct monitor_node *name;
                monitor_list_init(&name,ent->d_name);
                if(sign=="add")
                {
                    monitor_node_add(a,name,childpath,'d');
                    listDir(abspath,a,"add",fd,monitor_dirs_head);
                }
                }
                if(sign=="sub")
                {
                    moniitor_node_remove(a,childpath);
                    listDir(abspath,a,"sub",fd,monitor_dirs_head);
                }
     //        LOG_DEBUG("list directory.\n");
      //       LOG_DEBUG(a);

        }
        else
        {
            //it's file
             //LOG_DEBUG(childpath,"%s\n",ent->d_name);
             //LOG_DEBUG("file:%s/%s\n",childpath,ent->d_name);
            struct monitor_node *name;
            monitor_list_init(&name,ent->d_name);
            strcat(childpath,"/");
            strcat(childpath,ent->d_name);
            if(sign=="add")
            {
                monitor_node_add(a,name,childpath,'f');
            }
            if(sign=="sub")
            {
                moniitor_node_remove(a,childpath);
            }
        //     LOG_DEBUG("list file.\n");
        //     LOG_DEBUG(a);
        }
    //    LOG_DEBUG("while interior.\n");
    //    LOG_DEBUG(a);
    }
   // LOG_DEBUG("result is\n");
   // LOG_DEBUG(a);
}



int main(int argc, char **argv)
{

    //add my_monitor_log
    log_init("./log/log.conf","my_monitor");
    LOG_ERROR("error");

    int len = 0;
    int fd,dir_wd;
    const char* dirname = argv[1];
    if(argc != 2)
    {
        LOG_ERROR("PROGRAM: missing dirname\n");
        LOG_FATAL("Usage: PROGRAM absolute-dirname \n");
        exit(1);
    }
    char buf[EVENT_BUFSIZE];
    char *cur = buf;
    char *end;
    char abs_curdir[MAX_PATH_LENGTH];

//    getcwd(abs_curdir,MAX_PATH_LENGTH);
    realpath(dirname,abs_curdir);
    strncat(abs_curdir,"/",2);

    LOG_INFO("\nwelcome to use monitor filesystem!\n\n");
    LOG_INFO("our monitor system is used to monitor the file or directory.\n\n");

    struct monitor_node* monitor_head;
    monitor_list_init(&monitor_head,"first_node");
    LOG_DEBUG("first node monitor_header success.\n");

    struct monitor_dirs* monitor_dirs_head;
    monitor_dirs_init(&monitor_dirs_head);
    LOG_INFO("monitor_dirs first node create success.\n");


    fd = inotify_init();
    if(fd == 1)
    {
        perror("inotify_init failed\n");
        return -1;
    }
    LOG_NOTICE("fd is %d\n",fd);

    dir_wd = inotify_add_watch(fd,dirname,
        IN_CREATE | IN_ISDIR | IN_DELETE |
        IN_MOVED_TO | IN_MOVED_FROM |
        IN_MODIFY | IN_CLOSE_WRITE );
    if(dir_wd == -1)
    {
        perror("inotify_add_watch failed\n");
        goto exit;
    }
    LOG_DEBUG("new watch dir_wd :%d\n",dir_wd);
    struct monitor_dirs* new_monitor_dirs;
    monitor_dirs_init(&new_monitor_dirs);
    //add monitor_dirs of first node
    monitor_dirs_add(monitor_dirs_head,new_monitor_dirs,abs_curdir,dir_wd);
    listDir(argv[1],monitor_head,"add",fd,monitor_dirs_head);
    LOG_INFO("The directory has some file or directory already:\n");
    print_monitor_list(monitor_head);
    monitor_dirs_dump(monitor_dirs_head);
  //  LOG_DEBUG("the directory of monitor is empty!\n");

    struct stat s;
    LOG_DEBUG("filesystem init finish.\n\n");

    while(1)
    {
        len = read(fd,cur,EVENT_BUFSIZE - len);
        if(len <= 0)
        {
            perror("read inotify event failed\n");
            goto exit;
        }

        end = cur + len;
    //    LOG_DEBUG("end is %s\n",*end);
    //    LOG_DEBUG("cur is %s\n",*cur);
    //    LOG_DEBUG("len is %s\n",len);

        while(cur + sizeof(struct inotify_event) <= end)
        {
            struct inotify_event* e = (struct inotify_event*)cur;
            if(cur + sizeof(struct inotify_event) + e->len > end)
            {
                break;
            }
        //    LOG_DEBUG("cur : %p\n",cur + sizeof(struct inotify_event));
        //    LOG_DEBUG("end : %p\n",end);

        //    char *relat=NULL;
          //get the abs_path of current operation accord to wd
          //use a middle variable to store the value is imporant
            LOG_INFO("wd is %d\n",e->wd);
            char* match_dir = monitor_dirs_search(monitor_dirs_head,e->wd);
            //curial step or will exit the system
            if (match_dir == NULL)
            {
                LOG_DEBUG("search is empty, continue\n");
                break;
            }
            //  monitor_dirs_dump(monitor_dirs_head);
            char relat[MAX_PATH_LENGTH] = {0};
            strcat(relat,match_dir);
            strcat(relat,e->name);
            //the '/' is imporant to add dir_wd
            //LOG_DEBUG("{relat}:%s\n",relat);
            if(e->mask & IN_CREATE)
            {
                LOG_INFO("CREATE\n");
                //LOG_INFO("relat is %s\n",relat);
                if( stat(relat,&s) == 0 )
                {
        //            LOG_DEBUG("Mode: %lo (octal)\n", (unsigned long) s.st_mode);
                    if( s.st_mode & S_IFDIR )
                    {
                        LOG_DEBUG("is  dir\n");
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
                      //  LOG_DEBUG("wd is %d\n",e->wd);
                        monitor_node_add(monitor_head,name,relat,'d');
                        LOG_DEBUG("add directory's path: %s, type: %c\
                            success.\n",name->path,name->type);
                        //create new monitor_node and add to the monitor_dirs
                       // LOG_DEBUG("new watch dir_wd :%d\n",dir_wd);
                     //   LOG_DEBUG("watch dir_path: %s\n",relat);
                        struct monitor_dirs* new_monitor_dirs;
                        monitor_dirs_init(&new_monitor_dirs);
                        //add new abs_path of creat directory
                        monitor_dirs_add(monitor_dirs_head,new_monitor_dirs,relat,dir_wd);
                       // LOG_DEBUG("wd is %d\n",e->wd);
                      //  LOG_DEBUG("add directory_monitor success!\n");
                      //  monitor_dirs_dump(monitor_dirs_head);
                    }
                    else if( s.st_mode & S_IFREG )
                    {
                        //it's a file
                        //create new directory_node and add to the list
                        LOG_DEBUG("is file\n");
                        struct monitor_node *name;
                        monitor_list_init(&name,e->name);
                    //    LOG_DEBUG("relat is %s\n",relat);
                        monitor_node_add(monitor_head,name,relat,'f');
                        LOG_INFO("add path: %s\t type: %c success.\n",
                               name->path,name->type);
                    }
                    else
                    {
                       //something else
                        LOG_NOTICE("unknown types.\n");
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
                LOG_DEBUG("DELETE\n");
                LOG_DEBUG("relat is %s\n",relat);
                if(search_file_type(monitor_head,relat)=='d')
                {
                    //it's a directory
                    LOG_DEBUG("is dir\n");
                    strcat(relat,"/");
                    LOG_DEBUG("delete the directory's ");
                //  LOG_DEBUG("delete the dir_wd is %d\n",dir_wd);
                //  LOG_DEBUG("e->wd is %d\n",e->wd);
                    moniitor_node_remove_bydir(monitor_head,relat);
                    monitor_bydirs_remove(monitor_dirs_head,fd,relat);
                    LOG_INFO("delete monitor_node of directory_monitor success!\n");
                    monitor_dirs_dump(monitor_dirs_head);
                }
                else if(search_file_type(monitor_head,relat)=='f')
                {
                    //it's a file
                    //create new directory_node and add to the list
                    LOG_INFO("is file\n");
                    LOG_INFO("delete the file's ");
                    moniitor_node_remove(monitor_head,relat);
                }
                else
                {
                    //something else
                  //  goto exit;
                    LOG_WARN("can't recogninze the filetype.\n");
                    LOG_NOTICE("system will continue.");
                }
            }
            if(e->mask & IN_MOVED_TO)
            {
                LOG_DEBUG("MOVED FROM OTHER PLACE.\n");
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
              //  LOG_DEBUG("relat is %s\n",relat);
                if( stat(relat,&s) == 0 )
                {
                    if( s.st_mode & S_IFDIR )
                    {
                        //it's a directory
                        LOG_DEBUG("is dir\n");
                        strcat(relat,"/");

                        struct monitor_node *name;
                        monitor_list_init(&name,e->name);
                        monitor_node_add(monitor_head,name,relat,'d');
                        LOG_DEBUG("add directory's path: %s, type: %c success.\n",name->path,name->type);

                        struct monitor_dirs* new_monitor_dirs;
                        monitor_dirs_init(&new_monitor_dirs);
                        monitor_dirs_add(monitor_dirs_head,new_monitor_dirs,relat,dir_wd);
                        listDir(relat,monitor_head,"add",fd,monitor_dirs_head);
                    }
                    else if( s.st_mode & S_IFREG )
                    {
                        //it's a file
                        LOG_DEBUG("is file\n");
                        struct monitor_node *name;
                        monitor_list_init(&name,e->name);
                        monitor_node_add(monitor_head,name,relat,'f');
                    }
                    else
                    {
                       //something else
                        LOG_ERROR("unknown types.\n");
                    }
                }
                else
                {
                    //error
                    perror("stat is wrong when moved.\n");
                    return 1;
                }
                LOG_INFO("moved here result is:\n");
                monitor_dirs_dump(monitor_dirs_head);
                print_monitor_list(monitor_head);
            }
            if(e->mask & IN_MOVED_FROM)
            {
                LOG_DEBUG("MOVED TO OTHER PLACE.\n");
                //LOG_DEBUG("relat is %s\n",relat);
                //strcat(relat,"/");
                if(search_file_type(monitor_head,relat)=='d')
                {
                    //it's a directory
                    LOG_DEBUG("is dir\n");
                    strcat(relat,"/");
                    LOG_INFO("moved the directory's ");
                 //  LOG_DEBUG("delete the dir_wd is %d\n",dir_wd);
                 //  LOG_DEBUG("e->wd is %d\n",e->wd);
                    moniitor_node_remove_bydir(monitor_head,relat);
                    monitor_bydirs_remove(monitor_dirs_head,fd,relat);
                    LOG_INFO("moved out monitor_node of directory_monitor success!\n");
                    monitor_dirs_dump(monitor_dirs_head);
                }
                else if(search_file_type(monitor_head,relat)=='f')
                {
                    //it's a file
                    //create new directory_node and add to the list
                    LOG_INFO("is file\n");
                    LOG_DEBUG("moved out the file's ");
                    moniitor_node_remove(monitor_head,relat);
                }
                else
                {
                    //something else
                    goto exit;
                }
                LOG_DEBUG("moved out result is:\n");
                monitor_dirs_dump(monitor_dirs_head);
                print_monitor_list(monitor_head);
            }
            if(e->mask & IN_MODIFY)
            {
               // LOG_DEBUG("relat is %s\n",relat);
                LOG_DEBUG("file %s is modified.\n",relat);
            }
           /* if(e->mask & IN_CLOSE_WRITE)
            {
               // LOG_DEBUG("relat is %s\n",relat);
                LOG_DEBUG("file %s is modified success.\n",relat);
            }
              else
                {
                    //error
                    LOG_DEBUG("others stat is wrong.\n");
                // getcwd(e->name,MAX_PATH_LENGTH));
                }*/
            //    LOG_DEBUG("after add name {relat} is %s\n",relat);
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
                     //clear the list
    return 0;
}
