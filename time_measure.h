#ifndef TIME_MEASURE_H
#define TIME_MEASURE_H
#include <processthreadsapi.h>
#include <chrono>
#include <QString>

class time_measure
{
public:
    time_measure();

    void   wallTime_start();
    double wallTime_out();
    //@ 0 == seconds | 1 == milliseconds | 2 == microseconds
    //@ 3 == nanoseconds
    double wallTime_out(int type);
    QString wallTime_unit;

    void   cpuTime_start();
    double cpuTime_out();
    double getElapsedTime();
    QString cpuTime_unit;


private:
    std::chrono::high_resolution_clock::time_point wallTime_begin;
    std::chrono::high_resolution_clock::time_point wallTime_end;
    double wallTime_elapsed;

    double cpuTime_start_var;
    double cpuTime_end_var;
    double cpuTime_elapsedTime;
};

#endif // TIME_MEASURE_H
