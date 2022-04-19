#pragma once

#include <cstdio>
#include <cmath>
#include <chrono>
#include <sys/time.h>
#include "veronica_type.h"
#include "veronica_asm.h"

using namespace std;

namespace veronica
{
    const uint64 MAX_NUM_TIMER = 16;
          uint64 CPU_FREQ      = 0;

    // generic timer
    struct time_pair
    {
        #if defined ISA_X86_64
            chrono::time_point<chrono::steady_clock> start;
            chrono::time_point<chrono::steady_clock> end;
        #else
            struct timeval start;
            struct timeval end;
        #endif
    }time_pair_array[MAX_NUM_TIMER];

    void check_timer_index(const int index)
    {
        if(index >= MAX_NUM_TIMER)
        {
            cout << "[error] timer index is larger than MAX_NUM_TIMER " << MAX_NUM_TIMER << endl;
            exit(-1);
        }
    }

    void set_timer_start(const int index)
    {
        check_timer_index(index);
        
        #if defined ISA_X86_64
            MFENCE;
            time_pair_array[index].start = std::chrono::steady_clock::now();
        #else
            #if defined ISA_ARM64
                asm volatile("dmb ish": : :"memory");
            #endif
            gettimeofday(&(time_pair_array[index].start), NULL);
        #endif
    }

    void set_timer_end(const int index)
    {
        check_timer_index(index);
        #if defined ISA_X86_64
            MFENCE;
            time_pair_array[index].end = std::chrono::steady_clock::now();
        #else
            #if defined ISA_ARM64
                asm volatile("dmb ish": : :"memory");
            #endif
            gettimeofday(&(time_pair_array[index].end), NULL);
        #endif
    }

    double nsec_to_cycle(const double nano_secs)
    {
        if(CPU_FREQ == 0)
        {
            CPU_FREQ = get_cpu_freq();
            if(CPU_FREQ == 0)
            {
                cout << "[error] getting a 0 for cpu freq" << endl;
                exit(-1);
            }
            else
            {
                //cout << "[info] cpu freq is " << cpu_freq << endl;
            }
        }
        
        double cycles = nano_secs * CPU_FREQ / pow(10.0, 9.0);
        return cycles;
    }

    double get_elapsed_time_in_cycle(const int index)
    {
        check_timer_index(index);
        #if defined ISA_X86_64
            double nano_secs = chrono::duration_cast<chrono::nanoseconds>(time_pair_array[index].end - 
                                                                          time_pair_array[index].start).count();
            return nsec_to_cycle(nano_secs);
        #endif
    }

    double get_elapsed_time_in_usec(const int index)
    {
        #if defined ISA_X86_64
            return chrono::duration_cast<chrono::microseconds>(time_pair_array[index].end - 
                                                               time_pair_array[index].start).count();
        #else
            timeval* tv       = &(time_pair_array[index].start);
            double start_time = tv->tv_sec * pow(10.0,6.0) + tv->tv_usec;
                     tv       = &(time_pair_array[index].end);
            double end_time   = tv->tv_sec * pow(10.0,6.0) + tv->tv_usec;
            return end_time - start_time;
        #endif
    }

    void print_timer(const int index, const char* name)
    {
        double elapsed_time = get_elapsed_time_in_usec(index);

        if(elapsed_time >= pow(10.0,6.0))
        {
            int64 sec  = floor(elapsed_time / pow(10.0,6.0));
            int64 msec = floor((elapsed_time - sec * pow(10.0,6.0)) / pow(10.0,3.0));
            cout << "[result] timer " << name << " = " << sec << " s " << msec << " ms" << endl;
        }
        else if(elapsed_time >= pow(10.0,3.0))
        {
            int64 msec = floor(elapsed_time / pow(10.0,3.0));
            int64 usec = floor((elapsed_time - msec * pow(10.0,3.0)));
            cout << "[result] timer " << name << " = " << msec << " ms " << usec << " us" << endl;
        }
        else
        {
            cout << "[result] timer " << name << " = " << elapsed_time * 1000 << " ns " << endl;
        }
    }
}