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

    long long & totalBytes;             //Total bytes amount

    char * buffer;                      //buffer

    unsigned short & receiveIndex;      //Index in buffer

    bool & init;                        //Is init?
    bool & term;                        //Stop receiving flag
    bool & override;                    //Override circle buffer
};

struct DataHandlerParams {
    QString filename;

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
    bool & written;
    bool & overrided;
    bool & receiveTerminated;
};


#endif // CONFIG_H
