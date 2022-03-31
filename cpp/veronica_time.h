#pragma once

#include <cstdio>
#include <cmath>
#include <sys/time.h>
#include "veronica_type.h"

namespace veronica
{
    const uint64 MAX_NUM_TIMER = 16;
    
    // cycle-level precision timer
    struct cycle_pair
    {
        uint64 start;
        uint64 end;
    };

    // generic timer
    struct time_pair
    {
        struct timeval start;
        struct timeval end;
    };

    // for cycle-level precision
    static time_pair  time_pair_array[MAX_NUM_TIMER];
    // generic timer
    static cycle_pair cycle_pair_array[MAX_NUM_TIMER];

#if defined(ISA_X86_64)
    inline uint64_t x86_rdtsc() 
    { 
        uint32_t lo, hi; 
        asm volatile("mfence"); 
        asm volatile("rdtsc" : "=a" (lo), "=d" (hi)); 
        return (uint64_t)hi << 32 | lo; 
    }
#endif

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
        #if defined(ISA_X86_64)
            cycle_pair_array[index].start = x86_rdtsc();
        #else
            #if defined(ISA_ARM64)
                asm volatile("dmb ish": : :"memory");
            #endif
            gettimeofday(&(time_pair_array[index].start), NULL);
        #endif
    }

    void set_timer_end(const int index)
    {
        check_timer_index(index);
        #if defined(ISA_X86_64)
            cycle_pair_array[index].end = x86_rdtsc();
        #else
            #if defined(ISA_ARM64)
                asm volatile("dmb ish": : :"memory");
            #endif
            gettimeofday(&(time_pair_array[index].end), NULL);
        #endif
    }

    double cycle_count_to_nsec(const uint64 cycle)
    {
        uint64 cpu_freq = get_cpu_freq();
        if(cpu_freq == 0)
        {
            cout << "[error] getting a 0 for cpu freq" << endl;
            exit(-1);
        }
        double ns = pow(10.0, 9.0) * cycle / cpu_freq;
        return ns;
    }

    double get_elapsed_time_in_cycle(const int index)
    {
        return (cycle_pair_array[index].end - cycle_pair_array[index].start);
    }

    double get_elapsed_time_in_usec(const int index)
    {
        #if defined(ISA_X86_64)
            return (cycle_count_to_nsec(cycle_pair_array[index].end - cycle_pair_array[index].start)) / 1000;
        #else
            timeval* tv       = &(veronica::time_pair_array[index].start);
            double start_time = tv->tv_sec * pow(10.0,6.0) + tv->tv_usec;
                     tv       = &(veronica::time_pair_array[index].end);
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
