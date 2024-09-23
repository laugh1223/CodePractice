#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <pthread.h>
#include <sys/time.h>
#include <semaphore.h>

using namespace std;

#define MMAPPATH "/data/local/tmp/mmapTmp"
#define MMAPSIZE 40 * 4 * 1024

struct m_sem
{
    sem_t r_sem[2];
    sem_t w_sem;
    char buff[MMAPSIZE];
};

class commonMmap
{
public:
    commonMmap(string fname = "./mmapTmp"):filename(fname)
    {
        mmapsize = sizeof(struct m_sem);
    };
    ~commonMmap()
    {
        munmap(mapptr, mmapsize);
    }

    //打开或者创建一个共享内存
    int openMmap()
    {
        int fd = open(filename.c_str(), O_RDWR | O_EXCL | O_CREAT);
        if(fd > 0){
            ftruncate(fd, mmapsize);//mmap不能扩展文件大小，故首次创建时要确定文件的大小
            mapptr = (struct m_sem *)mmap(nullptr, mmapsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            close(fd);
            if(mapptr == MAP_FAILED){
                cout << "create mmap error" << endl;
                return -1;
            }
            sem_init(&mapptr->r_sem[0], 1, 0);
            sem_init(&mapptr->r_sem[1], 1, 0);
            sem_init(&mapptr->w_sem, 1, 1);
        } else {
            fd = open(filename.c_str(), O_RDWR);
            mapptr = (struct m_sem *)mmap(nullptr, mmapsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            close(fd);
            if(mapptr == MAP_FAILED){
                cout << "open mmap error" << endl;
                return -1;
            }
        }
        return 0;
    }

    /* 发送消息 */
    void mapSend(const char *buff, uint32_t len, const int index)
    {
        sem_wait(&mapptr->w_sem);
        memcpy(mapptr->buff, buff, len);
        sem_post(&mapptr->r_sem[index]);
    }

    /* 发送int类型消息 */
    void mapSend(const int num, const int index)
    {
        sem_wait(&mapptr->w_sem);
        char buff[sizeof(num)];
        snprintf(buff, sizeof(num), "%d", num);
        memcpy(mapptr->buff, buff, sizeof(num));
        sem_post(&mapptr->r_sem[index]);
    }

    /* 接收消息 */
    void mapRecv(char *buff, uint32_t len, const int index)
    {
        sem_wait(&mapptr->r_sem[index]);
        memcpy(buff, mapptr->buff, len);
        sem_post(&mapptr->w_sem);
    }

    /* 接收int类型消息 */
    void mapRecv(int &num, const int index)
    {
        sem_wait(&mapptr->r_sem[index]);
        num = atoi(mapptr->buff);
        sem_post(&mapptr->w_sem);
    }

private:
    string filename;        //映射的磁盘文件
    uint32_t mmapsize;      //mmap的大小
    struct m_sem *mapptr;   //mmap映射的首地址
};