#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAXNAME  10

typedef enum filetype
{
        file,
        floder
}FileType;

typedef struct fileNode
{
        char filename[MAXNAME];
        filetype type;
        struct fileNode* firstchild,*nextsibling,*parent;
}FileNode,*FileTree;

void identifyFileType(char* dir,FileNode* rootNode,FileNode* preNode,FileNode* currentNode,int depth)
{
        char dirname[MAXNAME];
        struct stat stbuf;
        sprintf_s(dirname,"%s\\%s",dir,currentNode->filename);
        if((stat(dirname,&stbuf)) == -1)
        {
            fprintf(stderr, "listdirtree:can't read file %s information!\n",dirname);
            return ;
        }
        if(rootNode->firstchild == NULL)
        {
            rootNode->firstchild = currentNode;
            currentNode->parent = rootNode;
        }
        else
        {
            preNode->nextsibling = currentNode;
        }
        printf("%s\n",currentNode->filename);
        if((stbuf.st_mode & S_IFMT) = S_IFDIR)
        {
            currentNode->type = FileType::folder;
            reverseCatalogueToTree(dirname,currentNode,NULL,NULL,++depth);
        }
}

void reverseCatalogueToTree(char* dir,FileNode* rootNode,FileNode* preNode* currentNode,int depth)
{
        struct dirent *fip;
        DIR *dp;
        if ((dp = opendir(dir)) == NULL)
        {
            //fprintf(stderr,"can't open %s\n",dirname);
            return;
            }
        while ((fip = readdir(dp)) != NULL)
        {
            if (strcmp(fip->d_name, ".") == 0 ||
                strcmp(fip->d_name, "..") ==
                0)/*跳过"."或者".."的目录*/
            {
                continue;
            }

            currentNode = fileNodeFactory();
            strcpy_s(currentNode->filename, MAXNAME, fip->d_name);
            currentNode->parent = rootNode;

            if(preNode == NULL)
            {
                preNode = fileNodeFactory();
            }
            else
            {
                preNode->nextsibling = currentNode;
                currentNode->parent = preNode->parent;
            }
            identifyFileType(dir, rootNode,preNode,currentNode,depth);
            preNode = currentNode;
        }
        closedir(dp);
}

int main(int argc, char **argv)
{
    FileTree tree = fileNodeFactory();
    if (argc == 1)
    {
        strcpy_s(tree->filename, MAXNAME, ".");
        reverseCatalogueToTree(".", tree, NULL, NULL, 0);
    }
    else
    {
        printf("%s\n", argv[1]);
        strcpy_s(tree->filename,MAXNAME, *++argv);
       reverseCatalogueToTree(*argv, tree, NULL,NULL,0);
    }
    return 0;
}
