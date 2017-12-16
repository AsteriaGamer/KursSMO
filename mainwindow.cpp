#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "computingsystem.h"
#include "computingsystemstatistics.h"
#include "constants.h"

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
	int runsNum = 1;

	ComputingSystem *cs = new ComputingSystem();

	auto stats = cs->Simulate(Liniar);

	stats.AnalizeSnapShots();

	for(auto it: stats.ProgrammsCountProbability)
	{
		if (it.first == 0)
		{
			ui->OutEdit->append(QString::number(it.second, 4.0) + " - вероятность того, что ВС не загружена");
		}
		else if (it.first < Constants::ServersCount)
		{
			ui->OutEdit->append(QString::number(it.second, 4.0) + " - вероятность того, что загружено: " + (it.first/Constants::ServersCount) + " серверов");
		}
		else if (it.first == Constants::ServersCount)
		{
			double probabilityAllServersBusy = 0;
			for(auto pair : stats.ProgrammsCountProbability)
			{
				if (pair.first >= it.first)
					probabilityAllServersBusy += pair.second;
			}

			ui->OutEdit->append(QString::number(probabilityAllServersBusy, 4.0) + " - вероятность того, что загружено:  " + (Constants::ServersCount/Constants::ServersCount) + " серверов");
		}
		else if (it.first > Constants::ServersCount)
		{
			int inBufferCount = it.first - Constants::ServersCount;

			ui->OutEdit->append(QString::number(it.second, 4.0) + " - вероятность того, что в буфере " + inBufferCount + " программа(ы)");
		}
	}

	double executedPercentage = countPercents(stats.TotalProgrammsAdded, stats.ExecutedProgrammsCount);
	ui->OutEdit->append(QString::number(executedPercentage, 4.0) + " - Q (относит. пропускная способность- процент программ, обработанных ВС) ");
	double programmsExecutedPerSecond = round((double)stats.ExecutedProgrammsCount / (double)Constants::SimulationTime);
	ui->OutEdit->append(QString::number(programmsExecutedPerSecond, 4.0) + " - S (среднее число программ, обработанных в секунду)");
	double discardProbability = stats.CountProbability(stats.DiscardedProgrammsCount, stats.TotalProgrammsAdded);
	ui->OutEdit->append(QString::number(discardProbability, 4.0) + " - Pоткл (вероятность отказа, т.е. того, что программа будет не обработанной)");

	int totalServersBusy = 0;
	int totalProgrammsInSystem = 0;
	int totalProgrammsInBuffer = 0;
	for(auto snapshot: stats.SnapShots)
	{
		totalProgrammsInSystem += snapshot.ExecutingCount + snapshot.BufferItemsCount;
		totalProgrammsInBuffer += snapshot.BufferItemsCount;
		int serversBusy = snapshot.ExecutingCount;
		if (serversBusy > Constants::ServersCount)
			serversBusy = Constants::ServersCount;
		totalServersBusy += serversBusy;
	}

	double avgServersBusy = round((double)totalServersBusy / (double)stats.SnapShots.size());
	ui->OutEdit->append(QString::number(avgServersBusy, 4.0) + " - K (среднее число занятых серверов)");

	double avgProgrammsInSystem = round((double)totalProgrammsInSystem / (double)stats.SnapShots.size());
	ui->OutEdit->append(QString::number(avgProgrammsInSystem, 4.0) + " - K (среднее число программ в ВС)");

	double spentTimeTotal = 0.0;
	double spentTimeInBuffer = 0.0;
	for(auto programm: stats.programms){
		spentTimeTotal += programm.ExecutionAwaitingTime + programm.ExecutionTime;
		spentTimeInBuffer += programm.ExecutionAwaitingTime;
	}
	double averageTimeInSystem = round(spentTimeTotal / (double)stats.programms.size());
	ui->OutEdit->append(QString::number(averageTimeInSystem, 2.0) + " сек - Tпрог (среднее время нахождения программы в ВС)");

	double avgProgrammsInBuffer = round((double)totalProgrammsInBuffer / (double)stats.SnapShots.size());
	ui->OutEdit->append(QString::number(avgProgrammsInBuffer, 2.0) + " - Nбуф (среднее число программ в буфере)");

	double averageTimeInBuffer = round(spentTimeInBuffer / (double)stats.programms.size());
	ui->OutEdit->append(QString::number(averageTimeInBuffer, 2.0) + " сек - Tбуф (среднее время нахождения программы в буфере) /n");

}

double MainWindow::countPercents(double total, double value)
{
	return (double)round(value / (total / 100.0));
}