#ifndef CONSTANTS_H
#define CONSTANTS_H


class Constants
{
public:
    Constants();
    const static int ServersCount = 2;
    const static int BufferSize = 3;
    const static int SimulationTime = 60*60;
    static constexpr double SimulationStep = 0.001;

    class ProgrammPopupTime{
    public:
        static constexpr double LinearMinTime = 1.0/2.0;
        static constexpr double LinearMaxTime = 5.0/6.0;

        static constexpr double ExpLambda = 1;
    };

    class ProgrammExecutionTime{
    public:
        static constexpr double LinearMinTime = 1;
        static constexpr double LinearMaxTime = 5;

        static constexpr double ExpAvgExecutionTime = 0.5;
    };

};

#endif // CONSTANTS_H
