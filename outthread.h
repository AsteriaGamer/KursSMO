#ifndef OUTTHREAD_H
#define OUTTHREAD_H

#include <QObject>
#include <QString>

class OutThread:public QObject
{
Q_OBJECT
public:
    OutThread(int, QString, QString, QString, QString, QString,
              QString, QString, QString, QString, QString);
    double countPercents(double, double);
    int DistType = 0;
    int tempRuns;
    int SC;
    int BS;
    int ST;
    double LPMinT;
    double LPMaxT;
    double EL;
    double LEMinT;
    double LEMaxT;
    double EAET;
public slots:
    void doWork();
signals:
    void send(QString, QString);
    void finished();
    void sendBarStatus(int);
};

#endif // OUTTHREAD_H
