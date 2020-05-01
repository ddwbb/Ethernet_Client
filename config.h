#ifndef CONFIG_H
#define CONFIG_H

#define BUFFER_SIZE 65535
#define DGRAMM_SIZE 1440

#include <QMutex>

struct NetworkParams {
    QMutex & bufferMutex;
    QMutex & receiveIndexMutex;
    QMutex & overrideMutex;

    long long & totalBytes;             //total bytes amount

    char * buffer;                      //buffer

    unsigned short & receiveIndex;      //index in buffer
    bool & override;                    //override circle buffer
    bool & term;                        //stop receiving flag
};


#endif // CONFIG_H
