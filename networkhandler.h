#ifndef NETWORKHANDLER_H
#define NETWORKHANDLER_H

#include <cstring>

#include <QObject>
#include <QUdpSocket>
#include <QNetworkDatagram>

#include "config.h"

#define REMOTE_ADDR "192.168.0.25"
#define REMOTE_PORT 4900
#define IP_PORT 49001

class NetworkHandler : public QObject
{
    Q_OBJECT
public:
    explicit NetworkHandler(struct NetworkParams , QObject *parent = nullptr);

public slots:
    void init();                            //initialize socket
    void start();                           //start receiving
    void stop();                            //stop receiving
    void receive();                         //receive datagramms

signals:
    void connectionStatus(int);             //connection init status
    void badReceive(QString);                      //error while receiving
    void overrideBuffer();                  //override buffer (receive speed > handle speed)
    void dataReceived();                    //data received and should be handle
private:
    QUdpSocket * m_socket;                  //Socket

    struct NetworkParams m_np;              //Network Parametrs

    bool m_firstReceive;                    //maybe I need it
};

#endif // NETWORKHANDLER_H
