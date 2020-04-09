#include "core.h"

Core::Core(QObject *parent) : QObject(parent)
{
    window = new Window;
}

void Core::operator()()
{
    window->show();
}
