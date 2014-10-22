#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/inotify.h>   //this header file is curial
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define EVENT_BUFSIZE 4096
#define MAX_PATH_LENGTH 256

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

    printf("name is %s\n",name);
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
            printf("filename is %s, filetype is %c\n",p->filename,p->filetype);
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
  //  strncat(abs_curdir,"/",4);
    realpath(dirname,abs_curdir);
    strncat(abs_curdir,"/",2);
    struct stat s;

    printf("\nwelcome to use monitor filesystem!\n\n");
    printf("our monitor system is used to monitor the file or directory.\n");

    struct test* a;
    initList(&a,"first_node");
    printf("first node create success.\n");

    fd = inotify_init();
    if(fd == 1)
    {
        perror("inotify_init failed\n");
        return -1;
    }
    printf("fd is %d\n",fd);

    dir_wd = inotify_add_watch(fd,dirname,IN_CREATE | IN_ISDIR | IN_DELETE | IN_MOVED_TO | IN_MOVED_FROM);
    if(dir_wd == -1)
    {
        perror("inotify_add_watch failed\n");
        goto exit;
    }
    printf("watch dir_wd :%d\n",dir_wd);
    printf("watch dir: %s\n",abs_curdir);
    printf("filesystem init finish.\n");

    while(1)
    {
       len = read(fd,cur,EVENT_BUFSIZE - len);
        if(len <= 0)
        {
            perror("read inotify event failed\n");
            goto exit;
        }

        end = cur + len;

        while(cur + sizeof(struct inotify_event) <= end)
        {
            struct inotify_event* e = (struct inotify_event*)cur;
            if(cur + sizeof(struct inotify_event) + e->len > end)
            {
                break;
            }

        //    printf("cur : %p\n",cur + sizeof(struct inotify_event));
        //    printf("end : %p\n",end);

//            char *relat=NULL;
            char relat[MAX_PATH_LENGTH] = {0};
            strncat(relat,abs_curdir,strlen(abs_curdir));
            strncat(relat,e->name,e->len);
      //      printf("relat is %s\n",relat);
            if(e->mask & IN_CREATE)
            {
                if( stat(relat,&s) == 0 )
                {
        //            printf("Mode: %lo (octal)\n", (unsigned long) s.st_mode);
                    if( s.st_mode & S_IFDIR )
                    {
                        //it's a directory
                        struct test *name;
                     //   name = (struct test*) e->name;
                        initList(&name,e->name);
                        AddNode(a,name,relat,'d');
                        printf("add directory success!\n");
                        printf("filename: %s\t filetype: %c\n",name->filename,name->filetype);
                     }
                    else if( s.st_mode & S_IFREG )
                    {
                        //it's a file
                        struct test *name;
                      //  strcpy(name,e->name);  //warning
                     //   name = (struct test*) e->name;
                        initList(&name,e->name);
                        AddNode(a,name,relat,'f');
                        printf("add file success!\n");
                        printf("filename: %s\t filetype: %c\n",name->filename,name->filetype);
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
                    printf("stat is wrong.\n");
                    return 1;
                }
            }
            if(e->mask & IN_DELETE)
            {
                 printf("relat is %s\n",relat);
                 DeleteNode(a,relat);
                 printf("delete success.\n");
     //            print_List(a);
     //         inotify_rm_watch(fd, dir_wd);
            }
            if(e->mask & IN_MOVED_FROM)
            {
                 DeleteNode(a,relat);
             //    print_List(a); //test if delete success
                 printf("dentry %s is moved to other place.\n",relat);
                   // getcwd(e->name,MAX_PATH_LENGTH));
            }
            if(e->mask & IN_MOVED_TO)
            {
                printf("dentry %s is moved here from other place.\n",relat);
                     //  getcwd(relat,MAX_PATH_LENGTH));
             //   printf("abs_curdir is %s\n",abs_curdir);
                 if( stat(relat,&s) == 0 )
                {
                    if( s.st_mode & S_IFDIR )
                    {
                        //it's a directory
                        strncat(relat,"/",1);
                        struct test *name;
                        initList(&name,e->name);
                        AddNode(a,name,relat,'d');
                 //       print_List(a);   //test if add success
                        printf("filename: %s\t filetype: %c\n",name->filename,name->filetype);
                     }
                    else if( s.st_mode & S_IFREG )
                    {
                        //it's a file
                        struct test *name;
                        initList(&name,e->name);
                        AddNode(a,name,relat,'f');
                        printf("filename: %s\t filetype: %c\n",name->filename,name->filetype);
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
                    printf("stat is wrong.\n");
                    return 1;
                }

           }

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
