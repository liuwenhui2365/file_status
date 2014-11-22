##关于文件监控的测试方法:

**测试原则**: 递归三层,每层一个文件和文件夹

### 创建

- 文件夹
       创建空文件夹  mkdir a
       创建非空文件夹 mkdir  b   touch b/c   mkdir b/d/  mkdir b/d/d   touch b/d/e    
          
- 文件
       创建文件命令      touch  c
      
###删除

- 文件夹 

>- 如果为空,使用命令       rmdir a   
>- 如果为非空,遍历查找要删除结点, 
> mkdir  b   touch b/c   mkdir b/d/  mkdir b/d/d   touch b/d/e 
 >rm -rf  b
**特别注意:**在删除文件夹后对应的路径转到被删除的文件夹路径上.所以要忽略该id对应的路径然后继续.

- 文件 

> 使用命令  touch c     rm c

###移出去
 
 - 文件夹
  
>- 非空文件夹   遍历查找要移除的文件 
> mkdir  b   touch b/c   mkdir b/d/  mkdir b/d/d   touch b/d/e 
>    mv b/  ../
>- 空文件夹 mkdir a       mv  a/  ../
**特别注意:**在删除文件夹后对应的路径转到被删除的文件夹路径上.,所以要忽略该Id对应的路径然后继续.
- 文件

>touch a             mv  a  ../ 

###移进来

- 文件夹

>- 非空文件夹   采用遍历方法把相关的结点添加到链表
> mkdir  b   touch b/c   mkdir b/d/  mkdir b/d/d   touch b/d/e 
> mv   b   ./
>- 空文件夹   按照文件夹路径存到链表
>mkdir a   rmdir  a

- 文件
>使用命令 mv    a   ./

###修改

- 文件
 
> 进入文件添加或删除内容   vim a   :x   


> 注意测试的时候,修改的时候监控会出现交换文件的创建和修改标记,同时会有本文件的创建和修改,虽然本文件已经存在了.



                                                                                                                                  2014-11-22