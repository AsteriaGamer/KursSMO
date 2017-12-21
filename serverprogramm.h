#ifndef SERVERPROGRAMM_H
#define SERVERPROGRAMM_H

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
    ~ServerProgramm();
    ProgrammStatus Status = AwaitingExecution;
    double ExecutionTimeLeft;
    double ExecutionTime;
    double ExecutionAwaitingTime = 0;
    inline bool operator==(const ServerProgramm &rhs) const {
        if(rhs.Status == AwaitingExecution)
            return true;
        else if(rhs.Status == Executing)
            return true;
        else if(rhs.Status == Executed)
            return true;
        else if(rhs.Status == Discarded)
            return true;
        else
            return true;
    }
    bool isExponential;
    double expAverage;
};

#endif // SERVERPROGRAMM_H
