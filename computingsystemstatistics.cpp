#include "computingsystemstatistics.h"

ComputingSystemStatistics::ComputingSystemStatistics()
{

}

void ComputingSystemStatistics::AnalizeSnapShots(){
    std::map<int,int> programmsStepsDictionary;

    for(int i = 0; i < SnapShots.size(); ++i){
        int programmsInSystem = SnapShots[i].ExecutingCount + SnapShots[i].BufferItemsCount;
        if(!programmsStepsDictionary(programmsInSystem))
    }
}
