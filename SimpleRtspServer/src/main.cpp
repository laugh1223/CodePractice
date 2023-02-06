#include "BasicUsageEnvironment/EventScheduler.h"
#include "BasicUsageEnvironment/ThreadPool.h"
#include "BasicUsageEnvironment/UsageEnvironment.h"
#include "BasicUsageEnvironment/Log.h"
#include "liveMedia/MediaSessionManager.h"
#include "liveMedia/RtspServer.h"
#include "liveMedia/H264FileMediaSource.h"
#include "liveMedia/H264FileSink.h"
#include "liveMedia/AACFileMediaSource.h"
#include "liveMedia/AACFileSink.h"

/*
程序初始化了一份session名为test的资源
使用ffplay客户端测试指令如下：
// rtp over tcp
ffplay -i -rtsp_transport tcp  rtsp://127.0.0.1:8444/test
// rtp over udp
ffplay -i rtsp://127.0.0.1:8444/test
*/

int main() {
    srand(time(NULL));//时间初始化，初始化随机种子，用于产生随机的ssrc

    EventScheduler* scheduler = EventScheduler::createNew(EventScheduler::POLLER_SELECT);
    ThreadPool* threadPool = ThreadPool::createNew(1);
    MediaSessionManager* sessMgr = MediaSessionManager::createNew();
    UsageEnvironment* env = UsageEnvironment::createNew(scheduler, threadPool);
 
    Ipv4Address rtspAddr("127.0.0.1", 8444);
    RtspServer* rtspServer = RtspServer::createNew(env, sessMgr,rtspAddr);

    LOGI("----------session init start------");
    {
        MediaSession* session = MediaSession::createNew("test");
        MediaSource* source = H264FileMediaSource::createNew(env, "../../data/daliu.h264");
        Sink* sink = H264FileSink::createNew(env, source);
        session->addSink(MediaSession::TrackId0, sink);

        source = AACFileMeidaSource::createNew(env, "../../data/daliu.aac");
        sink = AACFileSink::createNew(env, source);
        session->addSink(MediaSession::TrackId1, sink);

        sessMgr->addSession(session);
    }
    LOGI("----------session init end------");
    rtspServer->start();
    env->scheduler()->loop();
    return 0;
}