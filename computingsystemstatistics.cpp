#include "computingsystemstatistics.h"

ComputingSystemStatistics::ComputingSystemStatistics()
{

}

void ComputingSystemStatistics::AnalizeSnapShots(){
    std::map<int, int> programmsStepsDictionary;
                for (auto it: SnapShots)
                {
                    int programmsInSystem = (it.ExecutingCount + it.BufferItemsCount);
                    if (programmsStepsDictionary.count(programmsInSystem) == 0)
                        programmsStepsDictionary[programmsInSystem] = 0;

                    programmsStepsDictionary[programmsInSystem]++;
                }

                for (auto kv : programmsStepsDictionary) {
                    ProgrammsCountProbability[kv.first] = CountProbability(kv.second, SnapShots.count());

                }

}

double ComputingSystemStatistics::CountProbability(int cases, int maxCases)
{
        double prob = round(((double)cases / (double)maxCases)*10000)/10000;
        return prob <= 1.0 ? prob : 1.0;
}
