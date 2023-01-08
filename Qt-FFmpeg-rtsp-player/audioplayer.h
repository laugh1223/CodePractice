#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <QThread>
#include <QAudioFormat>
#include <QAudioOutput>
#include <QMutex>

extern "C"
{
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
    #include "libavutil/pixfmt.h"
    #include "libswscale/swscale.h"
    #include "libswresample/swresample.h"
}

class audioplayer : public QThread
{
    Q_OBJECT
public:
    explicit audioplayer();
    ~audioplayer();

    void startPlay(QString url);

signals:

protected:
    void run();

private:
    QString m_strFileName;
    QAudioOutput *m_audioPlay;
//    QMutex m_mutex;
};

#endif // AUDIOPLAYER_H
