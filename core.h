#ifndef CORE_H
#define CORE_H

#include <vector>

#include <QTimer>
#include <QObject>

#include "window.h"

class Core : public QObject
{
public:
    explicit Core(QObject *parent = nullptr);
    ~Core();

    void operator()();
signals:

private:
    Q_OBJECT
    Window * m_window;

    //Receiver
    //Handler

    std::vector<char[1440]> * m_buffer;
    char * m_bufferArray;

    QTimer m_timer;

    void init();
    void changeTime();
    void startReceive();
    void stopReceiveByTimer();
    void stopReceiveByInterrupt();


};

#endif // CORE_H
