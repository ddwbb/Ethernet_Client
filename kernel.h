#ifndef KERNEL_H
#define KERNEL_H

#include "mainwindow.h"
#include "networkhandler.h"
#include "outputstream.h"

#include <new>
#include <fstream>

#include <QFile>
#include <QMutex>
#include <QTimer>
#include <QThread>
#include <QString>
#include <QObject>
#include <QMessageBox>

#define MAX_BUFFER_SIZE 1440 * 65536

Q_DECLARE_METATYPE(QThread::Priority);

using namespace std;

class Kernel : public QObject
{
    Q_OBJECT
    QMutex bufferMutex;
    QMutex writtenDataIndexMutex;
    QMutex receivedDataIndexMutex;
    QMutex receivedMutex;
    QMutex doneMutex;
    QMutex overwriteMutex;
    QMutex networkInitMutex;

    MainWindow * window;
    OutputStream * writer;
    NetworkHandler * receiver;

    QFile file;
    QString fileName;

    ofstream outputStream;

    char * buffer;

    unsigned short writtenDataIndex;
    unsigned short receivedDataIndex;

    long long totalBytes;
    long long totalDatagrams;

    bool done;
    bool received;
    bool overwrite;
    bool initialized;
    bool stopReceiving;
    bool networkInitialized;

    int time;

    QTimer receivingTimer;
    QTimer viewUpdateTimer;

    QThread writerThread;
    QThread receivingThread;
public:
    explicit Kernel(QObject * = nullptr);
    ~Kernel();

    void init();
    void reset();

public slots:
    void stop();
    void start();
    void endWriting();
    void endReceiving();

private slots:
    void updateTime();

signals:
    void receiveStoped();
    void started();
    void networkInit();
};

#endif // KERNEL_H
