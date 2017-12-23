#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "computingsystem.h"
#include "computingsystemstatistics.h"
#include "constants.h"
#include <QString>
#include <map>
#include "outthread.h"
#include <QThread>
#include <QFile>
#include <QTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	connect(ui->StartButton, SIGNAL(clicked()), this, SLOT(RunAndShow()));
    connect(ui->SaveFile, SIGNAL(clicked()), this, SLOT(saveFile()));
    connect(ui->clearButton, SIGNAL(clicked()), this, SLOT(clearEdit()));
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
    connect(my, SIGNAL(finished()), thread, SLOT(terminate()));
    connect(my, SIGNAL(sendBarStatus(int)), this, SLOT(barMove(int)));

    thread->start();
}

void MainWindow::UpdateS(QString value, QString text){
    ui->OutEdit->append(value + text);
}

void MainWindow::saveFile(){
        QString fileName = "Отчёт_"+QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss") + ".txt";
        QFile fileOut(fileName);
        if(fileOut.open(QIODevice::WriteOnly))
        {
            fileOut.write(QString(ui->OutEdit->toPlainText()).toUtf8());
            fileOut.close();
        }
}

void MainWindow::clearEdit(){
    ui->OutEdit->clear();
}

void MainWindow::barMove(int barStatus){
    ui->progressBar->setValue(barStatus);
}
