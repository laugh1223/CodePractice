#include <stdio.h>
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
#include <poll.h>

int main()
{
    //创建socket
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
    
    //初始化poll结构体集合
    struct pollfd fds[1024];
    for(int i = 0; i < 1024; ++i){
        fds[i].fd = -1;
        fds[i].events = POLL_IN;
    }
    fds[0].fd = sfd;
    int nfds = 0;//poll集合中的最大索引

    while (true)
    {
        //调用poll系统函数，阻塞等待事件的发生
        int ret = poll(fds, nfds + 1, -1);
        if(ret == -1){
            perror("poll");
            exit(-1);
        }else if(ret == 0){
            continue;
        }else if(ret > 0){
            //有文件描述符对应的缓冲区的数据发生了变化
            if(fds[0].revents & POLL_IN){
                //有客户端发起链接
                struct sockaddr_in cliaddr;
                socklen_t len = sizeof(cliaddr);
                int cfd = accept(sfd, (struct sockaddr *)&cliaddr, &len);
                //将新文件描述符加入到集合中
                for(int i = 1; i < 1024; ++i){
                    if(fds[i].fd == -1){
                        fds[i].fd = cfd;
                        fds[i].events = POLL_IN;
                        break;
                    }
                }
                //更新集合中的最大索引
                nfds = nfds > cfd ? nfds : cfd;
            }
            for(int i = 1; i < 1024; ++i){
                if(fds[i].fd > 0 && (fds[i].revents & POLL_IN)){
                    //说明这个文件描述符的客户端发来了数据
                    char buff[1024] = {0};
                    int num = read(fds[i].fd, buff, sizeof(buff));
                    if(num == -1){
                        perror("read");
                        continue;
                    }else if(num == 0){
                        //说明客户端断开了链接
                        printf("client %d is closed....\n", fds[i].fd);
                        //关闭文件描述符并从集合中清除掉
                        close(fds[i].fd);
                        fds[i].fd = -1;
                    }else if(num > 0){
                        printf("read: %s\n", buff);
                        write(fds[i].fd, buff, sizeof(buff));
                    }
                }
            }
        }
    }
    close(sfd);
    return 0;
}