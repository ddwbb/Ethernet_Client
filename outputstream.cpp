#include "outputstream.h"
#include <QDebug>

OutputStream::OutputStream(OutputStreamParams p, QObject * parent) : QObject(parent), params(p)
{
    writtenDataIndex = 0;
}

void OutputStream::write()
{
    bool received;
    char tmp[PACKET_SIZE + 1];
    tmp[PACKET_SIZE] = '\0';
    qDebug() << "output";
    while (1) {

        params.receivedDataIndexMutex.lock();
        params.overwriteMutex.lock();
        received = ((writtenDataIndex < params.receivedDataIndex) && (!params.overwrite)) || (params.overwrite);
        params.receivedDataIndexMutex.unlock();
        params.overwriteMutex.unlock();

        if (received) {
            params.bufferMutex.lock();
            strncpy(tmp, &params.buffer[writtenDataIndex * PACKET_SIZE], PACKET_SIZE);
            params.bufferMutex.unlock();

            params.file.write(tmp);

            writtenDataIndex++;
            if (writtenDataIndex == 0) {
                params.overwriteMutex.lock();
                params.overwrite = false;
                params.overwriteMutex.unlock();
            }
        } else {
            params.receivedMutex.lock();
            bool r = params.received;
            params.receivedMutex.unlock();

            if (!r) {
                QThread::msleep(100);
            } else {
                break;
            }
        }
    }

    params.doneMutex.lock();
    params.done = true;
    params.doneMutex.unlock();
    this->thread()->quit();
}
