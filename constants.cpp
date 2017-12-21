#include "constants.h"

Constants::Constants(QString SC, QString BS, QString ST, QString LPMinT, QString LPMaxT, QString EL, QString LEMinT, QString LEMaxT, QString EAET){
    ServersCount = SC.toInt();
    BufferSize = BS.toInt();
    SimulationTime = ST.toInt();
    LinearPopMinTime = LPMinT.toDouble();
    LinearPopMaxTime = LPMaxT.toDouble();
    ExpLambda = EL.toDouble();
    LinearExecMinTime = LEMinT.toDouble();
    LinearExecMaxTime = LEMaxT.toDouble();
    ExpAvgExecutionTime = EAET.toDouble();
}
