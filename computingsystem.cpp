#include "computingsystem.h"
#include "serverprogramm.h"
#include "constants.h"
#include <cmath>
#include <algorithm>
#include <ctime>
#include <QDebug>
#include "cpplinq.hpp"
#include <QList>
#include <QMutableListIterator>

using namespace cpplinq;

ComputingSystem::ComputingSystem()
{

}

ComputingSystemStatistics ComputingSystem::Simulate(DistributionType distributionType){


    ComputingSystemStatistics statistic;
    QList<ServerProgramm> programms;
    QList<ServerProgramm> finishedProgramms;

    double timeTillNextProgrammLinear = 0;
    auto popupProbabilityExp = GetExpProbability(Constants::ProgrammPopupTime::ExpLambda, Constants::SimulationStep);

    for (double i = 0; i <= Constants::SimulationTime; i += Constants::SimulationStep){

        QMutableListIterator<ServerProgramm> TimeIter(programms);
            while(TimeIter.hasNext()){
                ServerProgramm val = TimeIter.next();
                if(val.Status == AwaitingExecution)
                    val.ExecutionAwaitingTime += Constants::SimulationStep;
                else if(val.Status == Executing){
                    if(val.isExponential){
                        val.ExecutionTime += Constants::SimulationStep;

                        double probabilityOfBeingExecuted = ComputingSystem::GetExpProbability(Constants::ProgrammExecutionTime::ExpAvgExecutionTime, Constants::SimulationStep);

                        if(probabilityOfBeingExecuted > ComputingSystem::GetRandomNumber(0,1))
                            val.Status = Executed;
                        TimeIter.setValue(val);
                    }else{
                        val.ExecutionTimeLeft -= Constants::SimulationStep;
                        if(val.ExecutionTimeLeft <= 0)
                            val.Status = Executed;
                        TimeIter.setValue(val);
                    }
                if(val.Status == AwaitingExecution){
                    val.Status = Executing;
                    TimeIter.setValue(val);
                }
            }

           }



//        for(auto it: programms){
//            it.Update(Constants::SimulationStep);
//        }

        if(distributionType == Liniar){
            if(timeTillNextProgrammLinear <= 0){
				ServerProgramm serverProgramm(Liniar, GetRandomNumber(Constants::ProgrammExecutionTime::LinearMinTime, Constants::ProgrammExecutionTime::LinearMaxTime));
				programms.push_back(serverProgramm);
                timeTillNextProgrammLinear = GetRandomNumber(Constants::ProgrammPopupTime::LinearMinTime, Constants::ProgrammPopupTime::LinearMaxTime);
                statistic.TotalProgrammsAdded++;
            }
            timeTillNextProgrammLinear -= Constants::SimulationStep;
        }else if(distributionType == Exponential){
            if(popupProbabilityExp > GetRandomNumber(0,1)){
				ServerProgramm serverProgramm(Exponential, GetRandomNumber(Constants::ProgrammExecutionTime::LinearMinTime, Constants::ProgrammExecutionTime::LinearMaxTime));
				programms.push_back(serverProgramm);
                statistic.TotalProgrammsAdded++;
            }
        }


        // Перевод программ из ожидания  в выполнение.
        int countFreeServ = from(programms)
                >> where([](ServerProgramm s) {return s.Status == Executing;})
                >> count();

        int freeServersCount = Constants::ServersCount - countFreeServ;
        int j = 0;

        QMutableListIterator<ServerProgramm> ServerIter(programms);
            while(ServerIter.hasNext() && j < freeServersCount){
                ServerProgramm val = ServerIter.next();
                if(val.Status == AwaitingExecution){
                    val.Status = Executing;
                    ServerIter.setValue(val);
                }
                j++;
            }





        // Переполняющие буфер программы считаются не выполненными
        int countOverFlow = from(programms)
                >> where([](ServerProgramm s) {return s.Status == AwaitingExecution;})
                >> count();

        int overflowCount = countOverFlow - Constants::BufferSize;
        int m = 0;

        if (overflowCount > 0){
            QMutableListIterator<ServerProgramm> BufferIter(programms);
            while(BufferIter.hasNext() && m < overflowCount){
                 ServerProgramm val = BufferIter.next();
                 if(val.Status == AwaitingExecution){
                    val.Status = Discarded;
                 BufferIter.setValue(val);
                 }
                 m++;
            }


            }



        //сбор статистики
        SnapShot SnapObj;

        SnapObj.BufferItemsCount = from(programms)
                >> where([](ServerProgramm s) {return s.Status == AwaitingExecution;})
                >> count();
        SnapObj.ExecutingCount = from(programms)
                >> where([](ServerProgramm s) {return s.Status == Executing;})
                >> count();

        statistic.SnapShots.push_back(SnapObj);

        //удаляем из листа программ не обработанные и выполненные программы для оптимизации скорости поиска по листу в следующих итерациях
//        auto hist = from(programms)
//                >> where([](ServerProgramm s) {return s.Status == Discarded || s.Status == Executed;})
//                >> to_list();

        QMutableListIterator<ServerProgramm> DeleteProg(programms);

        while(DeleteProg.hasNext()){
            ServerProgramm val = DeleteProg.next();
            if(val.Status == Executed){
                finishedProgramms.push_back(val);
                statistic.ExecutedProgrammsCount++;
                qDebug() << "Executed " << statistic.ExecutedProgrammsCount;
                DeleteProg.remove();
            }
            if(val.Status == Discarded){
                finishedProgramms.push_back(val);
                statistic.DiscardedProgrammsCount++;
                qDebug() << "Discarded " << statistic.DiscardedProgrammsCount;
                DeleteProg.remove();
            }
        }

//        for (auto programm : hist){
//            if (programm.Status == Executed)
//                finishedProgramms.push_back(programm);
//                statistic.ExecutedProgrammsCount++;
//                programms.erase(std::remove(programms.begin(), programms.end(), programm), programms.end());
//            if (programm.Status == Discarded)
//                finishedProgramms.push_back(programm);
//                statistic.DiscardedProgrammsCount++;
//                programms.erase(std::remove(programms.begin(), programms.end(), programm), programms.end());

//            finishedProgramms.push_back(programm);
//            programms.remove(programm);
//        }
//        for(auto it : programms){
//            qDebug() << "Status: "<< it.Status;
//        }
 }

    finishedProgramms.append(programms);
    statistic.programms = finishedProgramms;

    return statistic;
}


double ComputingSystem::GetExpProbability(double lambda, double time){
    return lambda < 0 ? 0 :
                        1 - (exp(-lambda*time));
}

double ComputingSystem::GetRandomNumber(double min, double max){
    return (double)(rand())/RAND_MAX*(max - min) + min;
}
