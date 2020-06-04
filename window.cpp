#include "window.h"
#include "ui_window.h"

Window::Window(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Window)
{
    ui->setupUi(this);

    m_timeRegExp.setPattern("^[0-9]{2}:[0-5][0-9]:[0-5][0-9]$");
    m_timeValidator.setRegExp(m_timeRegExp);
    ui->timeEdit->setValidator(&m_timeValidator);

    ui->selectFileEdit->setText(QDir::currentPath() + "/../output.txt");
    m_timeStr = ui->timeEdit->text();

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
    m_timeStr = timeToString(time);
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

void Window::printResult(int time, long long bytes, long dgramms)
{
    QString result = "Время работы: "
            + timeToString(time)
            + "\nПолучено датаграмм: "
            + QString::number(dgramms)
            + "\nВсего байт: "
            + QString::number(bytes);
    ui->textEdit->append(result);
    ui->textEdit->append("Завершение работы\n\n");
}

void Window::printError(const QString error)
{
    QString result = "Во время работы произошла фатальная ошибка:\n" + error;
    ui->textEdit->append(result);
}

void Window::selectFile(const QString name)
{
    ui->selectFileEdit->setText(name);
}


void Window::on_startButton_clicked()
{
    ui->textEdit->append("Начало работы");
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

QString Window::timeToString(int time)
{
    QString timeStr;
    if (time < 0) time = 0;
    int hours = time / 3600;
    time %= 3600;
    int minutes = time / 60;
    time %= 60;
    int seconds = time;

    QString tmp = (hours > 9) ? QString::number(hours) : "0" + QString::number(hours);
    timeStr = tmp + ":";
    tmp = (minutes > 9) ? QString::number(minutes) : "0" + QString::number(minutes);
    timeStr += tmp + ":";
    tmp = (seconds > 9) ? QString::number(seconds) : "0" + QString::number(seconds);
    timeStr += tmp;

    return timeStr;
}
