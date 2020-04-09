#ifndef CORE_H
#define CORE_H

#include <QObject>

#include "window.h"

class Core : public QObject
{
    Q_OBJECT

    Window * window;
public:
    explicit Core(QObject *parent = nullptr);

    void operator()();

signals:

};

#endif // CORE_H
