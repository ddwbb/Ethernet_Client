#ifndef CONFIG_H
#define CONFIG_H

#define BUFFER_SIZE 65535
#define DGRAMM_SIZE 1440

#include <QMutex>

struct NetworkParams {
    QMutex & initMutex;
    QMutex & bufferMutex;
    QMutex & receiveIndexMutex;
    QMutex & overrideMutex;

    long long & totalBytes;             //Total bytes amount

    char * buffer;                      //buffer

    unsigned short & receiveIndex;      //Index in buffer

    bool & init;                        //Is init?
    bool & term;                        //Stop receiving flag
    bool & override;                    //Override circle buffer
};


#endif // CONFIG_H
