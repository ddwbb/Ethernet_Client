#include "core.h"

Core::Core(QObject *parent) : QObject(parent)
{
    m_window = new Window;

}

Core::~Core()
{
    delete m_window;
    if (m_buffer)
        delete m_buffer;
    if (m_bufferArray)
        delete m_buffer;
}

void Core::operator()()
{
    m_window->show();
}

void Core::init()
{

}

void Core::changeTime()
{

}

void Core::startReceive()
{

}

void Core::stopReceiveByTimer()
{

}

void Core::stopReceiveByInterrupt()
{

}
