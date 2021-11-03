#pragma once

#include <cstdio>
#include <cmath>
#include <sys/time.h>
#include "veronica_type.h"

namespace veronica
{
    const uint64 MAX_NUM_TIMER = 16;

    // for x86 only, cycle-precision
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

    // for x86 only, cycle-precision
    static time_pair  time_pair_array[MAX_NUM_TIMER];
    // generic timer
    static cycle_pair cycle_pair_array[MAX_NUM_TIMER];

#ifdef X86_64
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
            printf("[Error] timer index is larger than MAX_NUM_TIMER %llu\n", MAX_NUM_TIMER);
            exit(-1);
        }
    }

    double time_spec_to_us(const timeval* tv)
    {
        return tv->tv_sec * pow(10.0,9.0) + tv->tv_usec;
    }

    double cycle_count_to_ns(const uint64 cycle)
    {
        uint64 cpu_freq = get_cpu_freq();
        if(cpu_freq == 0)
        {
            printf("[Error] getting a 0 for cpu freq\n");
            exit(-1);
        }
        double ns = pow(10.0,9.0) * cycle / cpu_freq;
        return ns;
    }

    void set_timer_start(const int index)
    {
        check_timer_index(index);
        #ifdef X86_64
            cycle_pair_array[index].start = x86_rdtsc();
        #else
            gettimeofday(&(time_pair_array[index].start), NULL);
        #endif
    }

    void set_timer_end(const int index)
    {
        check_timer_index(index);
        #ifdef X86_64
            cycle_pair_array[index].end = x86_rdtsc();
        #else
            gettimeofday(&(time_pair_array[index].end), NULL);
        #endif
    }

    double get_elapsed_time_in_us(const int index)
    {
        #ifdef X86_64
            return (cycle_count_to_ns(cycle_pair_array[index].end - cycle_pair_array[index].start)) / 1000;
        #else
            return time_spec_to_us(&(time_pair_array[index].end)) - time_spec_to_us((&time_pair_array[index].start));
        #endif
    }

    void print_timer(const int index, const char* name)
    {
        double elapsed_time = get_elapsed_time_in_us(index);

        if(elapsed_time >= pow(10.0,6.0))
        {
            double sec  = floor(elapsed_time / pow(10.0,6.0));
            double msec = floor((elapsed_time - sec * pow(10.0,6.0)) / pow(10.0,3.0));
            printf("[Time] timer %s = %.0f s %.0f ms\n", name, sec, msec);
        }
        else if(elapsed_time >= pow(10.0,3.0))
        {
            double msec = floor(elapsed_time / pow(10.0,3.0));
            double usec = floor((elapsed_time - msec * pow(10.0,3.0)));
            printf("[Time] timer %s = %.0f ms %.0f us\n", name, msec, usec);
        }
        else
        {
            printf("[Time] timer %s = %.0f ns\n", name, elapsed_time * 1000);
        }
    }
}
