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

    if (m_networkThread.isRunning())
        m_networkThread.terminate();

    m_network->deleteLater();
    m_networkThread.deleteLater();
}

void Core::operator()()
{
    m_window->show();
    init();
}

void Core::init()
{
    emit initNetwork();
    QThread::usleep(100);
    m_networkInitMutex.lock();
    if (m_networkInitialized) {
        qDebug() << "Seems good" << endl;


        ///
        m_initialized = true;
        startReceive();
        ///
        QThread::sleep(2);
        stopReceiving();
    } else {
        qDebug() << "Not good" << endl;
    }
    m_networkInitMutex.unlock();
    qDebug() << "Core" << endl;



    ///

}

void Core::createDataHandler() {

}

void Core::createNetworkHandler() {
    m_network = new NetworkHandler({
        m_networkInitMutex,
        m_bufferMutex,
        m_overrideMutex,
        m_receiveIndexMutex,
        m_totalBytes,
        m_buffer,
        m_receiveIndex,
        m_networkInitialized,
        m_receivingTerminated,
        m_overrided});

    m_network->moveToThread(&m_networkThread);

    connect(this, &Core::initNetwork, m_network, &NetworkHandler::init);
    connect(this, &Core::receivingStarted, m_network, &NetworkHandler::start);
    connect(this, &Core::receivingStoped, m_network, &NetworkHandler::stop);

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

    m_overrided = false;
    m_dataWritten = true;
    m_initialized = false;
    m_networkInitialized = false;
    m_receivingTerminated = false;
}

void Core::changeTime()
{
    if (m_timeInSec >= 0) {
        m_timeInSec--;
        //set time
    }
}

void Core::startReceive()
{
    //get time
    if (!m_initialized && !m_timeInSec) return;

    reset();
    m_dataWritten = false;
    emit receivingStarted();

    //set timers
    //set Working state
    //
}

void Core::stopReceiving()
{
    if (!m_receivingTerminated) {
        m_receivingTerminated = true;
        emit receivingStoped();

        //set stoping state
        //timer off
        //set time
    }
}

void Core::stopReceiveByTimer()
{

}

void Core::stopReceiveByInterrupt()
{

}
