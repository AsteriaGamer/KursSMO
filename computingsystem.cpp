#include "computingsystem.h"
#include "serverprogramm.h"
#include "constants.h"
#include <cmath>
#include <algorithm>
#include <ctime>
#include <QDebug>

ComputingSystem::ComputingSystem()
{

}

ComputingSystemStatistics ComputingSystem::Simulate(DistributionType distributionType = Liniar){


    ComputingSystemStatistics statistic;
	std::list<ServerProgramm> programms;
	std::list<ServerProgramm> finishedProgramms;

    double timeTillNextProgrammLinear = 0;
    auto popupProbabilityExp = GetExpProbability(Constants::ProgrammPopupTime::ExpLambda, Constants::SimulationStep);

    for (double i = 0; i <= Constants::SimulationTime; i += Constants::SimulationStep){

		for (auto it = begin(programms); it != end(programms); ++it) {
			it->Update(Constants::SimulationStep);
		}

        if(distributionType == Liniar){
            if(timeTillNextProgrammLinear <= 0){
                //ServerProgramm *serverProgramm = new ServerProgramm(Liniar, GetRandomNumber(Constants::ProgrammExecutionTime::LinearMinTime, Constants::ProgrammExecutionTime::LinearMaxTime));
                //programms.push_back(*serverProgramm);
				ServerProgramm serverProgramm(Liniar, GetRandomNumber(Constants::ProgrammExecutionTime::LinearMinTime, Constants::ProgrammExecutionTime::LinearMaxTime));
				programms.push_back(serverProgramm);
                timeTillNextProgrammLinear = GetRandomNumber(Constants::ProgrammPopupTime::LinearMinTime, Constants::ProgrammPopupTime::LinearMaxTime);
                statistic.TotalProgrammsAdded++;
            }
            timeTillNextProgrammLinear -= Constants::SimulationStep;
        }else if(distributionType == Exponential){
            if(popupProbabilityExp > GetRandomNumber(0,1)){
                //ServerProgramm *serverProgramm = new ServerProgramm(Exponential, GetRandomNumber(Constants::ProgrammExecutionTime::LinearMinTime, Constants::ProgrammExecutionTime::LinearMaxTime));
                //programms.push_back(*serverProgramm);
				ServerProgramm serverProgramm(Exponential, GetRandomNumber(Constants::ProgrammExecutionTime::LinearMinTime, Constants::ProgrammExecutionTime::LinearMaxTime));
				programms.push_back(serverProgramm);
                statistic.TotalProgrammsAdded++;
            }
        }


        // Перевод программ из ожидания в удаление.
        int countOfAwaiting = 0;

        for (auto it = begin(programms); it != end(programms); ++it) {
            if(it->Status == Executing)
                countOfAwaiting++;
        }

        int freeServersCount = Constants::ServersCount - countOfAwaiting;


		auto it = programms.begin();
        for(int i = 0; i <= freeServersCount; i++){
             std::advance(it, i);
             if(it->Status == AwaitingExecution)
                 it->Status = Executing;
        }

        // Переполняющие буфер программы считаются не выполненными
        int countOverFlow = 0;

        for (auto it = begin(programms); it != end(programms); ++it) {
            if(it->Status == AwaitingExecution)
                countOverFlow++;
        }

        int overflowCount = countOverFlow - Constants::BufferSize;
        if (overflowCount > 0){
			auto it = programms.begin();
            for(int i = 0; i <= countOverFlow; i++){
                 std::advance(it, i);
                 if(it->Status == AwaitingExecution)
                     it->Status = Discarded;
            }
        }


        //сбор статистики
        SnapShot SnapObj;

        for(auto it: programms){
            if (it.Status == AwaitingExecution) {
                    SnapObj.BufferItemsCount++;
                } else if(it.Status == Executing){
                    SnapObj.ExecutingCount++;
                }
        }

        statistic.SnapShots.push_back(SnapObj);

        //удаляем из листа программ не обработанные и выполненные программы для оптимизации скорости поиска по листу в следующих итерациях
        for(auto it: programms){
            if (it.Status == Executed) {
                    statistic.ExecutedProgrammsCount++;
                    programms.remove(it);
                    finishedProgramms.push_back(it);
                } else if(it.Status == Discarded){
                    statistic.DiscardedProgrammsCount++;
                    programms.remove(it);
                    finishedProgramms.push_back(it);
                }
        }

    //std::copy(programms.begin(), programms.end(), std::back_insert_iterator<std::list<ServerProgramm> >(finishedProgramms));
	finishedProgramms.insert(finishedProgramms.end(), programms.begin(), programms.end());
	statistic.programms.insert(programms.end(), finishedProgramms.begin(), finishedProgramms.end());
	//std::copy(finishedProgramms.begin(), finishedProgramms.end(), std::back_insert_iterator<std::list<ServerProgramm> >(statistic.programms));
    //statistic->programms = finishedProgramms;
    return statistic;
 }
}


double ComputingSystem::GetExpProbability(double lambda, double time){
    return lambda < 0 ? 0 :
                        1 - (exp(-lambda*time));
}

double ComputingSystem::GetRandomNumber(double min, double max){
    return (double)(rand())/RAND_MAX*(max - min) + min;
}
