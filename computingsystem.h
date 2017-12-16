#ifndef COMPUTINGSYSTEM_H
#define COMPUTINGSYSTEM_H

#include <list>
#include "serverprogramm.h"
#include "computingsystemstatistics.h"
#include "constants.h"

class ComputingSystem
{

public:
    ComputingSystem();
    static double GetExpProbability(double,double);
    static double GetRandomNumber(double, double);
    ComputingSystemStatistics Simulate(DistributionType);
private:
    
    double timeTillNextProgrammLinear = 0;
    double popupProbabilityExp = GetExpProbability(Constants::ProgrammPopupTime::ExpLambda, Constants::SimulationStep);

};

#endif // COMPUTINGSYSTEM_H
