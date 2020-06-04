#include "networkhandler.h"

NetworkHandler::NetworkHandler(struct NetworkParams p, QObject *parent) : QObject(parent), m_np(p)
{
    m_socket = nullptr;
    m_firstReceive = false;
}

void NetworkHandler::init()
{
    m_np.initMutex.lock();
    if (!m_socket)
        m_socket = new QUdpSocket(this);

    QHostAddress addr(QHostAddress::AnyIPv4);
    if (m_socket->bind(addr, IP_PORT, QUdpSocket::ReuseAddressHint) == false) {
        m_np.init = false;
    } else {
        m_np.init = true;
    }
    m_np.initMutex.unlock();
}

void NetworkHandler::start()
{
    if (m_socket->hasPendingDatagrams()) {
        char tmp[DGRAMM_SIZE];
        m_socket->readDatagram(tmp, DGRAMM_SIZE);
        m_socket->readAll();
    }
    connect(m_socket, &QUdpSocket::readyRead, this, &NetworkHandler::receive);
    QHostAddress remote(REMOTE_ADDR);
    m_socket->writeDatagram("1", 1, remote, REMOTE_PORT);
}

void NetworkHandler::stop()
{
    if (m_np.totalBytes == 0)
        emit dataReceived();
    disconnect(m_socket, &QUdpSocket::readyRead, this, &NetworkHandler::receive);
}

void NetworkHandler::receive()
{
    char tmp[DGRAMM_SIZE];
    int bytes = m_socket->readDatagram(tmp, DGRAMM_SIZE);
    if (bytes == -1) {
        emit badReceive(m_socket->errorString());
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
    if (idx++ == 0) {
        if (m_np.override) {
            emit overrideBuffer();
        }
        m_np.override = true;
    }
    m_np.overrideMutex.unlock();
}
