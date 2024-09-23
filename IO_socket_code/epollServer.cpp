#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

int main()
{
    //创建套接字
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

    //调用epoll_create创建一个epoll实例
    int epfd = epoll_create(100);

    //将监听的文件描述符添加到epoll实例中
    struct epoll_event epev;
    epev.events = EPOLLIN;
    epev.data.fd = sfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sfd, &epev);

    struct epoll_event epevs[1024];
    while (true)
    {
        //调用epoll_wait阻塞等待事件的发生
        int ret = epoll_wait(epfd, epevs, 1024, -1);
        if(ret == -1){
            perror("epoll");
            exit(-1);
        }
        printf("ret = %d\n", ret);
        for(int i = 0; i < ret; ++i){
            if(epevs[i].data.fd == sfd){
                //有客户端发起链接
                struct sockaddr_in cliaddr;
                socklen_t len = sizeof(cliaddr);
                int cfd = accept(sfd, (struct sockaddr *)&cliaddr, &len);
#if 0
                //设置cfd为非阻塞
                int flag = fcntl(cfd, F_GETFL);//获取当前状态
                flag |= O_NONBLOCK;
                fcntl(cfd, F_SETFL, flag);

                //将新的文件描述符添加到epoll中
                epev.events = EPOLLIN | EPOLLET; //设置边沿触发，边沿触发模式，文件描述符必须设置为非阻塞
#else
                //将新的文件描述符添加到epoll中
                epev.events = EPOLLIN;//默认为水平触发
#endif
                epev.data.fd = cfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &epev);
            }else{
                if(epevs[i].events & EPOLLOUT) continue;

                //说明这个文件描述符的客户端发来了数据
#if 0
                //边沿触发模式下，要循环读取所有数据，防止漏读数据
                int length = 0;
                char buff[10] = {0};
                while (true)
                {
                    //如果读一个缓冲区,缓冲区没有数据,如果是带阻塞,read会阻塞等待,如果是非阻塞,read会返回-1,并且会将errno 值设置为EAGAIN
                    memset(buff, 0, 10);
                    length = read(epevs[i].data.fd, buff, sizeof(buff));
                    if(length == 0){
                        printf("client %d closed\n", epevs[i].data.fd);
                        //关闭描述符，并从epoll集合中清除掉
                        epoll_ctl(epfd, EPOLL_CTL_DEL, epevs[i].data.fd, NULL);
                        close(epevs[i].data.fd);
                        break;
                    }else if(length == -1){
                        if(errno == EAGAIN){
                            break;
                        }
                        perror("read");
                        //关闭描述符，并从epoll集合中清除掉
                        epoll_ctl(epfd, EPOLL_CTL_DEL, epevs[i].data.fd, NULL);
                        close(epevs[i].data.fd);
                        break;
                    }else if(length > 0){
                        printf("recv data : %s\n", buff);
                        // write(epevs[i].data.fd, buff, sizeof(buff));
                    }
                }
#else
                char buff[1024] = {0};
                int num = read(epevs[i].data.fd, buff, sizeof(buff));
                if(num == -1){
                    perror("read");
                    continue;
                }else if(num == 0){
                    //说明客户端断开了链接
                    printf("client %d is closed....\n", epevs[i].data.fd);
                    //关闭描述符，并从epoll集合中清除掉
                    epoll_ctl(epfd, EPOLL_CTL_DEL, epevs[i].data.fd, NULL);
                    close(epevs[i].data.fd);
                }else if(num > 0){
                    printf("read: %s\n", buff);
                    write(epevs[i].data.fd, buff, strlen(buff) + 1);
                }
#endif
            }
        }
    }
    printf("over\n");
    close(sfd);
    close(epfd);
    return 0;
}