#include "networkhandler.h"
#include <QDebug>

NetworkHandler::NetworkHandler(NetworkHandlerParams p, QObject * parent) : QObject(parent), params (p)
{
    dataStartedWrite = false;
}

NetworkHandler::~NetworkHandler()
{

}

void NetworkHandler::initNetworkConnection()
{
    dataStartedWrite = false;
    params.networkInitMutex.lock();
    if (params.networkInitialized) {
        params.networkInitMutex.unlock();
        return;
    }
    socket = new QUdpSocket;
    serverAddress.setAddress(REMOTE_IP_ADDR);
    if (socket->bind(QHostAddress::Any, DEFAULT_IP_PORT, QAbstractSocket::ReuseAddressHint) == false) {
        //this->thread()->exit(-1);
        params.networkInitMutex.unlock();
        return;
    }

    if (socket->isValid()) {
        connect(socket, &QUdpSocket::readyRead, this, &NetworkHandler::receive);
        params.networkInitialized = true;
        params.networkInitMutex.unlock();
        return;
    } else {
        //this->thread()->exit(-1);
        params.networkInitMutex.unlock();
        return;
    }
}

void NetworkHandler::receive()
{
    params.bufferMutex.lock();
    int bytes = socket->readDatagram(&params.buffer[params.index * PACKET_SIZE], PACKET_SIZE);
    params.bufferMutex.unlock();


    if (!bytes) {
    }

    if (!params.totalBytes) {
        emit dataReceived(QThread::InheritPriority);
        dataStartedWrite = true;
    }
    params.indexMutex.lock();
    params.index++;
    params.indexMutex.unlock();

    if (params.index == 65535) {
        params.overwriteMutex.lock();
        if (!params.overwrite)
            params.overwrite = true;
        else
            emit dataOverwritten();
        params.overwriteMutex.unlock();
    }

    params.totalBytes += bytes;
    params.totalDatagrams++;
}

void NetworkHandler::stop()
{
    if (!dataStartedWrite)
        emit dataReceived();
    socket->close();
    disconnect(socket, &QUdpSocket::readyRead, this, &NetworkHandler::receive);
    params.networkInitialized = false;
}

void NetworkHandler::start()
{
    serverAddress.setAddress(REMOTE_IP_ADDR);
    if (socket->isValid()) {
        socket->writeDatagram("1", serverAddress, REMOTE_IP_PORT);
    } else {
        this->thread()->exit(-1);
    }
}


