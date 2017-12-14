#ifndef CONSTANTS_H
#define CONSTANTS_H


class Constants
{
public:
    Constants();
    const int ServersCount = 2;
    const int BufferSize = 3;
    const int SimulationTime = 60*60;
    const double SimulationStep = 0.001;

    class ProgrammPopupTime{
    public:
        const double LinearMinTime = 1.0/2.0;
        const double LinearMaxTime = 5.0/6.0;

        const double ExpLambda = 1;
    };

    class ProgrammExecutionTime{
    public:
        const double LinearMinTime = 1;
        const double LinearMaxTime = 5;

        const double ExpAvgExecutionTime = 0.5;
    };

};

#endif // CONSTANTS_H
