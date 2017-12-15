#include "computingsystemstatistics.h"

ComputingSystemStatistics::ComputingSystemStatistics()
{

}

void ComputingSystemStatistics::AnalizeSnapShots(){
    std::map<int, int> programmsStepsDictionary;
                //for (int i = 0; i < SnapShots.size(); ++i)
                for (auto it = begin(SnapShots); it != end(SnapShots); ++it)
                {
                    int programmsInSystem = it->ExecutingCount + it->BufferItemsCount;
                    if (!programmsStepsDictionary.find(programmsInSystem)->first)
                        programmsStepsDictionary.insert(programmsInSystem, 0);

                    programmsStepsDictionary[programmsInSystem]++;
                }

                for (auto kv : programmsStepsDictionary) {
                    ProgrammsCountProbability.insert(std::pair<int,double>(kv.first, CountProbability(kv.second, SnapShots.size())));

                }

}

double ComputingSystemStatistics::CountProbability(int cases, int maxCases)
{
        double prob = round(((double)cases / (double)maxCases)*10000)/10000;
        return prob <= 1.0 ? prob : 1.0;
}
