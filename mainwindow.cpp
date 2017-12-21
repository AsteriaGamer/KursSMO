#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "computingsystem.h"
#include "computingsystemstatistics.h"
#include "constants.h"
#include <QString>
#include <map>
#include "outthread.h"
#include <QThread>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	connect(ui->StartButton, SIGNAL(clicked()), this, SLOT(RunAndShow()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::RunAndShow() {

    QThread *thread= new QThread;
    OutThread *my = new OutThread(ui->DistrTypeBox->currentIndex(),ui->RunsNumEdit->text(),ui->ServerCountEdit->text(),ui->BufferCountEdit->text(),ui->TimeEdit->text(),
                                  ui->TimePopMinEdit->text(),ui->TimePopMaxEdit->text(),ui->LambdaEdit->text(),
                                  ui->TimeExecMinEdit->text(),ui->TimeExecMaxEdit->text(),ui->MuEdit->text());

    my->moveToThread(thread);

    connect(my, SIGNAL(send(QString,QString)), this, SLOT(UpdateS(QString,QString)));
    connect(thread, SIGNAL(started()), my, SLOT(doWork()));

    thread->start();
}

void MainWindow::UpdateS(QString value, QString text){
    ui->OutEdit->append(value + text);
}
