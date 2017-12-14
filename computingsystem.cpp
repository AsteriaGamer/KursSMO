#include "computingsystem.h"
#include "serverprogramm.h"
#include "constants.h"
#include "math.h"
#include <algorithm>
#include "cpplinq.hpp"

using namespace cpplinq;

ComputingSystem::ComputingSystem()
{

}

ComputingSystem::Simulate(DistributionType distributionType = Liniar){


    ComputingSystemStatistics statistic = new ComputingSystemStatistics();

    for (double i = 0; i <= Constants::SimulationTime; i += Constants::SimulationStep){

        std::for_each(programms.begin(), programms.end(), [](ServerProgramm s) {s.Update(Constants::SimulationStep);});

        if(distributionType == Liniar){
            if(timeTillNextProgrammLinear <= 0){
                ServerProgramm serverProgramm = new ServerProgramm(Liniar, GetRandomNumber(Constants::ProgrammExecutionTime::LinearMinTime, Constants::ProgrammExecutionTime::LinearMaxTime));
                programms.push_back(serverProgramm);
                timeTillNextProgrammLinear = GetRandomNumber(Constants::ProgrammPopupTime::LinearMinTime, Constants::ProgrammPopupTime::LinearMaxTime);
                statistic::TotalProgrammsAdded++;
            }
            timeTillNextProgrammLinear -= Constants::SimulationStep;
        }else if(distributionType == Exponential){
            if(popupProbabilityExp > GetRandomNumber(0,1)){
                ServerProgramm serverProgramm = new ServerProgramm(Exponential, GetRandomNumber(Constants::ProgrammExecutionTime::LinearMinTime, Constants::ProgrammExecutionTime::LinearMaxTime));
                programms.push_back(serverProgramm);
                statistic::TotalProgrammsAdded++;
            }
        }



        auto countOfAwaiting = from(programms)
                >> where([](ServerProgramm s) {return s.Status == ServerProgramm::ProgrammStatus::AwaitingExecution;})
                >> count();
        int freeServersCount = Constants::ServersCount - countOfAwaiting;

        for (std::list<programms>::iterator it=programms.begin(); it != programms.end(); ++it){
            if(it->Status == ServerProgramm::ProgrammStatus::AwaitingExecution){
                it->Status = ServerProgramm::ProgrammStatus::Execution;
            }
        }



//        from(programms)
//                >> where([](ServerProgramm s) {return s.Status == ServerProgramm::ProgrammStatus::AwaitingExecution;})
//                >> take(freeServersCount)
//                >> to_list();

        auto countOverFlow = from(programms)
                >> where([](ServerProgramm s) {return s.Status == ServerProgramm::ProgrammStatus::AwaitingExecution;})
                >> count();
        auto overflowCount = countOverFlow - Constants::BufferSize;
        if (overflowCount > 0){
            for (std::list<programms>::iterator it=programms.begin(); it != programms.end(); ++it){
                if(*it.Status == ServerProgramm::ProgrammStatus::AwaitingExecution){
                    *it.Status = ServerProgramm::ProgrammStatus::Discarded;
                }
            }


            programms.Where(x => x.Status == ServerProgramm.ProgrammStatus.AwaitingExecution).Take(overflowCount).ToList()
            .ForEach(x => x.Status = ServerProgramm.ProgrammStatus.Discarded);
        }

        //сбор статистики
        statistic.SnapShots.push_back(new SnapShot()
        {
            BufferItemsCount = from(programms)
                                    >> where([](ServerProgramm s) {return s.Status == ServerProgramm::ProgrammStatus::AwaitingExecution;})
                                    >> count();
            ExecutingCount =from(programms)
                                    >> where([](ServerProgramm s) {return s.Status == ServerProgramm::ProgrammStatus::Execution;})
                                    >> count();
        });

        //удаляем из листа программ не обработанные и выполненные программы для оптимизации скорости поиска по листу в следующих итерациях           
        for (std::list<programms>::iterator it=programms.begin(); it != programms.end(); ++it){
            if (it->Status == ServerProgramm.ProgrammStatus.Executed) {
                    statistic.ExecutedProgrammsCount++;
                    it = programms.erase(it);
                    finishedProgramms.push_back(it);
                } else if(it->Status == ServerProgramm.ProgrammStatus.Discarded){
                    statistic.DiscardedProgrammsCount++;
                    it = programms.erase(it);
                    finishedProgramms.push_back(it);
                }
        }

    }


    finishedProgramms.AddRange(programms);
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
