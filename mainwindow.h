#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QString>
#include <QFileDialog>
#include <QMainWindow>
#include <QRegExpValidator>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum ErrorType {
    socket = 1,
    file,
    receiving,
    writing
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

    Ui::MainWindow *ui;

    bool working;

    QString timeStr;
    QString fileName;

    QRegExpValidator timeValidator;

    QFileDialog * explorer;

    void validateTime();
    void validateTime(int);


public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    int getTime();
    void setTime(int);

    QString getFileName();

    void setErrorState(int);
    void setWorkingState();
    void setDefaultState();


public slots:
    void selectFile(const QString &);

signals:
    void started();
    void stoped();

private slots:
    void on_startButton_clicked();
    void on_stopButton_clicked();
    void on_fileButton_clicked();
    void on_timeEdit_textChanged(const QString &arg1);
};
#endif // MAINWINDOW_H
