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

            char abs_curdir[MAX_PATH_LENGTH];
            getcwd(abs_curdir,MAX_PATH_LENGTH);
            strncat(abs_curdir,"/",4);

            char *relat=NULL;
            relat =  strncat(abs_curdir,e->name,e->len);

            printf("relat is %s\n",relat);
            if(e->mask & IN_CREATE)
            {
               struct stat s;
               if( stat(relat,&s) == 0 )
                {
                    printf("Mode: %lo (octal)\n",
		                    (unsigned long) s.st_mode);

                    if( s.st_mode & S_IFDIR )
                    {
                        //it's a directory
                        printf("directory %s is created.\n",relat );
                     }
                    else if( s.st_mode & S_IFREG )
                    {
                        //it's a file
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
               struct stat s1;
               if( stat(relat, &s1) == 0 )
                {
                    if( s1.st_mode & S_IFDIR )
                    {
                        //it's a directory
                        printf("directory %s is deleted.\n",relat);
                    }
                    else if( s1.st_mode & S_IFREG )
                    {
                        //it's a file
                        printf("file %s is deleted.\n",relat);
                     }
                    else
                    {
                        //something else
                        printf("unknown types.\n");
                    }
                }
                else
                {
                    int errnum = errno;
                    //error
                    return 1;
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
