#include "core.h"

#include <QDebug>

Core::Core(QObject *parent) : QObject(parent)
{
    m_window = new Window;

    reset();
    createNetworkHandler();

    m_updateTimer.setInterval(1000);

    connect(&m_updateTimer, &QTimer::timeout, this, &Core::changeTime);
    connect(&m_timer, &QTimer::timeout, this, &Core::stopReceiving);

    connect(m_window, &Window::started, this, &Core::startReceive);
    connect(m_window, &Window::stoped, this, &Core::stopReceivingByInterrupt);
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
}

void Core::init()
{
    emit initNetwork();
    m_window->setIndicatorState(Indicators::Network, IndicatorState::Processing);
    QThread::usleep(10);
    m_networkInitMutex.lock();
    if (m_networkInitialized) {
        m_window->setIndicatorState(Indicators::Network, IndicatorState::Ok);
        m_initialized = true;
    } else {
        m_window->setIndicatorState(Indicators::Network, IndicatorState::Error);
    }
    m_networkInitMutex.unlock();
    qDebug() << "Core" << endl;
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
        m_window->setTime(m_timeInSec);
    }
}

void Core::startReceive()
{
    m_timeInSec = m_window->getTime();
    if (m_timeInSec == 0) return;
    init();
    if (!m_initialized) return;

    reset();
    m_dataWritten = false;
    emit receivingStarted();

    m_timer.start(m_timeInSec * 1000);
    m_updateTimer.start();

    m_window->setState(WindowState::WokringState);
    m_window->setIndicatorState(Indicators::Receiving, IndicatorState::Processing);
}

void Core::stopReceiving()
{
    if (!m_receivingTerminated) {
        m_receivingTerminated = true;
        emit receivingStoped();

        m_window->setState(WindowState::StoppingState);
        m_updateTimer.stop();
        m_timeInSec = 0;
        m_window->setTime(m_timeInSec);
    }
}

void Core::stopReceivingByInterrupt()
{
    m_timer.stop();
    stopReceiving();
}
