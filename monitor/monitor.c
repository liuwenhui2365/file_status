#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/inotify.h>   //this header file is curial
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>

#define EVENT_BUFSIZE 4096
#define MAX_PATH_LENGTH 256

struct monitor_dirs
{
    char abs_path[MAX_PATH_LENGTH];
    int dir_wd;
    struct monitor_dirs *next;
};   //The Node of List

void init_monitor_dirs(struct monitor_dirs **pNode)//two level point
{
    //be careful! because *pNode is empty,so it can't read **pNode
    *pNode = malloc(sizeof(struct monitor_dirs));
    if (*pNode == NULL)
    {
        printf("malloc error\n");
        exit(EXIT_FAILURE);
    }
    memset(*pNode, 0, sizeof(struct monitor_dirs));
}

struct monitor_dirs* travel_monitor_dirs(struct monitor_dirs* head)
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

char* search_monitor_dirs(struct monitor_dirs* head,int dir_wd)
{
    struct monitor_dirs* p = NULL;
    p = head;  //curial sentence
    while(NULL != p)
    {
        if(dir_wd==p->dir_wd)  //think about "=" and "=="
        {
      //    printf("find abs_path: %s, dir_wd:%d\n",p->abs_path,p->dir_wd);
            return p->abs_path;
        }
        else
        {
             p = p->next;
        }
    }
    printf("can't find the abs_path!\n");
}

void* remove_monitor_dirs(struct monitor_dirs* head,int dir_wd)
{
    struct monitor_dirs* p = NULL;
    p = head->next;  //curial sentence
    while(NULL != p)
    {
        if(dir_wd==p->dir_wd)  //think about "=" and "=="
        {
            printf("abs_path: %s, dir_wd:%c\n",p->abs_path,p->dir_wd);
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


void dump_monitor_dirs(struct monitor_dirs *head)
{
    printf("dump monitor dirs: \n");
    struct monitor_dirs* p = NULL;
    p = head->next;
    while(NULL != p)
    {
        printf("abs_path: %s, dir_wd:%d\n",p->abs_path,p->dir_wd);
        //head = p;
        p = p->next;
    }
}


void* add_monitor_dirs(struct monitor_dirs* head,
                       struct monitor_dirs* new_node,
                       char   *abs_path,int dir_wd)
{
    struct monitor_dirs* tail = travel_monitor_dirs(head);
    tail->next = new_node;
    strncpy(new_node->abs_path,abs_path,strlen(abs_path));//how about "="
    new_node->dir_wd = dir_wd;
    new_node->next = NULL;

    return ;
}


struct test
{
    char filename[MAX_PATH_LENGTH];
    char filetype;
    struct test *next;
};   //The Node of List

void initList(struct test **pNode,char *name)//two level point
{
    //be careful! because *pNode is empty,so it can't read **pNode
    *pNode = malloc(sizeof(struct test));
    if (*pNode == NULL)
    {
        printf("malloc error\n");
        exit(EXIT_FAILURE);
    }
    memset(*pNode, 0, sizeof(struct test));

   // printf("name is %s\n",name);
}

struct test* TravelList(struct test *head)
{
    struct test* p = NULL;
    p = head->next;  //curial sentence
    while(NULL != p)
    {
        head = p;     //this is very important
        p = p->next;
    }

    return head;
}

void* DeleteNode(struct test *head,char* filename)
{
    struct test* p = NULL;
    p = head->next;  //curial sentence
    while(NULL != p)
    {
        if(strcmp(filename,p->filename) == 0)  //think about "=" and "=="
        {
            printf("filename : %s, filetype : %c\n",p->filename,p->filetype);
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


void* print_List(struct test *head)
{
    struct test* p = NULL;
    p = head->next;
    while(NULL != p)
    {
        printf("filename: %s\tfiletype: %c\n",p->filename,p->filetype);
        head = p;
        p = p->next;
    }

    return ;
}


void* AddNode(struct test* h, struct test *Node,char *filename,char filetype)
{
    struct test *tail = TravelList(h);
    tail->next = Node;
/*
    Node->filetype = malloc(sizeof(char*));
    if(Node->filetype == NULL)
    {
        printf("malloc error\n");
        exit(EXIT_FAILURE);
    }
 */
    strncpy(Node->filename,filename,strlen(filename));//how about "="
    Node->filetype = filetype;
    Node->next = NULL;

    return ;
}

char listDir(char *path,struct test *a,
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
         //    printf("dir:%s/%s\n",childpath,ent->d_name);
            strcat(abspath,path);
            strcat(abspath,"/");
            strcat(abspath,ent->d_name);
     //       printf("%s",abspath);
            strcat(childpath,"/");
            strcat(childpath,ent->d_name);
            if(sign=="add")
            {
               dir_wd = inotify_add_watch(fd,childpath,\
                       IN_CREATE | IN_ISDIR | IN_DELETE |\
                       IN_MOVED_TO | IN_MOVED_FROM | IN_CLOSE_WRITE );
                if(dir_wd == -1)
                {
                    perror("inotify_add_watch failed\n");
                  //  goto exit;
                }
                printf("new watch dir_wd :%d\n",dir_wd);
                strcat(childpath,"/");
                printf("watch dir:%s\n",childpath);
                struct monitor_dirs* new_node;
                init_monitor_dirs(&new_node);
                add_monitor_dirs(monitor_dirs_head,new_node,childpath,dir_wd);
                struct test *name;
                initList(&name,ent->d_name);
                AddNode(a,name,childpath,'d');
                listDir(abspath,a,"add",fd,monitor_dirs_head);
            }
            if(sign=="sub")
            {
                DeleteNode(a,childpath);
                listDir(abspath,a,"sub",fd,monitor_dirs_head);
            }
     //        printf("list directory.\n");
      //       print_List(a);

        }
        else
        {
             //sprintf(childpath,"%s\n",ent->d_name);
             //printf("file:%s/%s\n",childpath,ent->d_name);
             struct test *name;
             initList(&name,ent->d_name);
             strcat(childpath,"/");
             strcat(childpath,ent->d_name);
             if(sign=="add")
             {
                 AddNode(a,name,childpath,'f');
             }
             if(sign=="sub")
             {
                 DeleteNode(a,childpath);
             }
        //     printf("list file.\n");
        //     print_List(a);
        }
    //    printf("while interior.\n");
    //    print_List(a);  
    }
   // printf("result is\n");
   // print_List(a);
}



int main(int argc, char **argv)
{

    int len = 0;
    int fd,dir_wd;
    const char* dirname = argv[1];
    if(argc != 2)
    {
        printf("PROGRAM: missing dirname\n");
        printf("Usage: PROGRAM absolute-dirname \n");
        exit(1);
    }
    char buf[EVENT_BUFSIZE];
    char *cur = buf;
    char *end;
    char abs_curdir[MAX_PATH_LENGTH];

//    getcwd(abs_curdir,MAX_PATH_LENGTH);
    realpath(dirname,abs_curdir);
    strncat(abs_curdir,"/",2);

    printf("\nwelcome to use monitor filesystem!\n\n");
    printf("our monitor system is used to monitor the file or directory.\n\n");

    struct test* a;
    initList(&a,"first_node");
    printf("first node create success.\n");

    struct monitor_dirs* monitor_dirs_head;
    init_monitor_dirs(&monitor_dirs_head);
    printf("monitor_dirs first node create success.\n");


    fd = inotify_init();
    if(fd == 1)
    {
        perror("inotify_init failed\n");
        return -1;
    }
    printf("fd is %d\n",fd);

    dir_wd = inotify_add_watch(fd,dirname,
        IN_CREATE | IN_ISDIR | IN_DELETE |
        IN_MOVED_TO | IN_MOVED_FROM |
        IN_CLOSE_WRITE );

    if(dir_wd == -1)
    {
        perror("inotify_add_watch failed\n");
        goto exit;
    }
    printf("watch dir_wd :%d\n",dir_wd);
    printf("watch dir: %s\n",abs_curdir);
    struct monitor_dirs* new_monitor_dirs;
    init_monitor_dirs(&new_monitor_dirs);
    //add monitor_dirs of first node
    add_monitor_dirs(monitor_dirs_head,new_monitor_dirs,abs_curdir,dir_wd);
    listDir(argv[1],a,"add",fd,monitor_dirs_head);
    printf("The directory has some file or directory already:\n");
    print_List(a);
    dump_monitor_dirs(monitor_dirs_head);
  //  printf("the directory of monitor is empty!\n");

    struct stat s;
    printf("filesystem init finish.\n\n");

    while(1)
    {
        len = read(fd,cur,EVENT_BUFSIZE - len);
        if(len <= 0)
        {
            perror("read inotify event failed\n");
            goto exit;
        }

        end = cur + len;
    //    printf("end is %s\n",*end);
    //    printf("cur is %s\n",*cur);
    //    printf("len is %s\n",len);

        while(cur + sizeof(struct inotify_event) <= end)
        {
            struct inotify_event* e = (struct inotify_event*)cur;
            if(cur + sizeof(struct inotify_event) + e->len > end)
            {
                break;
            }
        //    printf("cur : %p\n",cur + sizeof(struct inotify_event));
        //    printf("end : %p\n",end);

        //    char *relat=NULL;
          //get the abs_path of current operation accord to wd
            char* match_dir = search_monitor_dirs(monitor_dirs_head,e->wd);
          //  dump_monitor_dirs(monitor_dirs_head);
            char relat[MAX_PATH_LENGTH] = {0};
            strcat(relat,match_dir);
            strcat(relat,e->name);
            printf("before add {relat}:%s\n",relat);
            if(e->mask & IN_CREATE)
            {
                printf("in create\n");
                if( stat(relat,&s) == 0 )
                {
        //            printf("Mode: %lo (octal)\n", (unsigned long) s.st_mode);
                    if( s.st_mode & S_IFDIR )
                    {
                        printf("is dir\n");
                        //it's a directory
                        //get the abspath of create directory
                        //add watch and get the dir_wd
                        dir_wd = inotify_add_watch(fd,abs_curdir,
                                IN_CREATE | IN_ISDIR | IN_DELETE |\
                                IN_MOVED_TO | IN_MOVED_FROM |\
                                IN_CLOSE_WRITE );
                        if(dir_wd == -1)
                        {
                            perror("inotify_add_watch failed\n");
                            goto exit;
                        }
                        strcat(relat,"/");
                      //create new file_node and add to the list
                        struct test *name;
                        initList(&name,e->name);
                      //  printf("wd is %d\n",e->wd);
                        AddNode(a,name,relat,'d');
                        printf("add directory success!\n");
                        printf("filename: %s\t filetype: %c\n",name->filename,name->filetype);
                        //create new monitor_node and add to the monitor_dirs
                        printf("new watch wd :%d\n",e->wd);
                        printf("new watch dir_wd :%d\n",dir_wd);
                        printf("watch dir: %s\n",relat);
                        struct monitor_dirs* new_monitor_dirs;
                        init_monitor_dirs(&new_monitor_dirs);
                        //add new abs_path of creat directory
                        add_monitor_dirs(monitor_dirs_head,new_monitor_dirs,abs_curdir,dir_wd);
                       // printf("wd is %d\n",e->wd);
                        printf("add directory_monitor success!\n");
                        dump_monitor_dirs(monitor_dirs_head);
                    }
                    else if( s.st_mode & S_IFREG )
                    {
                        //it's a file
                        //create new directory_node and add to the list
                        printf("is file\n");
                        struct test *name;
                        initList(&name,e->name);
                        printf("relat is %s\n",relat);
                        AddNode(a,name,relat,'f');
                        printf("add filename: %s\t filetype: %c success.\n",
                               name->filename,name->filetype);
                     }
                    else
                    {
                       //something else
                        printf("unknown types.\n");
                     }
                }
                else
                {
                    //error
                    perror("stat is wrong when create.\n");
                    return 1;
                }
            }
            if(e->mask & IN_DELETE)
            {
            //     printf("relat is %s\n",relat);
                 DeleteNode(a,relat);
                 printf("delete success.\n");
     //            print_List(a);
     //         inotify_rm_watch(fd, dir_wd);
            }
            if(e->mask & IN_MOVED_TO)
            {
                printf("dentry %s is moved here from other place.\n",relat);
                listDir(relat,a,"add",fd,monitor_dirs_head);
                print_List(a);
            }
       /*     if(e->mask & IN_CLOSE_WRITE)
            {
                printf("file %s is modified.\n",e->name);
            }
        */
            if(e->mask & IN_MOVED_FROM)
            {
                if( s.st_mode & S_IFDIR )
                {
                    //it's a directory
                    printf("directory %s is moved to other place.\n",relat);
                    listDir(relat,a,"sub",fd,monitor_dirs_head);
                    print_List(a);
                }
                else if( s.st_mode & S_IFREG )
                {
                    //it's a file
                    printf("file %s is moved to other place.\n",relat);
                    DeleteNode(a,relat);
                    print_List(a);
                }
                else
                {
                    //something else
                    printf("unknown types.\n");
                }
            }
        /*    else
            {
                //error
                printf("others stat is wrong.\n");
               // getcwd(e->name,MAX_PATH_LENGTH));
            }*/
            printf("after add name {relat} is %s\n",relat);
            dump_monitor_dirs(monitor_dirs_head);
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
    return 0;
}
