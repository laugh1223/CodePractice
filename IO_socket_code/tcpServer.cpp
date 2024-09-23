#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <unistd.h>

int main(int argc, char *argv[])
{
    std::cout << "tcp server start" << std::endl;

    int sockServerFd = socket(AF_INET, SOCK_STREAM, 0);//创建套接字,tcp:SOCK_STREAM
    
    //设置套接字相关属性
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8888); //设置端口号，并转换为网络字节序，即大端字节序
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); //设置IP地址，INADDR_ANY表示本地任意IP地址

    //将属性绑到套接字
    if(bind(sockServerFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr))){
        perror("bind");
        exit(1);
    }

    //开启监听，监听数量为50
    if(listen(sockServerFd, 50) == -1){
        perror("listen");
        exit(1);
    }

    struct sockaddr_in clientAddr;
    socklen_t len = sizeof(clientAddr);
    
    while (true)
    {
        //阻塞等待客户端的链接
        int cliFd = accept(sockServerFd, (struct sockaddr *)&clientAddr, &len);
        if(cliFd < 0){
            std::cout << "accept error" << std::endl;
            continue;
        }

        std::thread([cliFd](){
            char buff[2014] = {0};
            recv(cliFd, buff, sizeof(buff), 0);
            std::cout << buff << std::endl;
            close(cliFd);//关闭链接
        }).detach();
    }

    close(sockServerFd);

    return 0;
}