#pragma once

#include <cstdio>
#include <ctime>
#include <cmath>
#include <string>
#include "veronica_type.h"

namespace veronica
{
    const uint64 MAX_NUM_TIMER = 10;

    struct timepoint
    {
        struct timespec start;
        struct timespec end;
    }timepoints[MAX_NUM_TIMER];

    void check_timer_index(const uint64 index)
    {
        if(index >= MAX_NUM_TIMER)
        {
            printf("[error] timer index is larger than MAX_NUM_TIMER %llu\n", MAX_NUM_TIMER);
            exit(-1);
        }
    }

    double time_spec_to_ns(const timespec* tv)
    {
        return tv->tv_sec * pow(10,9) + tv->tv_nsec;
    }

    void set_timer_start(const uint64 index)
    {
        check_timer_index(index);
        clock_gettime(CLOCK_MONOTONIC, &(timepoints[index].start));
    }

    void set_timer_end(const uint64 index)
    {
        check_timer_index(index);
        clock_gettime(CLOCK_MONOTONIC, &(timepoints[index].end));
    }

    void print_timer(const uint64 index, const char* name)
    {
        double elapsed_time = time_spec_to_ns(&(timepoints[index].end)) - time_spec_to_ns((&timepoints[index].start));

        if(elapsed_time >= pow(10,9))
        {
            uint64 sec  = floor(elapsed_time / pow(10,9));
            uint64 msec = floor((elapsed_time - sec * pow(10,9)) / pow(10,6));
            printf("[time] timer %s = %llus %llums\n", name, sec, msec);
        }
        else if(elapsed_time >= pow(10,6))
        {
            uint64 msec  = floor(elapsed_time / pow(10,6));
            uint64 usec = floor((elapsed_time - msec * pow(10,6)) / pow(10,3));
            printf("[time] timer %s = %llums %lluus\n", name, msec, usec);
        }
        else if(elapsed_time >= pow(10,3))
        {
            uint64 usec  = floor(elapsed_time / pow(10,3));
            uint64 nsec = floor((elapsed_time - usec * pow(10,3)));
            printf("[time] timer %s = %lluus %lluns\n", name, usec, nsec);
        }
        else
        {
            printf("[time] timer %s = %lluns\n", name, (uint64)time_spec_to_ns(&(timepoints[index].start)));
            printf("[time] timer %s = %lluns\n", name, (uint64)time_spec_to_ns(&(timepoints[index].end)));
        }
    }
}
