#pragma once

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "veronica_type.h"

namespace veronica
{
    const char* get_os_type()
    {
        #if defined __APPLE__
            return "MACOSX";
        #elif defined __linux__
            return "LINUX";
        #else
            #error "NOT SUPPORTED OS"
        #endif
    }

    uint64 get_cpu_freq()
    {
        uint64 freq = 0;
        FILE *fstream = NULL;
        char readbuf[64];
        memset(readbuf, 0, sizeof(readbuf));
        
        #if defined __APPLE__
            if(NULL == (fstream = popen("sysctl hw.cpufrequency", "r")))
        #elif defined __linux__
            if(NULL == (fstream = popen("cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq", "r")))
        #else
            #error NOT SUPPORTED OS
        #endif
        {
            cout << "[error] execute command failed: " << strerror(errno) << endl;
            exit(-1);
        }

        if(NULL != fgets(readbuf, sizeof(readbuf), fstream))
        {
            #if defined __APPLE__
                char* start = readbuf+17;
            #elif defined __linux__
                char* start = readbuf;
            #else
                #error NOT SUPPORTED OS
            #endif

            #if defined __APPLE__ || defined __linux__
                char* end   = start + strlen(start);
                freq        = strtoull(start, &end, 10);
                pclose(fstream);
                #if defined __linux__
                    freq *= 1000; // linux is counting freq in KHz
                #endif
            #endif
        }
        // it could be returning 0
        return freq;
    }

    uint64 get_page_size()
    {
        return (uint64)getpagesize();
    }

    uint64 get_page_mask()
    {
        uint64 page_size = get_page_size();
        return (~(page_size-1));
    }

    uint64 get_cache_line_size()
    {
        uint64 cache_line_size = 0;
        FILE *fstream = NULL;
        char readbuf[64];
        memset(readbuf, 0, sizeof(readbuf));

        #if defined __APPLE__
            if(NULL == (fstream = popen("sysctl hw.cachelinesize", "r")))
        #elif defined __linux__
            if(NULL == (fstream = popen("getconf LEVEL1_DCACHE_LINESIZE", "r")))
        #else
            #error NOT SUPPORTED OS
        #endif
        {
            cout << "[error] execute command failed: " << strerror(errno) << endl;
            exit(-1);
        }

        if(NULL != fgets(readbuf, sizeof(readbuf), fstream))
        {
            #if defined __APPLE__
                cache_line_size = atoi(readbuf+18);
            #elif defined __linux__
                cache_line_size = atoi(readbuf);
            #else
                #error NOT SUPPORTED OS
            #endif

            #if defined __APPLE__ || defined __linux__
                pclose(fstream);
            #endif
        }

        // it could be returning 0
        return cache_line_size;
    }
}
