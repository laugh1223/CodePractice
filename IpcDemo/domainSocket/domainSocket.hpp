#ifndef DOMAINSOCKET_HH
#define DOMAINSOCKET_HH

#include <cstdlib>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string>
#include <iostream>
#include <stdexcept>

using namespace std;

class domainSocket
{
private:
	int listenfd;
	struct sockaddr_un	cliaddr, servaddr;
    string m_path;

public:
    domainSocket(string path = "./tmpSocket"):m_path(path){}

    ~domainSocket()
    {
        if(listenfd > 0)
            close(listenfd);
    }

    void startServer()
    {
        listenfd = socket(AF_LOCAL, SOCK_STREAM, 0);

        unlink(m_path.c_str());
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sun_family = AF_LOCAL;
        strcpy(servaddr.sun_path, m_path.c_str());
        bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
        listen(listenfd, 5);
    }
	
    int acceptCli()
    {
        socklen_t clilen = sizeof(cliaddr);
        int connfd;
        while (true)
        {
            if((connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen)) < 0){
                if(errno == EINTR)
                    continue;
                else{
                    cout << " error " << endl;
                    exit(-1);
                }

            }
            return connfd;
        }
        return -1;
    }

    int connectServ()
    {
        int sockfd = socket(AF_LOCAL, SOCK_STREAM, 0);
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sun_family = AF_LOCAL;
        strcpy(servaddr.sun_path, m_path.c_str());
        if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == 0)
            return sockfd;
        return -1;
    }
};
#endif