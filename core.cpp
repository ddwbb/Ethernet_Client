#include "core.h"

Core::Core(QObject *parent) : QObject(parent)
{
    m_window = new Window;

    try {
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


    reset();
    createDataHandler();
    createNetworkHandler();

    m_updateTimer.setInterval(1000);

    connect(&m_updateTimer, &QTimer::timeout, this, &Core::changeTime);
    connect(&m_timer, &QTimer::timeout, this, &Core::stopReceiving);

    connect(m_window, &Window::started, this, &Core::startReceive);
    connect(m_window, &Window::stoped, this, &Core::stopReceivingByInterrupt);
}

Core::~Core()
{
    if (!m_receivingTerminated) {
        m_writtingTerminared = true;
        stopReceiving();
    }

    if (m_networkThread.isRunning()) {
        m_networkThread.terminate();
        m_networkThread.wait();
    }
    m_network->deleteLater();



    if (m_dataHandlerThread.isRunning()) {
        m_dataHandlerThread.exit();
        m_dataHandlerThread.wait();
    }
    m_dataHandler->deleteLater();
    m_window->deleteLater();

    if (m_buffer)
        delete [] m_buffer;
}

void Core::operator()()
{
    m_window->show();
}

void Core::init()
{
    m_window->setIndicatorState(Indicators::File, IndicatorState::Off);
    m_window->setIndicatorState(Indicators::Network, IndicatorState::Off);
    m_window->setIndicatorState(Indicators::Writing, IndicatorState::Off);
    m_window->setIndicatorState(Indicators::Receiving, IndicatorState::Off);

    if (!m_networkInitialized)
        emit initNetwork();
    emit initDataHandler();

    m_window->setIndicatorState(Indicators::File, IndicatorState::Processing);
    m_window->setIndicatorState(Indicators::Network, IndicatorState::Processing);

    QThread::usleep(10);
    IndicatorState networkHandlerState = Error;
    m_networkInitMutex.lock();
    if (m_networkInitialized) {
        networkHandlerState = Ok;
    }
    m_networkInitMutex.unlock();
    m_window->setIndicatorState(Indicators::Network, networkHandlerState);

    m_dataHandlerInitMutex.lock();
    IndicatorState dataHandlerState = IndicatorState::Error;
    if (m_dataHandlerInitialized) {
        dataHandlerState = Ok;
    }
    m_dataHandlerInitMutex.unlock();
    m_window->setIndicatorState(Indicators::File, dataHandlerState);

    if (m_networkInitialized && m_dataHandlerInitialized)
        m_initialized = true;
}

void Core::createDataHandler() {
    m_dataHandler = new DataHandler({
        m_filename,
        m_dataHandlerInitMutex,
        m_bufferMutex,
        m_overrideMutex,
        m_receiveIndexMutex,
        m_writtenIndexMutex,
        m_receivingTerminatedMutex,
        m_buffer,
        m_receiveIndex,
        m_writtenIndex,
        m_dataHandlerInitialized,
        m_overrided,
        m_receivingTerminated,
        m_writtingTerminared});

    m_dataHandler->moveToThread(&m_dataHandlerThread);

    connect(m_dataHandler, &DataHandler::dataWritten, this, &Core::end);
    connect(this, &Core::dataReceived, m_dataHandler, &DataHandler::write);
    connect(this, &Core::initDataHandler, m_dataHandler, &DataHandler::prepareFile);

    m_dataHandlerThread.start();
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
    connect(this, &Core::receivingStoped, m_network, &NetworkHandler::stop);
    connect(this, &Core::receivingStarted, m_network, &NetworkHandler::start);
    connect(m_network, &NetworkHandler::dataReceived, this, &Core::startWriteData);
    connect(m_network, &NetworkHandler::badReceive, this, &Core::badReceiveHandle);
    connect(m_network, &NetworkHandler::overrideBuffer, this, &Core::overrideBufferHandle);

    m_networkThread.start();
}

void Core::reset()
{
    m_totalTime = 0;
    m_totalBytes = 0;
    m_totalDgramms = 0;
    m_receiveIndex = 0;
    m_writtenIndex = 0;

    m_overrided = false;
    m_dataWritten = true;
    m_initialized = false;
    m_networkInitialized = false;
    m_receivingTerminated = false;
    m_writtingTerminared = false;
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

    m_filename = m_window->getFileName();
    if (m_timeInSec == 0)
        return;

    init();
    if (!m_initialized)
        return;

    reset();
    m_totalTime = m_timeInSec;
    m_dataWritten = false;
    m_networkInitialized = true;
    emit receivingStarted();

    m_timer.start(m_timeInSec * 1000);
    m_updateTimer.start();

    m_window->setState(WindowState::WokringState);
    m_window->setIndicatorState(Indicators::Receiving, IndicatorState::Processing);
}

void Core::stopReceiving()
{
    if (!m_receivingTerminated) {
        m_totalTime -= m_timeInSec;
        m_receivingTerminated = true;
        emit receivingStoped();
        m_totalDgramms = m_totalBytes / DGRAMM_SIZE;

        m_window->setState(WindowState::StoppingState);
        m_window->setIndicatorState(Indicators::Receiving, IndicatorState::Ok);
        m_updateTimer.stop();
    }
}

void Core::startWriteData()
{
    m_window->setIndicatorState(Indicators::Writing, IndicatorState::Processing);
    emit dataReceived();
}

void Core::stopReceivingByInterrupt()
{
    m_timer.stop();
    stopReceiving();
}

void Core::end()
{
    m_window->setIndicatorState(Indicators::Writing, IndicatorState::Ok);
    m_window->setState(WindowState::BaseState);
    m_window->printResult(m_totalTime, m_totalBytes, m_totalDgramms);
}

void Core::badReceiveHandle(const QString error)
{
    stopReceivingByInterrupt();
    m_window->setIndicatorState(Indicators::Receiving, IndicatorState::Error);
    m_window->setState(WindowState::BaseState);
    m_window->printError(error);
}

void Core::overrideBufferHandle()
{
    stopReceivingByInterrupt();
    m_window->setIndicatorState(Indicators::Writing, IndicatorState::Error);
    m_window->setState(WindowState::BaseState);
    m_window->printError("Скорость записи на диск не позволяет принять больше данных без перезаписи");
}
