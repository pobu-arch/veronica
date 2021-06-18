#pragma once

#include <cstdio>
#include <cmath>
#include <sys/time.h>
#include "veronica_type.h"

namespace veronica
{
    const uint64 MAX_NUM_TIMER = 10;

    struct time_pair
    {
        struct timeval start;
        struct timeval end;
    };
    
    static time_pair time_pair_array[MAX_NUM_TIMER];

    void check_timer_index(const int index)
    {
        if(index >= MAX_NUM_TIMER)
        {
            printf("[error] timer index is larger than MAX_NUM_TIMER %llu\n", MAX_NUM_TIMER);
            exit(-1);
        }
    }

    double time_spec_to_us(const timeval* tv)
    {
        return tv->tv_sec * pow(10.0,6.0) + tv->tv_usec;
    }

    void set_timer_start(const int index)
    {
        check_timer_index(index);
        gettimeofday(&(time_pair_array[index].start),NULL);
    }

    void set_timer_end(const int index)
    {
        check_timer_index(index);
        gettimeofday(&(time_pair_array[index].end), NULL);
    }

    void print_timer(const int index, const char* name)
    {
        double elapsed_time = time_spec_to_us(&(time_pair_array[index].end)) - time_spec_to_us((&time_pair_array[index].start));

        if(elapsed_time >= pow(10.0,6.0))
        {
            double sec  = floor(elapsed_time / pow(10.0,6.0));
            double msec = floor((elapsed_time - sec * pow(10.0,6.0)) / pow(10.0,3.0));
            printf("[time] timer %s = %.0fs %.0f ms\n", name, sec, msec);
        }
        else if(elapsed_time >= pow(10.0,3.0))
        {
            double msec = floor(elapsed_time / pow(10.0,3.0));
            double usec = floor((elapsed_time - msec * pow(10.0,3.0)));
            printf("[time] timer %s = %.0fms %.0f us\n", name, msec, usec);
        }
        else
        {
            printf("[time] timer %s = %.0fus\n", name, 
            time_spec_to_us(&(time_pair_array[index].end) - time_spec_to_us(&(time_pair_array[index].start))));
        }
    }
}
