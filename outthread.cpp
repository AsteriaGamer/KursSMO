#include "outthread.h"
#include "computingsystem.h"
#include "computingsystemstatistics.h"
#include "constants.h"
#include <QString>
#include <map>
#include "mainwindow.h"

OutThread::OutThread(int DistTypeA, QString tempRunsA, QString SCA, QString BSA, QString STA, QString LPMinTA,
                     QString LPMaxTA, QString ELA, QString LEMinTA, QString LEMaxTA, QString EAETA)
{
    DistType = DistTypeA;
    tempRuns = tempRunsA.toInt();
    SC = SCA;
    BS = BSA;
    ST = STA;
    LPMinT = LPMinTA;
    LPMaxT = LPMaxTA;
    EL = ELA;
    LEMinT = LEMinTA;
    LEMaxT = LEMaxTA;
    EAET = EAETA;
}

void OutThread::doWork(){
    //int runsNum = tempRuns.toInt();

    std::map<int, double> tempDict;
    auto tempExecutedPercent = 0.0;
    auto tempProgrammsExecutedPerSecond = 0.0;
    auto tempDiscardProbability = 0.0;
    auto tempAvgServerBusy = 0.0;
    auto tempAvgProgrammsInSystem = 0.0;
    auto tempAverageTimeInSystem = 0.0;
    auto tempAvgProgrammsInBuffer = 0.0;
    auto tempAverageTimeInBuffer = 0.0;

    Constants ConstObj(SC,BS,ST,
                       LPMinT,LPMaxT,EL,
                       LEMinT,LEMaxT,EAET);

    DistributionType type = Exponential;

    auto test = DistType;
    if (test == 0){
        type = Exponential;
    }else if (test == 1)
        type = Liniar;

    for(int i = 0; i < tempRuns; i++){

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
            emit send(QString::number(it.second/tempRuns)," - вероятность того, что ВС не загружена");
        }
        else if (it.first < SC.toInt())
        {
            QString test = " - вероятность того, что загружено: ";
            test += QString::number(it.first);
            test += "/";
            test += SC;
            test += " серверов. Буфер свободен.";
            emit send(QString::number(it.second/tempRuns), test);
        }
        else if (it.first == SC.toInt())
        {
            QString test2 = " - вероятность того, что загружено:  ";
            test2 += SC;
            test2 += "/";
            test2 += SC;
            test2 += " серверов. Буфер свободен.";
            emit send(QString::number(it.second/tempRuns),test2);
        }
        else if (it.first > SC.toInt())
        {
            int inBufferCount = it.first - SC.toInt();
            QString test3 = " - вероятность того, что в буфере ";
            test3 += QString::number(inBufferCount);
            test3 += " программа(ы)";
            emit send(QString::number(it.second/tempRuns),test3);
        }
    }
    emit send(QString::number(tempExecutedPercent/tempRuns, 2.0, 2), " - Q (относит. пропускная способность- процент программ, обработанных ВС) ");
    emit send(QString::number(tempProgrammsExecutedPerSecond/tempRuns, 2.0, 2), " - S (среднее число программ, обработанных в секунду)");
    emit send(QString::number(tempDiscardProbability/tempRuns, 2.0, 2), " - Pоткл (вероятность отказа, т.е. того, что программа будет не обработанной)");
    emit send(QString::number(tempAvgServerBusy/tempRuns), " - K (среднее число занятых серверов)");
    emit send(QString::number(tempAvgProgrammsInSystem/tempRuns, 2.0, 2), " - Nпрог (среднее число программ в ВС)");
    emit send(QString::number(tempAverageTimeInSystem/tempRuns, 2.0, 2), " сек - Tпрог (среднее время нахождения программы в ВС)");
    emit send(QString::number(tempAvgProgrammsInBuffer/tempRuns, 2.0, 2), " - Nбуф (среднее число программ в буфере)");
    emit send(QString::number(tempAverageTimeInBuffer/tempRuns, 2.0, 2), " сек - Tбуф (среднее время нахождения программы в буфере)");
    emit send("","");
}

double OutThread::countPercents(double total, double value)
{
    return (double)value / (total / 100.0);
}
