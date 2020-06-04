#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include <QFile>
#include <QThread>
#include <QObject>
#include <QTextStream>

#include "config.h"

class DataHandler : public QObject
{
    Q_OBJECT
public:
    explicit DataHandler(DataHandlerParams, QObject *parent = nullptr);
    ~DataHandler();

public slots:
    void write();
    void prepareFile();

signals:
    void dataWritten();

private:
    QFile m_file;
    DataHandlerParams m_params;

    bool m_overrided;
    bool m_receiveTerminated;
    unsigned short m_receiveIndex;
    unsigned short m_writtenIndex;

    void getParams();
};

#endif // DATAHANDLER_H
