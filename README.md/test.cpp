#include <iostream>
#include <unistd.h>
using namespace std;
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <cstdlib> //stdlib.h
#define N 2
#define NUM 1024
void Writer(int wfd)//将数据写入到管道里
{
    string s="hello xiaotao";
    pid_t id=getpid();
    int number=0;
    char buffer[NUM];

    //首先构建要发送的字符内容 “s+id+number”

   while(1)
   {
    sleep(1);
     buffer[0]=0;//字符串清空，是为了提醒阅读代码的人，我把这个数组当做字符串了
    
    snprintf(buffer,sizeof(buffer),"%s-%d-%d",s.c_str(),id,number++);
    
    //发送到管道里面
    write(wfd,buffer,strlen(buffer));//写入文件里可不按照C语言的格式要在最后加上\0
   }

}
void Reader(int rfd)//从管道里接收信息
{
   //因为发过来的数据是一个字符串，但存到文件里面，不是按照C字符串的格式存储，所以
   //我们拿出来时要在最后+\0
   char buffer[NUM];
   while(1)
   {
      buffer[0]=0;
      ssize_t n=read(rfd,buffer,sizeof(buffer));
   if(n>0)//说明读取成功
   {
       buffer[n]='\0';
       cout<<"father get a message:"<<getpid()<<":"<<buffer<<endl;
   }
   else if(n==0)//当读取到0个时，说明管道里的已经没有东西可以读取了
   {
    cout<<"father read file done"<<endl;
   }
   else 
   break;
   }
}
int main()
{
    int pipefd[N]={0};

    int n=pipe(pipefd);//创建管道,对同一个文件打开两次，一次读一次写
    //pipefd[0]du'qu
    if(n<0)return 1;

    //子进程写入，父进程读取
    pid_t id=fork();
    if(id<0)return 2;
    if(id==0)
    {
        //子进程---子进程也获得两个文件描述符pipe[0] pipe[1]
        close(pipefd[0]);//子进程要写，就要将读关闭
        Writer(pipefd[1]);

        exit(0);
    }
    //父进程
    close(pipefd[1]);//父进程要读，将要将写关闭
    Reader(pipefd[0]);

    //父进程等待子进程退出
    pid_t rid=waitpid(id,nullptr,0);
    if(rid<0)return 3;

    sleep(5);
    return 0;
}