#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <unistd.h>
#include <cstring>

int main(int argc, char *argv[])
{
    std::cout << "udp server start" << std::endl;

    int sockServerFd = socket(AF_INET, SOCK_DGRAM, 0);//创建套接字, UDP:SOCK_DGRAM
    
    //设置套接字相关属性
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(6666); //设置端口号，并转换为网络字节序，即大端字节序
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); //设置IP地址，INADDR_ANY表示本地任意IP地址

    //将属性绑到套接字
    if(bind(sockServerFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr))){
        perror("bind");
        exit(1);
    }

    //保存接受的套接字信息
    struct sockaddr_in clientAddr;
    socklen_t len = sizeof(clientAddr);
    
    while (true)
    {
        //阻塞等待客户端的消息
        char buff[1024] = {0};
        memset(&clientAddr, 0, sizeof(clientAddr));
        if(recvfrom(sockServerFd, buff, sizeof(buff), 0, (struct sockaddr *)&clientAddr, &len) == -1){
            perror("recvfrom");
            continue;
        }
        char IP[40] = {0};
        inet_ntop(AF_INET, (void *)&clientAddr.sin_addr.s_addr, IP, sizeof(IP));
        std::cout << buff << " >>> ip : " << IP << " port : " << ntohs(clientAddr.sin_port) << std::endl;
        sprintf(buff, "server send");
        if(sendto(sockServerFd, buff, sizeof(buff), 0, (struct sockaddr *)&clientAddr, len) == -1){
            perror("sendto");
        }
    }

    close(sockServerFd);

    return 0;
}