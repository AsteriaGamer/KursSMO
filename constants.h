#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>

class Constants
{
public:
    Constants(QString, QString, QString, QString, QString, QString, QString, QString, QString);
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
