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

struct test {
    char *filename;
    char *filetype;
    struct test *next;
};   //The Node of List


void initNode(struct test *pNode)//char *filetype1)  //two level point
{
    //be careful! because *pNode is empty,so it can't read **pNode
     pNode = malloc(sizeof(struct test));
     memset(pNode, 0, sizeof(struct test));
    if (pNode == NULL)
    {
        printf("malloc error\n");
        exit(EXIT_FAILURE);
    }
    pNode->next = NULL;

    printf("List initial success!\n");
/*
    pNode->filetype = malloc(sizeof(filetype1));  //You must malloc size for variable.
    pNode->filetype = filetype1;
    printf("filetype is %s\n",pNode->filetype); */
}

struct test* TravelList(struct test *head)
{
    struct test* p = head->next;  //curial sentence
    while(NULL != p)
    {
        head = p;     //this is very important
        p = p->next;
    }

    return head;
}


void* AddNode(struct test *head, struct test *pNode,char* filename;char* filetype)
{
    struct test *tail = TravelList(head);
    pNode = malloc(sizeof(struct test pNode));
    tail->next = pNode;

    pNode->filename = malloc(sizeof(char*)*10);
    pNode->filename = filename;
    pNode->filename = malloc(sizeof(char*)*10);
    pNode->filetype = filetype;
    pNode->next = NULL;

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
    char *tmp=NULL;
    char abs_curdir[MAX_PATH_LENGTH];
    getcwd(abs_curdir,MAX_PATH_LENGTH);
    strncat(abs_curdir,"/",4);
    struct stat s;
    struct test *a = NULL;
   
    initNode(a);

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

            printf("cur : %p\n",cur + sizeof(struct inotify_event));
            printf("end : %p\n",end);

            char *relat=NULL;
            relat =  strncat(abs_curdir,e->name,e->len);
            if(e->mask & IN_CREATE)
            {
                if( stat(relat,&s) == 0 )
                {
                    printf("Mode: %lo (octal)\n",
		                    (unsigned long) s.st_mode);

                    if( s.st_mode & S_IFDIR )
                    {
                        //it's a directory
                        AddNode(a,a1,e->name,"directory")
                        printf("filetype is %s\n",a1->filetype);
                        printf("directory %s is created.\n",relat );
                     }
                    else if( s.st_mode & S_IFREG )
                    {
                        //it's a file
                        AddNode(a,a1,e->name,"file");
                        printf("filetype is %s\n",tmp);
                        printf("file %s is created.\n",relat);
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
             //   printf("filetype is %s\n",tmp);
                if(strcmp(tmp, "file") == 0 )
                {
                    printf("file %s is deleted.\n",e->name);
                }
                if(strcmp(tmp, "directory") == 0 )
                {
                    printf("directory is deleted.\n",relat);
                }

       //         inotify_rm_watch(fd, dir_wd);
                memset(abs_curdir,0,256);
            }
            if(e->mask & IN_MOVED_FROM)
            {
              printf("dentry %s is moved to other place.\n",
                    getcwd(e->name,MAX_PATH_LENGTH));
            }
            if(e->mask & IN_MOVED_TO)
            {
                printf("dentry %s is moved here from other place.\n",
                    getcwd(e->name,MAX_PATH_LENGTH));
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
