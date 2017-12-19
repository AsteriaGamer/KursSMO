#include "serverprogramm.h"
#include "computingsystem.h"
#include "constants.h"

ServerProgramm::~ServerProgramm(){

}


ServerProgramm::ServerProgramm(DistributionType distrType, double time){
    if(distrType == Liniar){
        ExecutionTime = ExecutionTimeLeft = time;
        isExponential = false;
    }else{
        ExecutionTime = 0;
        expAverage = time;
        isExponential = true;
    }
}

void ServerProgramm::Update(double timePassed){
    if(Status == AwaitingExecution)
        ExecutionAwaitingTime += timePassed;
    else if(Status == Executing){
        if(isExponential){
            ExecutionTime += timePassed;

            double probabilityOfBeingExecuted = ComputingSystem::GetExpProbability(Constants::ProgrammExecutionTime::ExpAvgExecutionTime, Constants::SimulationStep);

            if(probabilityOfBeingExecuted > ComputingSystem::GetRandomNumber(0,1))
                Status = Executed;
        }else{
            ExecutionTimeLeft -= timePassed;
            if(ExecutionTimeLeft <= 0)
                Status = Executed;
        }
    }
}
