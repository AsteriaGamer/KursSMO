#ifndef COMPUTINGSYSTEM_H
#define COMPUTINGSYSTEM_H

#include "serverprogramm.h"
#include "computingsystemstatistics.h"
#include "constants.h"

class ComputingSystem
{

public:
    ComputingSystem();
    static double GetExpProbability(double,double);
    static double GetRandomNumber(double, double);
    ComputingSystemStatistics Simulate(DistributionType, Constants);
};

#endif // COMPUTINGSYSTEM_H
