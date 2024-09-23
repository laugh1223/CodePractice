#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <unistd.h>

int main(int argc, char *argv[])
{
    std::cout << "tcp client start" << std::endl;

    int cliFd = socket(AF_INET, SOCK_STREAM, 0);//创建套接字,tcp:SOCK_STREAM
    
    //设置服务端信息
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8888); //服务端端口
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); //服务端IP
    
    //发起链接
    if(connect(cliFd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0){
        perror("connect");
        exit(1);
    }

    pid_t cpid = getpid();
    char buff[1024] = {0};
    sprintf(buff, "client connect %d", cpid);
    std::cout << buff << std::endl;
    send(cliFd, buff, sizeof(buff), 0);

    close(cliFd);

    return 0;
}