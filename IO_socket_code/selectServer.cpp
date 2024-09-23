#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <wait.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <stdio.h>

int main()
{
    //创建一个tcp socket
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sfd == -1){
        perror("socket");
        exit(-1);
    }
    struct sockaddr_in saddr;
    saddr.sin_addr.s_addr = INADDR_ANY;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8888);

    //绑定
    bind(sfd, (struct sockaddr *)&saddr, sizeof(saddr));

    //开启监听
    listen(sfd, 8);

    //创建一个fd_set集合，存放的是需要检测的文件描述符
    fd_set rdset,tmp;
    //初始化标志位
    FD_ZERO(&rdset);
    //添加需要检测的文件描述符
    FD_SET(sfd, &rdset);
    //设置当前最大文件描述符号
    int maxfd = sfd;
    //设置超时
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    while (true)
    {
        //重置文件描述符集合
        tmp = rdset;

        //调用select系统函数，等待文件描述符的事件发生
        int ret = select(maxfd + 1, &tmp, NULL, NULL, &timeout);
        if(ret == -1){
            perror("select");
            exit(-1);
        }else if(ret == 0){
            continue;
        }else if(ret > 0){
            //检测到文件描述符对应的缓冲区有数据发生变化
            if(FD_ISSET(sfd, &tmp)){
                //sfd发生变化，说明有新客户端连接
                struct sockaddr_in cliAddr;
                socklen_t clilen = sizeof(cliAddr);
                int cfd = accept(sfd, (struct sockaddr *)&cliAddr, &clilen);

                //将新的文件描述符加入到集合中
                FD_SET(cfd, &rdset);
                //更新最大文件描述符号
                maxfd = maxfd > cfd ? maxfd:cfd;
            }
            for(int i = sfd + 1; i <= maxfd; ++i){
                //判断是否在集合中
                if(FD_ISSET(i, &tmp)){
                    //说明这个文件描述符的客户端发来了数据
                    char buff[1024] = {0};
                    int num = read(i, buff, sizeof(buff));
                    if(num == -1){
                        perror("read");
                        continue;
                    }else if(num == 0){
                        //说明该客户端断开连接
                        printf("client %d is closed....\n", i);
                        //关闭描述符，并从集合中清除掉
                        close(i);
                        FD_CLR(i, &rdset);
                    }else if(num > 0){
                        printf("read: %s\n", buff);
                        write(i, buff, strlen(buff) + 1);
                    }
                }
            }
        }
    }
    close(sfd);
    return 0;
}