#pragma once

#include <cstdio>
#include <cmath>
#include <sys/time.h>
#include "veronica_type.h"

namespace veronica
{
    const uint64 MAX_NUM_TIMER = 16;
    int arm_msr_enable = 0;
    // cycle-level precision
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

#ifdef MACRO_ISA_X86_64
    inline uint64_t x86_rdtsc() 
    { 
        uint32_t lo, hi; 
        asm volatile("mfence"); 
        asm volatile("rdtsc" : "=a" (lo), "=d" (hi)); 
        return (uint64_t)hi << 32 | lo; 
    }
#endif

#ifdef MACRO_ISA_ARM64
    inline uint64_t arm_mfcp() 
    {
        unsigned long result = 0;
        if (!arm_msr_enable)
        {
            // program the performance-counter control-register:
            asm volatile("msr pmcr_el0, %0" : : "r" (17));
            //enable all counters
            asm volatile("msr PMCNTENSET_EL0, %0" : : "r" (0x8000000f));
            //clear the overflow 
            asm volatile("msr PMOVSCLR_EL0, %0" : : "r" (0x8000000f));
            arm_msr_enable = 1;
        }
        //read the coutner value
        asm volatile("mrs %0, PMCCNTR_EL0" : "=r" (result));
        return result;
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

    void set_timer_start(const int index)
    {
        check_timer_index(index);
        #ifdef MACRO_ISA_X86_64
            cycle_pair_array[index].start = x86_rdtsc();
        #elif MACRO_ISA_ARM64
            cycle_pair_array[index].start = arm_mfcp();
        #else
            gettimeofday(&(time_pair_array[index].start), NULL);
        #endif
    }

    void set_timer_end(const int index)
    {
        check_timer_index(index);
        #ifdef MACRO_ISA_X86_64
            cycle_pair_array[index].end = x86_rdtsc();
        #elif MACRO_ISA_ARM64
            cycle_pair_array[index].end = arm_mfcp();
        #else
            gettimeofday(&(time_pair_array[index].end), NULL);
        #endif
    }

    double time_spec_to_usec(const timeval* tv)
    {
        return tv->tv_sec * pow(10.0,6.0) + tv->tv_usec;
    }

    double cycle_count_to_nsec(const uint64 cycle)
    {
        uint64 cpu_freq = get_cpu_freq();
        if(cpu_freq == 0)
        {
            printf("[Error] getting a 0 for cpu freq\n");
            exit(-1);
        }
        double ns = pow(10.0, 9.0) * cycle / cpu_freq;
        return ns;
    }

    double get_elapsed_time_in_usec(const int index)
    {
        #if defined(MACRO_ISA_X86_64) || defined(MACRO_ISA_ARM64)
            return (cycle_count_to_nsec(cycle_pair_array[index].end - cycle_pair_array[index].start)) / 1000;
        #else
            double start_time = veronica::time_spec_to_usec(&(veronica::time_pair_array[index].start));
            double end_time   = veronica::time_spec_to_usec(&(veronica::time_pair_array[index].end));
            return end_time - start_time;
        #endif
    }

    double get_elapsed_time_in_cycle(const int index)
    {
        return (cycle_pair_array[index].end - cycle_pair_array[index].start);
    }

    void print_timer(const int index, const char* name)
    {
        double elapsed_time = get_elapsed_time_in_usec(index);

        if(elapsed_time >= pow(10.0,6.0))
        {
            double sec  = floor(elapsed_time / pow(10.0,6.0));
            double msec = floor((elapsed_time - sec * pow(10.0,6.0)) / pow(10.0,3.0));
            printf("[Time] timer %s = %.0lf s %.0lf ms\n", name, sec, msec);
        }
        else if(elapsed_time >= pow(10.0,3.0))
        {
            double msec = floor(elapsed_time / pow(10.0,3.0));
            double usec = floor((elapsed_time - msec * pow(10.0,3.0)));
            printf("[Time] timer %s = %.0lf ms %.0lf us\n", name, msec, usec);
        }
        else
        {
            printf("[Time] timer %s = %.0lf ns\n", name, elapsed_time * 1000);
        }
    }
}
