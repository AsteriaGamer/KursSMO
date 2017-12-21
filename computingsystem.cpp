#include "computingsystem.h"
#include "serverprogramm.h"
#include "constants.h"
#include <cmath>
#include <ctime>
#include <QList>
#include <QMutableListIterator>

ComputingSystem::ComputingSystem()
{

}

ComputingSystemStatistics ComputingSystem::Simulate(DistributionType distributionType, Constants ConstObj){


    ComputingSystemStatistics statistic;
    QList<ServerProgramm> programms;
    QList<ServerProgramm> finishedProgramms;

    double timeTillNextProgrammLinear = 0;
    auto popupProbabilityExp = GetExpProbability(ConstObj.ExpLambda, ConstObj.SimulationStep);

    for (double i = 0; i <= ConstObj.SimulationTime; i += ConstObj.SimulationStep){

        QMutableListIterator<ServerProgramm> TimeIter(programms);
            while(TimeIter.hasNext()){
                ServerProgramm val = TimeIter.next();
                if(val.Status == AwaitingExecution){
                    val.ExecutionAwaitingTime += ConstObj.SimulationStep;
                    TimeIter.setValue(val);
                }else if(val.Status == Executing){
                    if(val.isExponential){
                        val.ExecutionTime += ConstObj.SimulationStep;
                        TimeIter.setValue(val);
                        double probabilityOfBeingExecuted = ComputingSystem::GetExpProbability(ConstObj.ExpAvgExecutionTime, ConstObj.SimulationStep);
                        if(probabilityOfBeingExecuted > ComputingSystem::GetRandomNumber(0,1))
                            val.Status = Executed;
                        TimeIter.setValue(val);
                    }else{
                        val.ExecutionTimeLeft -= ConstObj.SimulationStep;
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

        if(distributionType == Liniar){
            if(timeTillNextProgrammLinear <= 0){
                ServerProgramm serverProgramm(Liniar, GetRandomNumber(ConstObj.LinearExecMinTime, ConstObj.LinearExecMaxTime));
				programms.push_back(serverProgramm);
                timeTillNextProgrammLinear = GetRandomNumber(ConstObj.LinearPopMinTime, ConstObj.LinearPopMaxTime);
                statistic.TotalProgrammsAdded++;
            }
            timeTillNextProgrammLinear -= ConstObj.SimulationStep;
        }else if(distributionType == Exponential){
            if(popupProbabilityExp > GetRandomNumber(0,1)){
                ServerProgramm serverProgramm(Exponential, GetRandomNumber(ConstObj.LinearExecMinTime, ConstObj.LinearExecMaxTime));
				programms.push_back(serverProgramm);
                statistic.TotalProgrammsAdded++;
            }
        }


        // Перевод программ из ожидания  в выполнение.
        int freeServersCount = ConstObj.ServersCount;
        int j = 0; // Счётчик для цикла

        QMutableListIterator<ServerProgramm> FreeIter(programms);
        while(FreeIter.hasNext()){
            ServerProgramm val = FreeIter.next();
            if(val.Status == Executing)
                freeServersCount--;
        }

        QMutableListIterator<ServerProgramm> ServerIter(programms);
        while(ServerIter.hasNext()){
            ServerProgramm val = ServerIter.next();
            if(j < freeServersCount){
                if(val.Status == AwaitingExecution){
                    val.Status = Executing;
                    ServerIter.setValue(val);
                    j++;
                }
            }
        }

        // Переполняющие буфер программы считаются не выполненными
        int overflowCount = -(ConstObj.BufferSize);
        int m = 0; //Счётчик для цикла

        QMutableListIterator<ServerProgramm> BuffIter(programms);
        while(BuffIter.hasNext()){
            ServerProgramm val = BuffIter.next();
            if(val.Status == AwaitingExecution)
                overflowCount++;
        }

        if (overflowCount > 0){
            QMutableListIterator<ServerProgramm> BufferIter(programms);

            while(BufferIter.hasNext()){
                ServerProgramm val = BufferIter.next();
                if(m < overflowCount){
                    if(val.Status == AwaitingExecution){
                        val.Status = Discarded;
                        BufferIter.setValue(val);
                        m++;
                    }
                }
            }
         }



        //сбор статистики
        SnapShot SnapObj;

        for(auto it : programms){
            if(it.Status == AwaitingExecution)
                SnapObj.BufferItemsCount++;
            if(it.Status == Executing)
                SnapObj.ExecutingCount++;
        }

        statistic.SnapShots.push_back(SnapObj);

        //удаляем из листа программ не обработанные и выполненные программы для оптимизации скорости поиска по листу в следующих итерациях

        QMutableListIterator<ServerProgramm> DeleteProg(programms);

        while(DeleteProg.hasNext()){
            ServerProgramm val = DeleteProg.next();
            if(val.Status == Executed){
                finishedProgramms.push_back(val);
                statistic.ExecutedProgrammsCount++;
                DeleteProg.remove();
            }
            if(val.Status == Discarded){
                finishedProgramms.push_back(val);
                statistic.DiscardedProgrammsCount++;
                DeleteProg.remove();
            }
        }
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
