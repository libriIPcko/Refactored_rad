#include "time_measure.h"

time_measure::time_measure()
{

}



void   time_measure::wallTime_start(){
    wallTime_begin = std::chrono::high_resolution_clock::now();
}

//@default in seconds
double time_measure::wallTime_out(){
    wallTime_end = std::chrono::high_resolution_clock::now();
    wallTime_elapsed = std::chrono::duration<double, std::ratio<1,1>>(wallTime_end - wallTime_begin).count();
    wallTime_unit = "[s]";
    return wallTime_elapsed;
}



double time_measure::wallTime_out(int type){
    if(type == 0){
        wallTime_end = std::chrono::high_resolution_clock::now();
        wallTime_elapsed = std::chrono::duration<double, std::ratio<1,1>>(wallTime_end - wallTime_begin).count();
        wallTime_unit = "[s]";
        return wallTime_elapsed;
    }
    else if(type == 1){
        wallTime_end = std::chrono::high_resolution_clock::now();
        wallTime_elapsed = std::chrono::duration<double, std::milli>(wallTime_end - wallTime_begin).count();
        wallTime_unit = "[ms]";
        return wallTime_elapsed;
    }
    else if(type == 2){
        wallTime_end = std::chrono::high_resolution_clock::now();
        wallTime_elapsed = std::chrono::duration<double, std::micro>(wallTime_end - wallTime_begin).count();
        wallTime_unit = "[us]";
        return wallTime_elapsed;
    }
    else if(type == 3){
        wallTime_end = std::chrono::high_resolution_clock::now();
        wallTime_elapsed = std::chrono::duration<double, std::nano>(wallTime_end - wallTime_begin).count();
        wallTime_unit = "[ns]";
        return wallTime_elapsed;
    }
    else if(type ==4){
        wallTime_end = std::chrono::high_resolution_clock::now();
        wallTime_elapsed = std::chrono::duration<double, std::pico>(wallTime_end - wallTime_begin).count();
        wallTime_unit = "[ps]";
        return wallTime_elapsed;
    }
    else{
        //error statement
        return 99;
    }

}



void   time_measure::cpuTime_start(){
    //processthreadsapi, getProcessTime for windows
    //time, clock for Linux
    FILETIME a,b,c,d;
    if (GetProcessTimes(GetCurrentProcess(),&a,&b,&c,&d) != 0){
        //  Returns total user time.
        //  Can be tweaked to include kernel times as well.
        cpuTime_start_var = (double)(d.dwLowDateTime | ((unsigned long long)d.dwHighDateTime << 32)) * 0.0000001;
    }else{
        //  Handle error
        cpuTime_start_var = 0;
    }
}

double time_measure::cpuTime_out(){
    FILETIME a,b,c,d;
    if (GetProcessTimes(GetCurrentProcess(),&a,&b,&c,&d) != 0){
        //  Returns total user time.
        //  Can be tweaked to include kernel times as well.
        cpuTime_end_var = (double)(d.dwLowDateTime | ((unsigned long long)d.dwHighDateTime << 32)) * 0.0000001;
        cpuTime_elapsedTime = cpuTime_end_var - cpuTime_start_var;
        return cpuTime_elapsedTime;
    }else{
        //  Handle error
        return  99;
    }
}

double time_measure::getElapsedTime(){
    return cpuTime_elapsedTime;
}
