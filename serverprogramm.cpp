#include "serverprogramm.h"
#include "computingsystem.h"
#include "constants.h"

ServerProgramm::~ServerProgramm(){

}


ServerProgramm::ServerProgramm(DistributionType distrType, double time){
    if(distrType == Liniar){
        ExecutionTime = ExecutionTimeLeft = time;
        isExponential = false;
    }else{
        ExecutionTime = 0;
        expAverage = time;
        isExponential = true;
    }
}
