#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QRegExp timeExp("^[0-9]{2}:[0-5][0-9]:[0-5][0-9]$");
    timeValidator.setRegExp(timeExp);
    ui->timeEdit->setValidator(&timeValidator);

    explorer = new QFileDialog(this);

    QDir dir;
    fileName = dir.currentPath();
    fileName += "/output.txt";
    ui->fileEdit->setText(fileName);

    setDefaultState();

    connect(explorer, &QFileDialog::fileSelected, this, &MainWindow::selectFile);

    setTime(60);
}

MainWindow::~MainWindow()
{
    delete explorer;
    delete ui;
}

int MainWindow::getTime()
{
    int time = 0;
    validateTime();
    QStringList strList = timeStr.split(":");
    time = strList[0].toInt() * 3600 + strList[1].toInt() * 60 + strList[2].toInt();
    return time;
}

void MainWindow::setTime(int time)
{
    validateTime(time);
    ui->timeEdit->setText(timeStr);
}

void MainWindow::validateTime()
{
    int pos = 0;
    if (timeValidator.validate(timeStr, pos) != QValidator::Acceptable) {
        for (int i = timeStr.length(); i < 8; i++) {
            if (i == 2 || i == 5)
                timeStr += ":";
            else
                timeStr += "0";
        }
        ui->timeEdit->setText(timeStr);
    }
}

void MainWindow::validateTime(int time)
{
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
}

void MainWindow::selectFile(const QString &file)
{
    fileName = file;
    ui->fileEdit->setText(fileName);
}

QString MainWindow::getFileName()
{
    return fileName;
}

void MainWindow::setErrorState(int code)
{
    working = false;

    switch (code) {
        case ErrorType::socket:
            ui->infoEdit->append("Ошибка создания сокета для получения данных по сети");
            break;
        case ErrorType::file:
            ui->infoEdit->append("Ошибка открытия файла для записи данных");
            break;
        case ErrorType::writing:
            ui->infoEdit->append("Ошибка во время записи данных в файл");
            break;
        case ErrorType::receiving:
            ui->infoEdit->append("Ошибка получения данных по сети");
            break;
    }

    //to do
}

void MainWindow::setWorkingState()
{
    working = true;
    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(true);
    ui->timeEdit->setReadOnly(true);
    ui->fileEdit->setReadOnly(true);
}

void MainWindow::setDefaultState()
{
    working = false;
    ui->startButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
    ui->timeEdit->setReadOnly(false);
    ui->fileEdit->setReadOnly(false);
}


void MainWindow::on_startButton_clicked()
{
    if (getTime())
        emit started();
}

void MainWindow::on_stopButton_clicked()
{
    emit stoped();
}

void MainWindow::on_fileButton_clicked()
{
    explorer->show();
}

void MainWindow::on_timeEdit_textChanged(const QString &arg1)
{
    int oldLength = timeStr.length();
    int newLength = arg1.length();
    timeStr = arg1;
    if (oldLength < newLength && (newLength == 2 || newLength == 5))
        timeStr += ":";
    ui->timeEdit->setText(timeStr);
}
