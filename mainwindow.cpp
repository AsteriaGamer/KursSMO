#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "computingsystem.h"
#include "computingsystemstatistics.h"
#include "constants.h"
#include <QString>
#include <QDebug>
#include <map>

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
    int runsNum = ui->RunsNumEdit->text().toInt();

    std::map<int, double> tempDict;
    auto tempExecutedPercent = 0.0;
    auto tempProgrammsExecutedPerSecond = 0.0;
    auto tempDiscardProbability = 0.0;
    auto tempAvgServerBusy = 0.0;
    auto tempAvgProgrammsInSystem = 0.0;
    auto tempAverageTimeInSystem = 0.0;
    auto tempAvgProgrammsInBuffer = 0.0;
    auto tempAverageTimeInBuffer = 0.0;

//    Constants GUIConstObj(ui->ServerCountEdit->text(),ui->BufferCountEdit->text(),ui->TimeEdit->text(),
//                       ui->TimePopMinEdit->text(),ui->TimePopMaxEdit->text(),ui->LambdaEdit->text(),
//                       ui->TimeExecMinEdit->text(),ui->TimeExecMaxEdit->text(),ui->MuEdit->text());
    Constants ConstObj(ui->ServerCountEdit->text(),ui->BufferCountEdit->text(),ui->TimeEdit->text(),
                       ui->TimePopMinEdit->text(),ui->TimePopMaxEdit->text(),ui->LambdaEdit->text(),
                       ui->TimeExecMinEdit->text(),ui->TimeExecMaxEdit->text(),ui->MuEdit->text());

    for(int i = 0; i < runsNum; i++){

//        Constants ConstObj(ui->ServerCountEdit->text(),ui->BufferCountEdit->text(),ui->TimeEdit->text(),
//                           ui->TimePopMinEdit->text(),ui->TimePopMaxEdit->text(),ui->LambdaEdit->text(),
//                           ui->TimeExecMinEdit->text(),ui->TimeExecMaxEdit->text(),ui->MuEdit->text());

        DistributionType type = Exponential;

        auto test = ui->DistrTypeBox->currentIndex();
        if (test == 0){
            type = Exponential;
        }else if (test == 1)
            type = Liniar;

        ComputingSystem cs;

        auto stats = cs.Simulate(type, ConstObj);

        stats.AnalizeSnapShots();

        for(auto it: stats.ProgrammsCountProbability)
        {
            if (tempDict.count(it.first) == 0)
                tempDict[it.first] = 0;

            if (it.first == 0)
            {
                tempDict[it.first] += it.second;
            }
            else if (it.first < ConstObj.ServersCount)
            {
                tempDict[it.first] += it.second;
            }
            else if (it.first == ConstObj.ServersCount)
            {
                tempDict[it.first] += it.second;
            }
            else if (it.first > ConstObj.ServersCount)
            {
                tempDict[it.first] += it.second;
            }
        }

        double executedPercentage = countPercents(stats.TotalProgrammsAdded, stats.ExecutedProgrammsCount);
        tempExecutedPercent += executedPercentage;
        double programmsExecutedPerSecond = (double)stats.ExecutedProgrammsCount / (double)ConstObj.SimulationTime;
        tempProgrammsExecutedPerSecond += programmsExecutedPerSecond;
        double discardProbability = stats.CountProbability(stats.DiscardedProgrammsCount, stats.TotalProgrammsAdded);
        tempDiscardProbability += discardProbability;

        int totalServersBusy = 0;
        int totalProgrammsInSystem = 0;
        int totalProgrammsInBuffer = 0;
        for(auto snapshot: stats.SnapShots)
        {
            totalProgrammsInSystem += snapshot.ExecutingCount + snapshot.BufferItemsCount;
            totalProgrammsInBuffer += snapshot.BufferItemsCount;
            int serversBusy = snapshot.ExecutingCount;
            if (serversBusy > ConstObj.ServersCount)
                serversBusy = ConstObj.ServersCount;
            totalServersBusy += serversBusy;
        }

        double avgServersBusy = (double)totalServersBusy / (double)stats.SnapShots.count();
        tempAvgServerBusy += avgServersBusy;
        double avgProgrammsInSystem = (double)totalProgrammsInSystem / (double)stats.SnapShots.count();
        tempAvgProgrammsInSystem += avgProgrammsInSystem;

        double spentTimeTotal = 0.0;
        double spentTimeInBuffer = 0.0;
        for(auto programm: stats.programms){
            spentTimeTotal += programm.ExecutionAwaitingTime + programm.ExecutionTime;
            spentTimeInBuffer += programm.ExecutionAwaitingTime;
        }
        double averageTimeInSystem = spentTimeTotal / (double)stats.programms.count();
        tempAverageTimeInSystem += averageTimeInSystem;

        double avgProgrammsInBuffer = (double)totalProgrammsInBuffer / (double)stats.SnapShots.count();
        tempAvgProgrammsInBuffer += avgProgrammsInBuffer;

        double averageTimeInBuffer = spentTimeInBuffer / (double)stats.programms.count();
        tempAverageTimeInBuffer += averageTimeInBuffer;

    }

    for(auto it: tempDict)
	{
		if (it.first == 0)
		{
            ui->OutEdit->append(QString::number(it.second/runsNum) + " - вероятность того, что ВС не загружена");
        }
        else if (it.first < ui->ServerCountEdit->text().toInt())
		{
            ui->OutEdit->append(QString::number(it.second/runsNum) + " - вероятность того, что загружено: " + QString::number(it.first) + "/" + QString::number(ui->ServerCountEdit->text().toInt()) + " серверов. Буфер свободен.");
		}
        else if (it.first == ui->ServerCountEdit->text().toInt())
        {
            ui->OutEdit->append(QString::number(it.second/runsNum) + " - вероятность того, что загружено:  " + QString::number(ui->ServerCountEdit->text().toInt()) + "/" + QString::number(ui->ServerCountEdit->text().toInt()) + " серверов. Буфер свободен.");
        }
        else if (it.first > ui->ServerCountEdit->text().toInt())
        {
            int inBufferCount = it.first - ui->ServerCountEdit->text().toInt();

            ui->OutEdit->append(QString::number(it.second/runsNum) + " - вероятность того, что в буфере " + QString::number(inBufferCount) + " программа(ы)");
        }
	}
    ui->OutEdit->append(QString::number(tempExecutedPercent/runsNum, 2.0, 2) + " - Q (относит. пропускная способность- процент программ, обработанных ВС) ");
    ui->OutEdit->append(QString::number(tempProgrammsExecutedPerSecond/runsNum, 2.0, 2) + " - S (среднее число программ, обработанных в секунду)");
    ui->OutEdit->append(QString::number(tempDiscardProbability/runsNum, 2.0, 2) + " - Pоткл (вероятность отказа, т.е. того, что программа будет не обработанной)");
    ui->OutEdit->append(QString::number(tempAvgServerBusy/runsNum) + " - K (среднее число занятых серверов)");
    ui->OutEdit->append(QString::number(tempAvgProgrammsInSystem/runsNum, 2.0, 2) + " - Nпрог (среднее число программ в ВС)");
    ui->OutEdit->append(QString::number(tempAverageTimeInSystem/runsNum, 2.0, 2) + " сек - Tпрог (среднее время нахождения программы в ВС)");
    ui->OutEdit->append(QString::number(tempAvgProgrammsInBuffer/runsNum, 2.0, 2) + " - Nбуф (среднее число программ в буфере)");
    ui->OutEdit->append(QString::number(tempAverageTimeInBuffer/runsNum, 2.0, 2) + " сек - Tбуф (среднее время нахождения программы в буфере)");
    ui->OutEdit->append(" ");
}

double MainWindow::countPercents(double total, double value)
{
    return (double)value / (total / 100.0);
}
