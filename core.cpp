#include "core.h"

#include <QDebug>

Core::Core(QObject *parent) : QObject(parent)
{
    m_window = new Window;

    reset();
    createNetworkHandler();
}

Core::~Core()
{
    delete m_window;
    if (m_buffer)
        delete [] m_buffer;
}

void Core::operator()()
{
    m_window->show();
    init();
}

void Core::init()
{
    emit initNetwork();
    m_networkThread.wait();
    qDebug() << "Core" << endl;
}

void Core::createDataHandler() {

}

void Core::createNetworkHandler() {
    m_network = new NetworkHandler({
        m_bufferMutex,
        m_receiveIndexMutex,
        m_overrideMutex,
        m_totalBytes,
        m_buffer,
        m_receiveIndex,
        m_override,
        m_terminateReceiving});

    m_network->moveToThread(&m_networkThread);

    connect(this, &Core::initNetwork, m_network, &NetworkHandler::init);
    connect(this, &Core::startReceiving, m_network, &NetworkHandler::start);
    connect(this, &Core::stopReceiving, m_network, &NetworkHandler::stop);

    m_networkThread.start();
}

void Core::reset()
{
    try {
        if(m_buffer)
            delete [] m_buffer;
        m_buffer = new char[BUFFER_SIZE * DGRAMM_SIZE];
    } catch (std::bad_alloc e) {
        QMessageBox msg(QMessageBox::Icon::Critical, "Bad alloc", "Ошибка выделения памяти");
        msg.show();
        this->deleteLater();
        return;
    }
    catch (...) {
        QMessageBox msg(QMessageBox::Icon::Critical, "Error", "Неизвестная ошибка");
        msg.show();
        this->deleteLater();
        return;
    }


    m_totalBytes = 0;
    m_totalDgramms = 0;
    m_receiveIndex = 0;
    m_writtenIndex = 0;

    m_override = false;
    m_dataWritten = true;
    m_terminateReceiving = true;
}

void Core::changeTime()
{

}

void Core::startReceive()
{

}

void Core::stopReceiveByTimer()
{

}

void Core::stopReceiveByInterrupt()
{

}
