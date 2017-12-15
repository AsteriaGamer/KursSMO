#ifndef COMPUTINGSYSTEM_H
#define COMPUTINGSYSTEM_H

#include <list>
#include "serverprogramm.h"
#include "computingsystemstatistics.h"
#include <list>
#include "constants.h"

class ComputingSystem
{

public:
    ComputingSystem();
    static double GetExpProbability(double,double);
    static double GetRandomNumber(double, double);
    ComputingSystemStatistics Simulate(DistributionType);
private:
    std::list<ServerProgramm> programms;
    std::list<ServerProgramm> finishedProgramms;
    
    double timeTillNextProgrammLinear = 0;
    double popupProbabilityExp = GetExpProbability(Constants::ProgrammPopupTime::ExpLambda, Constants::SimulationStep);
    double elapsedExp = 0;

};

#endif // COMPUTINGSYSTEM_H
