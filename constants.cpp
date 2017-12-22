#include "constants.h"

Constants::Constants(int SC, int BS, int ST, double LPMinT, double LPMaxT, double EL, double LEMinT, double LEMaxT, double EAET){
    ServersCount = SC;
    BufferSize = BS;
    SimulationTime = ST;
    LinearPopMinTime = LPMinT;
    LinearPopMaxTime = LPMaxT;
    ExpLambda = EL;
    LinearExecMinTime = LEMinT;
    LinearExecMaxTime = LEMaxT;
    ExpAvgExecutionTime = EAET;
}
