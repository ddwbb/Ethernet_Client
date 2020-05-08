#ifndef WINDOW_H
#define WINDOW_H

#include <QString>
#include <QRegExp>
#include <QValidator>
#include <QFileDialog>
#include <QMainWindow>

#include "indicator.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Window; }
QT_END_NAMESPACE

enum WindowState {
    BaseState,
    WokringState,
    StoppingState
};

enum Indicators {
    File,
    Network,
    Writing,
    Receiving
};

class Window : public QMainWindow
{
    Q_OBJECT

public:
    Window(QWidget *parent = nullptr);
    ~Window();

    void setState(WindowState);

    int getTime();
    QString getFileName();

    void setTime(int);

    void validateTime();

public slots:
    void setIndicatorState(Indicators, IndicatorState);

signals:
    void stoped();
    void started();

private slots:
    void selectFile(const QString);

    void on_startButton_clicked();

    void on_stopButton_clicked();

    void on_selectFileButton_clicked();

    void on_timeEdit_textChanged(const QString &arg1);

private:
    Ui::Window * ui;

    QString m_timeStr;

    QRegExp m_timeRegExp;
    QRegExpValidator m_timeValidator;

    QFileDialog explorer;
};
#endif // WINDOW_H
