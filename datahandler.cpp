#include "datahandler.h"

DataHandler::DataHandler(DataHandlerParams dataHandlerParams, QObject *parent) : QObject(parent), m_params(dataHandlerParams)
{

}

DataHandler::~DataHandler() {
    if (m_file.isOpen())
        m_file.close();
}

void DataHandler::prepareFile()
{
    if (!m_file.isOpen()) {
        m_file.setFileName(m_params.filename);
        if (!m_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            return;
        }
    }
    m_params.initMutex.lock();
    m_params.init = true;
    m_params.initMutex.unlock();
}

void DataHandler::write()
{
    QString str(DGRAMM_SIZE);
    QTextStream stream(&m_file);
    while (true) {
        getParams();
        if (m_params.writtingTerminated)
            break;
        if ((m_receiveIndex > m_writtenIndex && !m_overrided) || m_overrided) {
            m_params.bufferMutex.lock();
            str = &m_params.buffer[m_writtenIndex * DGRAMM_SIZE];
            m_params.bufferMutex.unlock();

            stream << str;

            m_params.writtenIndexMutex.lock();
            m_params.writtenIndex++;
            m_params.writtenIndexMutex.unlock();
            m_writtenIndex++;

            if (m_overrided && m_writtenIndex == 0) {
                m_params.overrideMutex.lock();
                m_params.overrided = false;
                m_params.overrideMutex.unlock();
            }
        } else {
            if (m_receiveTerminated) {
                break;
            } else {
                QThread::usleep(10);
            }
        }
    }

    m_file.close();
    emit dataWritten();
}

void DataHandler::getParams() {
    m_params.overrideMutex.lock();
    m_overrided = m_params.overrided;
    m_params.overrideMutex.unlock();

    m_params.receiveTerminatedMutex.lock();
    m_receiveTerminated = m_params.receiveTerminated;
    m_params.receiveTerminatedMutex.unlock();

    m_params.receiveIndexMutex.lock();
    m_receiveIndex = m_params.receiveIndex;
    m_params.receiveIndexMutex.unlock();

    m_params.writtenIndexMutex.lock();
    m_writtenIndex = m_params.writtenIndex;
    m_params.writtenIndexMutex.unlock();
}
