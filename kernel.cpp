#include "kernel.h"

Kernel::Kernel(QObject * parent) : QObject(parent)
{
    try {
        buffer = new char[MAX_BUFFER_SIZE];
    } catch(bad_alloc &err) {
        QString err_message = "Произошла ошибка во время попытки выделения памяти под кольцевой буфер";
        err_message += err.what();
        QMessageBox message(QMessageBox::Icon::Critical, "Ошибка выделения памяти", err_message);
        return;
    }
    window = new MainWindow;

    NetworkHandlerParams networkParams = {
        receivedDataIndexMutex,
        bufferMutex,
        overwriteMutex,
        networkInitMutex,
        buffer,
        overwrite,
        receivedDataIndex,
        totalBytes,
        totalDatagrams,
        networkInitialized
    };

    OutputStreamParams outputParams = {
        doneMutex,
        receivedDataIndexMutex,
        bufferMutex,
        overwriteMutex,
        receivedMutex,
        file,
        buffer,
        done,
        received,
        overwrite,
        receivedDataIndex
    };

    writer = new OutputStream(outputParams);
    receiver = new NetworkHandler(networkParams);

    writtenDataIndex = 0;
    receivedDataIndex = 0;

    totalBytes = 0;
    totalDatagrams = 0;

    done = true;
    received = false;
    overwrite = false;
    initialized = false;
    stopReceiving = false;
    networkInitialized = false;

    time = 0;

    qRegisterMetaType<QThread::Priority>("QThread::Priority");

    connect(window, &MainWindow::stoped, this, &Kernel::stop);
    connect(window, &MainWindow::started, this, &Kernel::start);
    connect(window, &MainWindow::destroyed, this, &Kernel::deleteLater);

    connect(this, &Kernel::receiveStoped, receiver, &NetworkHandler::stop);
    connect(this, &Kernel::started, receiver, &NetworkHandler::start);
    connect(this, &Kernel::networkInit, receiver, &NetworkHandler::initNetworkConnection);

    connect(&receivingThread, &QThread::finished, receiver, &NetworkHandler::deleteLater);
    //connect(&receivingThread, &QThread::started, receiver, &NetworkHandler::start);
    connect(receiver, &NetworkHandler::dataReceived, &writerThread, &QThread::start);
    connect(&writerThread, &QThread::started, writer, &OutputStream::write);
    connect(&writerThread, &QThread::finished, this, &Kernel::endWriting);

    connect(&receivingTimer, &QTimer::timeout, this, &Kernel::stop);
    connect(&viewUpdateTimer, &QTimer::timeout, this, &Kernel::updateTime);

    viewUpdateTimer.setInterval(1000);
    viewUpdateTimer.start();

    receiver->moveToThread(&receivingThread);

    receivingThread.start();

    window->show();
}

Kernel::~Kernel()
{
    stop();
    delete window;
    if (receivingThread.isRunning())
        receivingThread.exit();
        receivingThread.wait();
    delete receiver;
    delete writer;
}

void Kernel::init()
{
    initialized = false;
    emit networkInit();
    QThread::msleep(100);
    networkInitMutex.lock();
    if (!networkInitialized) {
        networkInitMutex.unlock();
        return;
    }
    networkInitMutex.unlock();
    fileName = window->getFileName();
    file.setFileName(fileName);
    if (!file.isOpen() && !file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        window->setErrorState(ErrorType::file);
        return;
    }
    initialized = true;
}

void Kernel::stop()
{
    if (!stopReceiving && !done) {
        emit receiveStoped();
        received = true;
        receivingTimer.stop();
        stopReceiving = true;
        writerThread.quit();
        initialized = false;
        time = 0;
    }
}

void Kernel::reset()
{

}

void Kernel::endWriting()
{
    file.close();
    window->setDefaultState();
}

void Kernel::endReceiving()
{

}

void Kernel::start()
{
    init();
    if (!initialized) {
        return;
    }
    done = false;
    received = false;
    overwrite = false;
    stopReceiving = false;
    writer->moveToThread(&writerThread);
    window->setWorkingState();
    time = window->getTime();
    emit started();

    receivingTimer.start(time * 1000);
    receivingThread.start();
}

void Kernel::updateTime()
{
    if (time && initialized) {
        time--;
        window->setTime(time);
    }
}

