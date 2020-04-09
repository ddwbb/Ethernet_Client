#ifndef NETWORKHANDLER_H
#define NETWORKHANDLER_H

#include <QMutex>
#include <QObject>
#include <QThread>
#include <QUdpSocket>

#define PACKET_SIZE 1440
#define REMOTE_IP_ADDR "192.168.0.25"
#define REMOTE_IP_PORT 4900
#define DEFAULT_IP_PORT 49001

struct NetworkHandlerParams{
    QMutex & indexMutex;
    QMutex & bufferMutex;
    QMutex & overwriteMutex;
    QMutex & networkInitMutex;

    char * buffer;
    bool & overwrite;

    unsigned short & index;

    long long & totalBytes;
    long long & totalDatagrams;
    bool & networkInitialized;
};

class NetworkHandler : public QObject
{
    Q_OBJECT
    QUdpSocket * socket;
    QHostAddress serverAddress;

    NetworkHandlerParams params;

    bool dataStartedWrite;

public:
    explicit NetworkHandler(NetworkHandlerParams, QObject * p = nullptr);
    ~NetworkHandler();


public slots:
    void stop();
    void start();
    void receive();
    void initNetworkConnection();

signals:
    void dataReceived(QThread::Priority = QThread::InheritPriority);
    void dataOverwritten();
};

#endif // NETWORKHANDLER_H
