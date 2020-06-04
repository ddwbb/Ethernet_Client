#ifndef CONFIG_H
#define CONFIG_H

#define BUFFER_SIZE 65535
#define DGRAMM_SIZE 1440

#include <QMutex>
#include <QString>

struct NetworkParams {
    QMutex & initMutex;
    QMutex & bufferMutex;
    QMutex & overrideMutex;
    QMutex & receiveIndexMutex;

    long long & totalBytes;

    char * buffer;

    unsigned short & receiveIndex;

    bool & init;
    bool & term;
    bool & override;
};

struct DataHandlerParams {
    QString & filename;

    QMutex & initMutex;
    QMutex & bufferMutex;
    QMutex & overrideMutex;
    QMutex & receiveIndexMutex;
    QMutex & writtenIndexMutex;
    QMutex & receiveTerminatedMutex;

    char * buffer;
    unsigned short & receiveIndex;
    unsigned short & writtenIndex;

    bool & init;
    bool & overrided;
    bool & receiveTerminated;
    bool & writtingTerminated;
};


#endif // CONFIG_H
