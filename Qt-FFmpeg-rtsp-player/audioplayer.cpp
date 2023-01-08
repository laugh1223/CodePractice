#include "audioplayer.h"
#include <QDebug>
#include <QFile>
#include <thread>
QFile g_file;
audioplayer::audioplayer()
{

    QAudioFormat fmt;
    fmt.setSampleRate(44100);                       //样本率
    fmt.setSampleSize(16);                         //设置样本大小
    fmt.setChannelCount(2);                        //设置声道数
    fmt.setCodec("audio/pcm");                     //设置播放格式
    fmt.setByteOrder(QAudioFormat::LittleEndian);  //设置字节模式
    fmt.setSampleType(QAudioFormat::UnSignedInt);  //设置样本类型
    m_audioPlay = new QAudioOutput(fmt);
}

audioplayer::~audioplayer()
{
//    if(m_audioPlay)
//    {
//        delete m_audioPlay;
//        m_audioPlay = nullptr;
//    }
}

void audioplayer::startPlay(QString url)
{
    //调用start函数将会自动执行run函数
    m_strFileName = url;
    this->start();
}

void audioplayer::run()
{
#if 0
    //测试qt的音频播放器
    QIODevice* io = m_audioPlay->start();
    int size = m_audioPlay->periodSize();     // 这是每个周期防止缓冲区欠载和确保不间断播放所需的数据量。
    g_file.setFileName("D:/WorkSpace/wuzj/Coding/code_practice/QTDemo/Qt-FFmpeg-rtsp-player/test.pcm");
    g_file.open(QIODevice::ReadOnly);
    QByteArray buf = g_file.readAll();    // 将需要播放的音频数据读到buf中
    char *tmpBuff = buf.data();
    g_file.close();
    while (!buf.isEmpty())
    {
        if (m_audioPlay->bytesFree() < size)   // 音频缓冲区中可用的空闲字节数，判断缓冲区是否可写入数据。
        {
            QThread::msleep(1);
            continue;
        }
        qDebug() << io->write(buf.mid(0, size));   // 写入需要播放的数据
        buf.remove(0, size);
//        qDebug() << buf.count();
    }

    io->close();
    delete io;
    io = nullptr;
#endif

#if 1
    AVFormatContext *pAVFormatContext = 0;          // ffmpeg的全局上下文，所有ffmpeg操作都需要
    AVStream *pAVStream = 0;                        // ffmpeg流信息
    AVCodecContext *pAVCodecContext = 0;            // ffmpeg编码上下文
    AVCodec *pAVCodec = 0;                          // ffmpeg编码器
    AVPacket *pAVPacket = 0;                        // ffmpag单帧数据包
    AVFrame *pAVFrame = 0;                          // ffmpeg单帧缓存
    SwrContext *pSwrContext = 0;                    // ffmpeg音频转码

    int ret = 0;                                    // 函数执行结果
    int audioIndex = -1;                            // 音频流所在的序号
    int numBytes = 0;                               // 音频采样点字节数
    uint8_t *outData[8] = {0};                        // 音频缓存区（不带P的）
    int dstNbSamples = 0;                           // 解码目标的采样率

    int outChannel = 0;                             // 重采样后输出的通道
    AVSampleFormat outFormat = AV_SAMPLE_FMT_NONE;  // 重采样后输出的格式
    int outSampleRate = 0;                          // 重采样后输出的采样率

    pAVFormatContext = avformat_alloc_context();    // 分配
    pAVPacket = av_packet_alloc();                  // 分配
    pAVFrame = av_frame_alloc();                    // 分配

    if(!pAVFormatContext || !pAVPacket || !pAVFrame)
    {
        qDebug() << "Failed to alloc";
//        goto END;
        return ;
    }

    // 步骤一：注册所有容器和编解码器（也可以只注册一类，如注册容器、注册编码器等）
    av_register_all();

    // 步骤二：打开文件(ffmpeg成功则返回0)
    ret = avformat_open_input(&pAVFormatContext, m_strFileName.toUtf8().data(), 0, 0);
    if(ret)
    {
        qDebug() << "Failed";
//        goto END;
        return ;
    }

    // 步骤三：探测流媒体信息
    ret = avformat_find_stream_info(pAVFormatContext, 0);
    if(ret < 0)
    {
        qDebug() << "Failed to avformat_find_stream_info(pAVCodecContext, 0)";
//        goto END;
        return ;
    }

    // 步骤四：提取流信息,提取视频信息
    for(int index = 0; index < pAVFormatContext->nb_streams; index++)
    {
        pAVCodecContext = pAVFormatContext->streams[index]->codec;
        pAVStream = pAVFormatContext->streams[index];
        switch (pAVCodecContext->codec_type)
        {
        case AVMEDIA_TYPE_UNKNOWN:
            qDebug() << "流序号:" << index << "类型为:" << "AVMEDIA_TYPE_UNKNOWN";
            break;
        case AVMEDIA_TYPE_VIDEO:
            qDebug() << "流序号:" << index << "类型为:" << "AVMEDIA_TYPE_VIDEO";
            break;
        case AVMEDIA_TYPE_AUDIO:
            qDebug() << "流序号:" << index << "类型为:" << "AVMEDIA_TYPE_AUDIO";
            audioIndex = index;
            break;
        case AVMEDIA_TYPE_DATA:
            qDebug() << "流序号:" << index << "类型为:" << "AVMEDIA_TYPE_DATA";
            break;
        case AVMEDIA_TYPE_SUBTITLE:
            qDebug() << "流序号:" << index << "类型为:" << "AVMEDIA_TYPE_SUBTITLE";
            break;
        case AVMEDIA_TYPE_ATTACHMENT:
            qDebug() << "流序号:" << index << "类型为:" << "AVMEDIA_TYPE_ATTACHMENT";
            break;
        case AVMEDIA_TYPE_NB:
            qDebug() << "流序号:" << index << "类型为:" << "AVMEDIA_TYPE_NB";
            break;
        default:
            break;
        }
        // 已经找打视频品流
        if(audioIndex != -1)
        {
            break;
        }
    }
    if(audioIndex == -1 || !pAVCodecContext)
    {
        qDebug() << "Failed to find video stream";
//        goto END;
        return ;
    }

    // 步骤五：对找到的音频流寻解码器
    pAVCodec = avcodec_find_decoder(pAVCodecContext->codec_id);
    if(!pAVCodec)
    {
        qDebug() << "Fialed to avcodec_find_decoder(pAVCodecContext->codec_id):"
            << pAVCodecContext->codec_id;
//        goto END;
        return ;
    }

    // 步骤六：打开解码器
    ret = avcodec_open2(pAVCodecContext, pAVCodec, NULL);
    if(ret)
    {
        qDebug() << "Failed to avcodec_open2(pAVCodecContext, pAVCodec, pAVDictionary)";
//        goto END;
        return ;
    }

    // 打印
    qDebug() << "code name:" << pAVCodec->name
        << "\n channels: " << pAVCodecContext->channels
        << "\n layout: " << av_get_default_channel_layout(pAVCodecContext->channels)
        << "\n sample rate: " << pAVCodecContext->sample_rate
        << "\n sample fmt: " << pAVCodecContext->sample_fmt;
    outChannel = 2;
    outSampleRate = 44100;
    outFormat = AV_SAMPLE_FMT_S16;

    // 步骤七：获取音频转码器并设置采样参数初始化
    pSwrContext = swr_alloc_set_opts(0,                                 // 输入为空，则会分配
                                     av_get_default_channel_layout(outChannel),
                                     outFormat,                         // 输出的采样频率
                                     outSampleRate,                     // 输出的格式
                                     av_get_default_channel_layout(pAVCodecContext->channels),
                                     pAVCodecContext->sample_fmt,       // 输入的格式
                                     pAVCodecContext->sample_rate,      // 输入的采样率
                                     0,
                                     0);
    ret = swr_init(pSwrContext);
    if(ret < 0)
    {
        qDebug() << "Failed to swr_init(pSwrContext);";
//        goto END;
        return;
    }
    // 最大缓存区，1152个采样样本,16字节,支持最长8个通道
    outData[0] = (uint8_t *)av_malloc(1152 * 2 * 8);

    if(ret)
    {
        qDebug() << "Failed";
        return;
    }


    QIODevice *io = m_audioPlay->start();
    int size = m_audioPlay->periodSize();     // 这是每个周期防止缓冲区欠载和确保不间断播放所需的数据量。

    //将解码后的音频数据保存
    g_file.setFileName("D:/WorkSpace/wuzj/Coding/code_practice/QTDemo/Qt-FFmpeg-rtsp-player/test.pcm");
    g_file.open(QIODevice::WriteOnly | QIODevice::Truncate);

    char *audioBuffer = new char[102400];//将解码后的数据放到这个buff中，播放器从这个buff中获取数据
    int audioLength = 0;

    // 步骤八：读取一帧数据的数据包
    while(av_read_frame(pAVFormatContext, pAVPacket) >= 0)
    {
        if(pAVPacket->stream_index == audioIndex)
        {
            // 步骤九：将封装包发往解码器
            ret = avcodec_send_packet(pAVCodecContext, pAVPacket);
            if(ret)
            {
                qDebug() << "Failed to avcodec_send_packet(pAVCodecContext, pAVPacket) ,ret =" << ret;
                break;
            }
            // 步骤十：从解码器循环拿取数据帧
            while(!avcodec_receive_frame(pAVCodecContext, pAVFrame))
            {
                // nb_samples并不是每个包都相同，遇见过第一个包为47，第二个包开始为1152的
//                qDebug() << pAVFrame->nb_samples;
                // 步骤十一：获取每个采样点的字节大小
                numBytes = av_get_bytes_per_sample(outFormat);
                // 步骤十二：修改采样率参数后，需要重新获取采样点的样本个数
                dstNbSamples = av_rescale_rnd(pAVFrame->nb_samples,
                                              outSampleRate,
                                              pAVCodecContext->sample_rate,
                                              AV_ROUND_ZERO);
                // 步骤十三：重采样
                swr_convert(pSwrContext,
                            outData,
                            dstNbSamples,
                            (const uint8_t **)pAVFrame->data,
                            pAVFrame->nb_samples);
                // 第一次显示
                static bool show = true;
                if(show)
                {
                    qDebug() << numBytes << pAVFrame->nb_samples << "to" << dstNbSamples;
                    show = false;
                }

                unsigned int length = numBytes * dstNbSamples * outChannel;
                memset(audioBuffer, 0, 102400);
                memcpy(audioBuffer + audioLength, (const char *)outData[0], length);
                audioLength += length;
                const char *buff =  audioBuffer;
                while(audioLength > 0)
                {
                    if (m_audioPlay->bytesFree() < size)   // 音频缓冲区中可用的空闲字节数，判断缓冲区是否可写入数据。
                    {
                        QThread::msleep(1);
                        continue;
                    }
                    int writeSize = io->write(buff, size);   // 写入需要播放的数据
                    buff += writeSize;
                    audioLength -= writeSize;
                }
                g_file.write((const char *)outData[0], length);
            }
            av_free_packet(pAVPacket);
        }
    }
    g_file.close();
    delete []audioBuffer;
END:
    qDebug() << "释放回收资源";
    if(outData[0])
    {
        av_free(outData[0]);
        outData[0] = 0;
        qDebug() << "av_free(outData)";
    }
    if(pSwrContext)
    {
        swr_free(&pSwrContext);
        pSwrContext = 0;
    }
    if(pAVFrame)
    {
        av_frame_free(&pAVFrame);
        pAVFrame = 0;
        qDebug() << "av_frame_free(pAVFrame)";
    }
    if(pAVPacket)
    {
        av_free_packet(pAVPacket);
        pAVPacket = 0;
        qDebug() << "av_free_packet(pAVPacket)";
    }
    if(pAVCodecContext)
    {
        avcodec_close(pAVCodecContext);
        pAVCodecContext = 0;
        qDebug() << "avcodec_close(pAVCodecContext);";
    }
    if(pAVFormatContext)
    {
        avformat_close_input(&pAVFormatContext);
        avformat_free_context(pAVFormatContext);
        pAVFormatContext = 0;
        qDebug() << "avformat_free_context(pAVFormatContext)";
    }
#endif
}
