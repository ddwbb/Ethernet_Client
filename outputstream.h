#ifndef OUTPUTSTREAM_H
#define OUTPUTSTREAM_H

#include <QFile>
#include <QMutex>
#include <QObject>
#include <QThread>

#define PACKET_SIZE 1440

struct OutputStreamParams {
    QMutex & doneMutex;
    QMutex & receivedDataIndexMutex;
    QMutex & bufferMutex;
    QMutex & overwriteMutex;
    QMutex & receivedMutex;

    QFile & file;

    char * buffer;
    bool & done;
    bool & received;
    bool & overwrite;
    unsigned short & receivedDataIndex;
};

class OutputStream : public QObject
{
    Q_OBJECT
    OutputStreamParams params;
    unsigned short writtenDataIndex;
public:
    explicit OutputStream(OutputStreamParams, QObject * = nullptr);

public slots:
    void write();
};

#endif // OUTPUTSTREAM_H
