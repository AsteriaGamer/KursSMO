#ifndef COMPUTINGSYSTEM_H
#define COMPUTINGSYSTEM_H

#include <list>
#include "serverprogramm.h"
#include "computingsystemstatistics.h"
#include <list>

class ComputingSystem
{

public:
    ComputingSystem();
    static double GetExpProbability(double,double);
    static double GetRandomNumber(double, double);
    ComputingSystemStatistics Simulate(DistributionType);
signals:

public slots:

private:
    int bufferItems = 0;
    std::list<ServerProgramm> programms;
    std::list<ServerProgramm> finishedProgramms;
};

#endif // COMPUTINGSYSTEM_H
