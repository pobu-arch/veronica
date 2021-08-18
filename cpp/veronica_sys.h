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
    const uint64 PFN_MASK         = ((((uint64)1)<<55)-1);
    const uint64 PFN_PRESENT_FLAG = (((uint64)1)<<63);

    #define page_map_file     "/proc/self/pagemap"

    const char* get_os_type()
    {
        #if defined(__APPLE__)
            return "MACOSX";
        #elif defined(__linux__)
            return "LINUX";
        #else
            #error NOT SUPPORTED OS
        #endif
    }

    uint64 get_cpu_freq()
    {
        uint64 FREQ = 0;
        FILE *fstream = NULL;
        char readbuf[64];
        memset(readbuf, 0, sizeof(readbuf));
        
        #if defined(__APPLE__)
            if(NULL == (fstream = popen("sysctl hw.cpufrequency", "r")))
        #elif defined(__linux__)
            if(NULL == (fstream = popen("cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq", "r")))
        #else
            #error NOT SUPPORTED OS
        #endif
        {
            printf("[Error] execute command failed: %s\n", strerror(errno));
            exit(-1);
        }

        if(NULL != fgets(readbuf, sizeof(readbuf), fstream))
        {
            #if defined(__APPLE__)
                char* start = readbuf+17;
            #elif defined(__linux__)
                char* start = readbuf;
            #else
                #error NOT SUPPORTED OS
            #endif

            #if defined(__APPLE__) || defined(__linux__)
                char* end   = start + strlen(start);
                FREQ        = strtoull(start, &end, 10);
                pclose(fstream);
            #endif
        }
        // it could be returning 0
        return FREQ;
    }

    uint64 get_page_size()
    {
        return (uint64)getpagesize();
    }

    uint64 get_cache_line_size()
    {
        uint64 CACHE_LINE_SIZE = 0;
        FILE *fstream = NULL;
        char readbuf[64];
        memset(readbuf, 0, sizeof(readbuf));

        #if defined(__APPLE__)
            if(NULL == (fstream = popen("sysctl hw.cachelinesize", "r")))
        #elif defined(__linux__)
            if(NULL == (fstream = popen("getconf LEVEL1_DCACHE_LINESIZE", "r")))
        #else
            #error NOT SUPPORTED OS
        #endif
        {
            printf("[Error] execute command failed: %s\n", strerror(errno));
            exit(-1);
        }

        if(NULL != fgets(readbuf, sizeof(readbuf), fstream))
        {
            #if defined(__APPLE__)
                CACHE_LINE_SIZE = atoi(readbuf+18);
            #elif defined(__linux__)
                CACHE_LINE_SIZE = atoi(readbuf);
            #else
                #error NOT SUPPORTED OS
            #endif

            #if defined(__APPLE__) || defined(__linux__)
                pclose(fstream);
            #endif
        }

        // it could be returning 0
        return CACHE_LINE_SIZE;
    }

    uint64 mem_addr_vir2phy(uint64 vir)
    {
        int fd;
        int page_size = getpagesize();
        unsigned long vir_page_idx    = vir/page_size;
        unsigned long pfn_item_offset = vir_page_idx * sizeof(uint64);
        uint64 pfn_item;

        fd = open(page_map_file, O_RDONLY);
        if(fd<0)
        {
            printf("[Error] open %s failed\n", page_map_file);
            exit(-1);
        }

        if((off_t)-1 == lseek(fd, pfn_item_offset, SEEK_SET))
        {
            printf("[Error] lseek %s failed\n", page_map_file);
            exit(-1);
        }

        if(sizeof(uint64) != read(fd, &pfn_item, sizeof(uint64)))
        {
            printf("[Error] read %s failed for addr %llx\n", page_map_file, vir);
            exit(-1);
        }

        if(0==(pfn_item & PFN_PRESENT_FLAG))
        {
            printf("[Error] page is not present for addr %llx\n", vir);
            exit(-1);
        }
        uint64 phy = (pfn_item & PFN_MASK) * page_size + vir % page_size;
        //printf("virtual-to-physical mapping found for %llx page -> addr %llx\n", vir, phy);
        return phy;
    }

    // TODO
    // need root access ?
    static inline void invalidate_tlb_entry_x86(uint64 addr)
    {
	    //addr = (unsigned long) addr;
        //asm volatile("invlpg (%0)" ::"r" (addr) : "memory");
        void *__p = (void *) addr;
        asm volatile("invlpg %0" : "+m" (*(char*)__p));
    }

    // can be called from user mode
    static inline void flush_cache_line_x86(uint64 addr)
    {
        void *__p = (void *) addr;
        asm volatile("clflush %0" : "+m" (*(char*)__p));
    }
}