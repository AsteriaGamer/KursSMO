#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>

class Constants
{
public:
    Constants(int, int, int, double, double, double, double, double, double);
    int ServersCount;
    int BufferSize;
    int SimulationTime;
    const double SimulationStep = 0.001;
    double LinearPopMinTime;
    double LinearPopMaxTime;
    double ExpLambda;
    double LinearExecMinTime;
    double LinearExecMaxTime;
    double ExpAvgExecutionTime;
};

#endif // CONSTANTS_H
