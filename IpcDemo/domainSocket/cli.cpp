#include "domainSocket.hpp"
#include <thread>

void readMsg(int fd)
{
    char buff[1024] = {0};
    while (1)
    {
        read(fd, buff, 1024);
        cout << "r: " << buff << endl;
        if(!strcmp(buff, "-1"))
            break;
        bzero(buff, 1024);
    } 
}

void sendMsg(int fd)
{
    char buff[1024] = {0};
    while (cin.getline(buff,1024))
    {
        write(fd, buff, strlen(buff));
        if(!strcmp(buff, "-1"))
            break;
        bzero(buff, 1024);
    }
}

int main(int argc, char *argv[])
{
    domainSocket m_cli;
    int fd = m_cli.connectServ();
    cout << "start : " << fd << endl;
    if(fd > 0){
        thread tid1(readMsg, fd);
        thread tid2(sendMsg, fd);
        tid1.join();
        tid2.join();
    }

    close(fd);
    return 0;
}