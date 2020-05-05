#include "window.h"
#include "ui_window.h"

#include <QDebug>

Window::Window(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Window)
{
    ui->setupUi(this);

    ui->networkInitState->setState(IndicatorState::Processing);

    m_timeRegExp.setPattern("^[0-9]{2}:[0-5][0-9]:[0-5][0-9]$");
    m_timeValidator.setRegExp(m_timeRegExp);
    ui->timeEdit->setValidator(&m_timeValidator);

    ui->selectFileEdit->setText(QDir::currentPath() + "/output.txt");

    connect(&explorer, &QFileDialog::fileSelected, this, &Window::selectFile);
}

Window::~Window()
{
    delete ui;
}

void Window::setState(WindowState state)
{
    switch (state) {
        case WindowState::BaseState:
            ui->timeEdit->setReadOnly(false);
            ui->startButton->setEnabled(true);
            ui->stopButton->setEnabled(false);
            break;
        case WindowState::WokringState:
            ui->timeEdit->setReadOnly(true);
            ui->startButton->setEnabled(false);
            ui->stopButton->setEnabled(true);
            break;
        case WindowState::StoppingState:
            ui->timeEdit->setReadOnly(true);
            ui->startButton->setEnabled(false);
            ui->stopButton->setEnabled(false);
            break;
    }
}

void Window::setIndicatorState(Indicators indicator, IndicatorState state)
{
    switch (indicator) {
        case Indicators::File:
            ui->fileInitState->setState(state);
            break;
        case Indicators::Network:
            ui->networkInitState->setState(state);
            break;
        case Indicators::Writing:
            ui->fileWritingState->setState(state);
            break;
        case Indicators::Receiving:
            ui->networkReceivingState->setState(state);
            break;
    }
}

int Window::getTime()
{
    validateTime();
    int timeInSec = 0;
    QStringList time = m_timeStr.split(":");
    timeInSec = time[0].toInt() * 3600 + time[1].toInt() * 60 + time[2].toInt();
    return timeInSec;
}

QString Window::getFileName()
{
    return ui->selectFileEdit->text();
}

void Window::setTime(int time)
{
    if (time < 0) time = 0;
    int hours = time / 3600;
    time %= 3600;
    int minutes = time / 60;
    time %= 60;
    int seconds = time;

    QString tmp = (hours > 9) ? QString::number(hours) : "0" + QString::number(hours);
    m_timeStr = tmp + ":";
    tmp = (minutes > 9) ? QString::number(minutes) : "0" + QString::number(minutes);
    m_timeStr += tmp + ":";
    tmp = (seconds > 9) ? QString::number(seconds) : "0" + QString::number(seconds);
    m_timeStr += tmp;

    ui->timeEdit->setText(m_timeStr);
}

void Window::validateTime()
{
    int pos = 0;
    if (m_timeValidator.validate(m_timeStr, pos) != QValidator::Acceptable) {
        for (int i = m_timeStr.length(); i < 8; i++) {
            if (i == 2 || i == 5)
                m_timeStr += ":";
            else
                m_timeStr += "0";
        }
        ui->timeEdit->setText(m_timeStr);
    }
}

void Window::selectFile(const QString name)
{
    ui->selectFileEdit->setText(name);
}


void Window::on_startButton_clicked()
{
    emit started();
}

void Window::on_stopButton_clicked()
{
    emit stoped();
}

void Window::on_selectFileButton_clicked()
{
    explorer.show();
}

void Window::on_timeEdit_textChanged(const QString &arg1)
{
    int oldLength = m_timeStr.length();
    int newLength = arg1.length();

    m_timeStr = arg1;
    if (oldLength < newLength && (newLength == 2 || newLength == 5))
            m_timeStr += ":";
        ui->timeEdit->setText(m_timeStr);
}
