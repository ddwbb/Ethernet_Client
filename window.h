#ifndef WINDOW_H
#define WINDOW_H

#include <QString>
#include <QRegExp>
#include <QValidator>
#include <QMainWindow>

#include "indicator.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Window; }
QT_END_NAMESPACE

class Window : public QMainWindow
{
    Q_OBJECT

public:
    Window(QWidget *parent = nullptr);
    ~Window();

private slots:
    void on_startButton_clicked();

    void on_stopButton_clicked();

private:
    Ui::Window * ui;

    int m_timeInSec;

    QString m_timeStr;
    QString m_filename;




};
#endif // WINDOW_H
