#pragma once

#include <iostream>
#include <cmath>
#include <chrono>
#include <sys/time.h>
#include "veronica_type.h"
#include "veronica_asm.h"

using namespace std;

namespace veronica
{
    uint64 CPU_FREQ = 0;

    class timer
    {
        private:
            
            #if defined ISA == X64
                chrono::time_point<chrono::steady_clock> tick;
            #elif defined ISA == AARCH64
                chrono::time_point<chrono::steady_clock> tick;
            #else
                struct timeval tick;
            #endif
        
            double nsec_to_cycle(const double &nano_secs)
            {
                return nano_secs / pow(10.0, 9.0) * CPU_FREQ;
            }
        
        public:
            timer()
            {
                #if defined ISA_X64
                    MFENCE;
                    tick = std::chrono::steady_clock::now();
                #elif defined ISA_AARCH64
                    MFENCE;
                    tick = std::chrono::steady_clock::now();
                #else
                    gettimeofday(&tick, NULL);
                #endif

                if(CPU_FREQ == 0)
                {
                    CPU_FREQ = get_cpu_freq();
                    if(CPU_FREQ == 0)
                    {
                        cout << "[ERROR-Veronica] timer cpu freq is 0" << endl;
                        exit(-1);
                    }
                    else
                    {
                        cout << "[WARNING-Veronica] timer set for cpu freq " << CPU_FREQ / 1000 / 1000 << " MHz" << endl;
                    }
                }
            }

            friend double get_elapsed_time_in_cycle(timer& begin, timer& end);
            friend double get_elapsed_time_in_nsec(timer& begin, timer& end);
            friend double get_elapsed_time_in_usec(timer& begin, timer& end);
            friend double get_elapsed_time_in_msec(timer& begin, timer& end);
            friend double get_elapsed_time_in_sec(timer& begin, timer& end);
    };

    // depends on get_elapsed_time_in_nsec() for non-x64 architecture
    // due to the inaccuracy of get_cpu_freq() and DVFS, the cycles reading could be inaccurate
    double get_elapsed_time_in_cycle(timer& begin, timer& end)
    {
        return end.nsec_to_cycle(get_elapsed_time_in_nsec(begin, end));
    }

    double get_elapsed_time_in_nsec(timer& begin, timer& end)
    {
        #if defined ISA_X64
            return chrono::duration_cast<chrono::nanoseconds>(end.tick - begin.tick).count();
        #elif defined ISA_AARCH64
            return chrono::duration_cast<chrono::nanoseconds>(end.tick - begin.tick).count();
        #else
            return get_elapsed_time_in_usec(begin, end) * 1000;
        #endif
    }

    // depends on get_elapsed_time_in_nsec() for non-x64 architecture
    double get_elapsed_time_in_usec(timer& begin, timer& end)
    {
        #if defined ISA_X64
            return get_elapsed_time_in_nsec(begin, end) / pow(10.0, 3.0);
        #elif defined ISA_AARCH64
            return get_elapsed_time_in_nsec(begin, end) / pow(10.0, 3.0);
        #else
            double begin_time = begin.tick.tv_sec * pow(10.0,6.0) + begin.tick.tv_usec;
            double end_time   = end.tick.tv_sec   * pow(10.0,6.0) + end.tick.tv_usec;
            return end_time - begin_time;
        #endif
    }

    // depends on get_elapsed_time_in_usec() for non-x64 architecture
    double get_elapsed_time_in_msec(timer& begin, timer& end)
    {
        return get_elapsed_time_in_usec(begin, end) / pow(10.0, 3.0);
    }

    // depends on get_elapsed_time_in_usec() for non-x64 architecture
    double get_elapsed_time_in_sec(timer& begin, timer& end)
    {
        return get_elapsed_time_in_usec(begin, end) / pow(10.0, 6.0);
    }
}