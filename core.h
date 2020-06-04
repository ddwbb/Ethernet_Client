#ifndef CORE_H
#define CORE_H

#include <vector>
#include <exception>

#include <QTimer>
#include <QThread>
#include <QObject>
#include <QDialog>
#include <QMessageBox>

#include "window.h"
#include "config.h"
#include "datahandler.h"
#include "networkhandler.h"

class Core : public QObject
{
public:
    explicit Core(QObject *parent = nullptr);
    ~Core();

    void operator()();

public slots:


signals:
    void initNetwork();
    void dataReceived();
    void initDataHandler();
    void receivingStarted();
    void receivingStoped();

private slots:
    void startReceive();                            //Start receive data
    void stopReceiving();                           //Stop receiving data
    void startWriteData();
    void stopReceivingByInterrupt();                //Receiving stoped by interrupt
    void end();
    void badReceiveHandle(const QString);
    void overrideBufferHandle();

private:
    Q_OBJECT

    QString m_filename;

    QTimer m_timer;                                 //Receive Timer
    QTimer m_updateTimer;                           //Time update timer

    Window * m_window;                              //Gui
    NetworkHandler * m_network;                     //Network handler + receiver
    DataHandler * m_dataHandler;

    QThread m_networkThread;                        //Network Handler's thread
    QThread m_dataHandlerThread;                    //Data Handler's thread

    QMutex m_bufferMutex;                           //Mutex of circle buffer
    QMutex m_overrideMutex;                         //Mutex of override buffer
    QMutex m_networkInitMutex;                      //Mutex of network init
    QMutex m_dataHandlerInitMutex;                  //Mutex of dataHandlerInitialized
    QMutex m_receiveIndexMutex;                     //Mutex of received data index
    QMutex m_writtenIndexMutex;                     //Mutex of written data index
    QMutex m_receivingTerminatedMutex;

    int m_totalTime;
    int m_timeInSec;                                //Time to receive in sec

    char * m_buffer;                                //Circle buffer
    //std::vector<char[1440]> * m_buffer;

    long m_totalDgramms;                            //Total amount of received dgramms
    long long m_totalBytes;                         //Total amount of received bytes

    unsigned short m_receiveIndex;                  //Received data index in circle buffer
    unsigned short m_writtenIndex;                  //Handled data index in circle buffer

    bool m_overrided;                               //Is buffer overriding?
    bool m_initialized;                             //Is initialized and ready to process
    bool m_dataWritten;                             //Is all data was written?
    bool m_dataHandlerInitialized;                  //Is data handler initialized (file opened successfully)?
    bool m_networkInitialized;                      //Is network initialized?
    bool m_writtingTerminared;
    bool m_receivingTerminated;                     //Is receiving must be stop?

    void init();                                    //Initialize
    void createDataHandler();                       //Create and initialize slots and signals of Data Handler
    void createNetworkHandler();                    //Create and initialize slots and signals of Network Handler
    void reset();                                   //Set default state
    void changeTime();                              //Change time (caused by timer)

};

#endif // CORE_H
