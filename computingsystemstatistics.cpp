#include "computingsystemstatistics.h"

ComputingSystemStatistics::ComputingSystemStatistics()
{

}

void ComputingSystemStatistics::AnalizeSnapShots(){
    std::map<int, int> programmsStepsDictionary;
                for (int i = 0; i < SnapShots.size(); ++i)
                {
                    int programmsInSystem = SnapShots[i].ExecutingCount + SnapShots[i].BufferItemsCount;
                    if (!programmsStepsDictionary.find(programmsInSystem)->first)
                        programmsStepsDictionary.insert(programmsInSystem, 0);

                    programmsStepsDictionary[programmsInSystem]++;
                }

                for (const auto& kv : programmsStepsDictionary) {
                    ProgrammsCountProbability.insert(kv.first, CountProbability(kv.second, SnapShots.size()));

                }

}

static double ComputingSystemStatistics::CountProbability(int cases, int maxCases)
{
        double prob = round(((double)cases / (double)maxCases)*10000)/10000;
        return prob <= 1 ? prob : 1;
}
