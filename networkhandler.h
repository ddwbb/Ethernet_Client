#ifndef NETWORKHANDLER_H
#define NETWORKHANDLER_H

#include <cstring>

#include <QObject>
#include <QUdpSocket>

#include "config.h"

#define REMOTE_ADDR "192.168.0.1"
#define REMOTE_PORT 49001
#define IP_PORT 4900

class NetworkHandler : public QObject
{
    Q_OBJECT
public:
    explicit NetworkHandler(struct NetworkParams , QObject *parent = nullptr);

public slots:
    void init();
    void start();
    void stop();
    void receive();

signals:
    void connectionStatus(int);
    void badReceive();
private:
    QUdpSocket * m_socket;                  //Socket

    struct NetworkParams m_np;              //Network Parametrs

    bool m_firstReceive;
};

#endif // NETWORKHANDLER_H
