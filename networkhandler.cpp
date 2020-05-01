#include "networkhandler.h"

NetworkHandler::NetworkHandler(struct NetworkParams p, QObject *parent) : QObject(parent), m_np(p)
{
    m_firstReceive = false;
}

void NetworkHandler::init()
{
    if (!m_socket)
        m_socket = new QUdpSocket(this);
    QHostAddress addr(QHostAddress::AnyIPv4);
    if (m_socket->bind(addr, IP_PORT, QUdpSocket::ReuseAddressHint) == false) {
        emit connectionStatus(0);
    } else {
        emit connectionStatus(1);
    }
}

void NetworkHandler::start()
{
    connect(m_socket, &QUdpSocket::readyRead, this, &NetworkHandler::receive);
}

void NetworkHandler::stop()
{
    disconnect(m_socket, &QUdpSocket::readyRead, this, &NetworkHandler::receive);
}

void NetworkHandler::receive()
{
    char tmp[DGRAMM_SIZE];
    int bytes = m_socket->readDatagram(tmp, DGRAMM_SIZE);
    if (bytes == -1) {
        stop();
        emit badReceive();
        return;
    }

    if (m_np.totalBytes == 0)
        emit dataReceived();
    m_np.totalBytes += bytes;

    m_np.receiveIndexMutex.lock();
    unsigned short idx = m_np.receiveIndex;
    m_np.receiveIndex++;
    m_np.receiveIndexMutex.unlock();

    m_np.bufferMutex.lock();
    strcpy(&m_np.buffer[idx * DGRAMM_SIZE], tmp);
    m_np.bufferMutex.unlock();

    m_np.overrideMutex.lock();
    if (idx + 1 == 0) {
        if (m_np.override) {
            stop();
            emit overrideBuffer();
        }
        m_np.override = true;
    }
    m_np.overrideMutex.unlock();
}
