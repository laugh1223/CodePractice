cmake_minimum_required(VERSION 3.10)
project(SimpleRtspServer)

#set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread ")

set(CMAKE_BUILD_TYPE DEBUG)
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

set(INCLUDE_DIR /usr/local/include)
set(LIB_DIR /usr/local/lib)
include_directories(${INCLUDE_DIR})
link_directories(${LIB_DIR})

add_executable(SimpleRtspServer
        src/liveMedia/Buffer.cpp
        src/liveMedia/InetAddress.cpp
        src/liveMedia/MediaSessionManager.cpp
        src/liveMedia/MediaSession.cpp
        src/liveMedia/AACFileMediaSource.cpp
        src/liveMedia/H264FileMediaSource.cpp
        src/liveMedia/Rtp.cpp
        src/liveMedia/MediaSource.cpp
        src/liveMedia/H264FileSink.cpp
        src/liveMedia/AACFileSink.cpp
        src/liveMedia/Sink.cpp
        src/liveMedia/RtspConnection.cpp
        src/liveMedia/RtspServer.cpp
        src/liveMedia/TcpConnection.cpp
        src/BasicUsageEnvironment/Event.cpp
        src/BasicUsageEnvironment/EventBasicUsageEnvironment.cpp
        src/BasicUsageEnvironment/Poller.cpp
        src/BasicUsageEnvironment/SelectPoller.cpp
        src/BasicUsageEnvironment/SocketsOps.cpp
        src/BasicUsageEnvironment/Thread.cpp
        src/BasicUsageEnvironment/ThreadPool.cpp
        src/BasicUsageEnvironment/Timer.cpp
        src/BasicUsageEnvironment/UsageEnvironment.cpp
        src/main.cpp
        )