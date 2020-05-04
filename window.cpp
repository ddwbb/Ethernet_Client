#include "window.h"
#include "ui_window.h"

Window::Window(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Window)
{
    ui->setupUi(this);

    ui->networkInitState->setState(IndicatorState::Processing);
}

Window::~Window()
{
    delete ui;
}


void Window::on_startButton_clicked()
{

}

void Window::on_stopButton_clicked()
{

}
