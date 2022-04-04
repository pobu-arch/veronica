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
            #error "NOT SUPPORTED OS"
        #endif
    }

    uint64 get_cpu_freq()
    {
        uint64 freq = 0;
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
            cout << "[error] execute command failed: " << strerror(errno) << endl;
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
                freq        = strtoull(start, &end, 10);
                pclose(fstream);
                #if defined(__linux__)
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

        #if defined(__APPLE__)
            if(NULL == (fstream = popen("sysctl hw.cachelinesize", "r")))
        #elif defined(__linux__)
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
            #if defined(__APPLE__)
                cache_line_size = atoi(readbuf+18);
            #elif defined(__linux__)
                cache_line_size = atoi(readbuf);
            #else
                #error NOT SUPPORTED OS
            #endif

            #if defined(__APPLE__) || defined(__linux__)
                pclose(fstream);
            #endif
        }

        // it could be returning 0
        return cache_line_size;
    }

    void mlock_buffer(const void *addr, uint64 size)
    {
        if(mlock(addr, size) == -1)
        {
            cout << "[error] unable to pin memory pages" << endl;
            exit(-1);
        }
        else
        {
            //cout << "[info] memory pages were pinned successfully" << endl;
            return ;
        }
    }

    // need to have sudo
    uint64 mem_addr_vir2phy(uint64 vir)
    {
        int fd;
        int page_size = getpagesize();
        unsigned long vir_page_idx    = vir/page_size;
        unsigned long pfn_item_offset = vir_page_idx * sizeof(uint64);
        uint64 pfn_item;

        fd = open(page_map_file, O_RDONLY);
        if(fd < 0)
        {
            cout << "[error] open " << page_map_file << " failed" << endl;
            exit(-1);
        }

        if(lseek(fd, pfn_item_offset, SEEK_SET) == (off_t)-1)
        {
            cout << "[error] lseek " << page_map_file << " failed" << endl;
            exit(-1);
        }

        if(read(fd, &pfn_item, sizeof(uint64)) != sizeof(uint64))
        {
            cout << "[error] read " << page_map_file << " for addr 0x" << hex << vir << " failed" << dec << endl;
            exit(-1);
        }

        if((pfn_item & PFN_PRESENT_FLAG) == 0)
        {
            cout << "[error] page is not present for addr 0x" << hex << vir << endl;
            exit(-1);
        }
        close(fd);
        uint64 phy = (pfn_item & PFN_MASK) * page_size + vir % page_size;
        //cout << "virtual-to-physical mapping found for 0x" << hex << vir << " -> 0x" << (pfn_item) << dec << endl;
        return phy;
    }

    // TODO
    // need root access ?
    #if defined(ISA_X86_64)
    // can be called from user mode
    static inline void flush_cache_line_x86(uint64 addr)
    {
        void *__p = (void *) addr;
        asm volatile("clflush %0" : "+m" (*(char*)__p));
    }
    
    static inline void invalidate_tlb_entry_x86(uint64 addr)
    {
        //addr = (unsigned long) addr;
        //asm volatile("invlpg (%0)" ::"r" (addr) : "memory");
        void *__p = (void *) addr;
        asm volatile("invlpg %0" : "+m" (*(char*)__p));
    }
    #endif
}
