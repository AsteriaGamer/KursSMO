#ifndef SERVERPROGRAMM_H
#define SERVERPROGRAMM_H

#include <QObject>

enum DistributionType
{
    Liniar, Exponential
};

enum ProgrammStatus
{
    AwaitingExecution, Executing, Executed, Discarded
};

class ServerProgramm
{
public:
    ServerProgramm(DistributionType, double);
    ProgrammStatus Status = AwaitingExecution;
    double ExecutionTimeLeft;
    double ExecutionTime;
    double ExecutionAwaitingTime = 0;
    void Update(double);
private:
    bool isExponential;
    double expAverage;
signals:

public slots:
};

#endif // SERVERPROGRAMM_H
