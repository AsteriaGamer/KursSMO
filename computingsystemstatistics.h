#ifndef COMPUTINGSYSTEMSTATISTICS_H
#define COMPUTINGSYSTEMSTATISTICS_H

#include <list>
#include "serverprogramm.h"
#include <map>
#include <cmath>
#include <QList>

class SnapShot
    {
public:
        int ExecutingCount = 0;
        int BufferItemsCount = 0;
    };

class ComputingSystemStatistics
{
public:
    ComputingSystemStatistics();
    QList<SnapShot> SnapShots;
    QList<ServerProgramm> programms;
    std::map<int, double> ProgrammsCountProbability;

    int ExecutedProgrammsCount = 0;
    int DiscardedProgrammsCount = 0;
    int TotalProgrammsAdded = 0;

    void AnalizeSnapShots();
    double CountProbability(int, int);

};

#endif // COMPUTINGSYSTEMSTATISTICS_H
