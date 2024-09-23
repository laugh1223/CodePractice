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
    std::cout << "udp client start" << std::endl;

    int cliFd = socket(AF_INET, SOCK_DGRAM, 0);//创建套接字, UDP:SOCK_DGRAM
    
    //设置服务端信息
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(6666); //服务端端口
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); //服务端IP

    //保存接受的套接字信息
    struct sockaddr_in recvSrc;
    socklen_t recvLen;

    pid_t cpid = getpid();
    char buff[1024] = {0};
    sprintf(buff, "client connect %d", cpid);
    std::cout << buff << std::endl;

    //udp使用sendto、recvfrom进行消息的接受和发送
    if(sendto(cliFd, buff, sizeof(buff), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1){
        perror("sendto");
        exit(1);
    }

    memset(&recvSrc, 0, sizeof(recvSrc));    
    if(recvfrom(cliFd, buff, sizeof(buff), 0, (struct sockaddr *)&recvSrc, &recvLen)){
        perror("recvfrom");
    }
    char IP[40] = {0};
    inet_ntop(AF_INET, (void *)&recvSrc.sin_addr.s_addr, IP, sizeof(IP));
    std::cout << "recvfrom : " << buff << " " << " serv  ip: " << IP << " port: " << ntohs(recvSrc.sin_port) << std::endl; 
    
    close(cliFd);

    return 0;
}