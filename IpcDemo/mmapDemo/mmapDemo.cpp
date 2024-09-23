#include "commonMmap.hpp"

int main(int argc, char *argv[])
{
    commonMmap m_mmap;
    m_mmap.openMmap();
    m_mmap.mapSend("hello world", 12, 0);

    return 0;
}