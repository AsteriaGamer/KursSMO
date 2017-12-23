#include "outthread.h"
#include "computingsystem.h"
#include "computingsystemstatistics.h"
#include "constants.h"
#include <QString>
#include <map>
#include "mainwindow.h"
#include <QTime>

OutThread::OutThread(int DistTypeA, QString tempRunsA, QString SCA, QString BSA, QString STA, QString LPMinTA,
                     QString LPMaxTA, QString ELA, QString LEMinTA, QString LEMaxTA, QString EAETA)
{
    DistType = DistTypeA;
    tempRuns = tempRunsA.toInt();
    SC = SCA.toInt();
    BS = BSA.toInt();
    ST = STA.toInt();
    LPMinT = LPMinTA.toDouble();
    LPMaxT = LPMaxTA.toDouble();
    EL = ELA.toDouble();
    LEMinT = LEMinTA.toDouble();
    LEMaxT = LEMaxTA.toDouble();
    EAET = EAETA.toDouble();
}

void OutThread::doWork(){
    std::map<int, double> tempDict;
    auto tempExecutedPercent = 0.0;
    auto tempProgrammsExecutedPerSecond = 0.0;
    auto tempDiscardProbability = 0.0;
    auto tempAvgServerBusy = 0.0;
    auto tempAvgProgrammsInSystem = 0.0;
    auto tempAverageTimeInSystem = 0.0;
    auto tempAvgProgrammsInBuffer = 0.0;
    auto tempAverageTimeInBuffer = 0.0;

    int barStatus = 0;

    QTime currTime = QTime::currentTime();

    Constants ConstObj(SC,BS,ST,
                       LPMinT,LPMaxT,EL,
                       LEMinT,LEMaxT,EAET);

    DistributionType type = Exponential;
    QString typeDistName = "";

    auto test = DistType;
    if (test == 0){
        type = Exponential;
        typeDistName = "Экспоненциальное";
    }else if (test == 1){
        typeDistName = "Линейное";
        type = Liniar;
    }

    for(int i = 0; i < tempRuns; i++){

        barStatus += 25/tempRuns;
        emit sendBarStatus(barStatus);

        ComputingSystem cs;

        auto stats = cs.Simulate(type, ConstObj);

        stats.AnalizeSnapShots();

        barStatus += 25/tempRuns;
        emit sendBarStatus(barStatus);

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
        barStatus += 25/tempRuns;
        emit sendBarStatus(barStatus);
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

        barStatus += 25/tempRuns;
        emit sendBarStatus(barStatus);

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

        barStatus += 25/tempRuns;
        emit sendBarStatus(barStatus);
    }

    emit send("Время начала обработки задачи : ", currTime.toString("hh:mm:ss"));
    emit send("Тип распределения: ", typeDistName);

    for(auto it: tempDict)
    {
        if (it.first == 0)
        {
            emit send(QString::number(it.second/tempRuns)," - вероятность того, что ВС не загружена");
        }
        else if (it.first < SC)
        {
            QString test = " - вероятность того, что загружено: ";
            test += QString::number(it.first);
            test += "/";
            test += QString::number(SC);
            test += " серверов. Буфер свободен.";
            emit send(QString::number(it.second/tempRuns), test);
        }
        else if (it.first == SC)
        {
            QString test2 = " - вероятность того, что загружено:  ";
            test2 += QString::number(SC);
            test2 += "/";
            test2 += QString::number(SC);;
            test2 += " серверов. Буфер свободен.";
            emit send(QString::number(it.second/tempRuns),test2);
        }
        else if (it.first > SC)
        {
            int inBufferCount = it.first - SC;
            QString test3 = " - вероятность того, что в буфере ";
            test3 += QString::number(inBufferCount);
            test3 += " программа(ы)";
            emit send(QString::number(it.second/tempRuns),test3);
        }
    }
    emit send(QString::number(tempExecutedPercent/tempRuns, 2.0, 2), " - Q (относит. пропускная способность- процент программ, обработанных ВС) ");
    emit send(QString::number(tempProgrammsExecutedPerSecond/tempRuns, 2.0, 2), " - S (среднее число программ, обработанных в секунду)");
    emit send(QString::number(tempDiscardProbability/tempRuns, 2.0, 2), " - Pотк (вероятность отказа, т.е. того, что программа будет не обработанной)");
    emit send(QString::number(tempAvgServerBusy/tempRuns), " - K (среднее число занятых серверов)");
    emit send(QString::number(tempAvgProgrammsInSystem/tempRuns, 2.0, 2), " - Nпрог (среднее число программ в ВС)");
    emit send(QString::number(tempAverageTimeInSystem/tempRuns, 2.0, 2), " сек - Tпрог (среднее время нахождения программы в ВС)");
    emit send(QString::number(tempAvgProgrammsInBuffer/tempRuns, 2.0, 2), " - Nбуф (среднее число программ в буфере)");
    emit send(QString::number(tempAverageTimeInBuffer/tempRuns, 2.0, 2), " сек - Tбуф (среднее время нахождения программы в буфере)");
    currTime = QTime::currentTime();
    emit send("Время завершения обработки задачи : ", currTime.toString("hh:mm:ss"));
    emit send("-------------------------------------------------------------","------------------------------------------");
    emit send("","");

    emit finished();
}

double OutThread::countPercents(double total, double value)
{
    return (double)value / (total / 100.0);
}
