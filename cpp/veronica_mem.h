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
    // TODO: get cache line size programmatically
    const uint64 PFN_MASK         = ((((uint64)1)<<55)-1);
    const uint64 PFN_PRESENT_FLAG = (((uint64)1)<<63);
          uint64 CACHE_LINE_SIZE  = 0;

    #define page_map_file     "/proc/self/pagemap"

    // TODO
    uint64 get_cpu_freq_in_GHz()
    {
        return 0;
    }
    
    const char* get_os_type()
    {
        #if defined(__APPLE__)
            return "MACOSX";
        #endif

        #if defined(__linux__)
            return "LINUX";
        #endif
    }

    uint64 get_page_size()
    {
        return (uint64)getpagesize();
    }

    uint64 get_cache_line_size()
    {
        if(CACHE_LINE_SIZE != 0) 
            return CACHE_LINE_SIZE;
        
        FILE *fstream = NULL;
        char readbuf[32];
        memset(readbuf, 0, sizeof(readbuf));

        #if defined(__APPLE__)
        if(NULL == (fstream = popen("sysctl hw.cachelinesize","r")))
        #endif
        #if defined(__linux__)
        if(NULL == (fstream = popen("getconf LEVEL1_DCACHE_LINESIZE","r")))
        #endif
        {
            printf("[Error] execute command failed: %s\n", strerror(errno));
            exit(-1);
        }

        if(NULL != fgets(readbuf, sizeof(readbuf), fstream))
        {
            #if defined(__APPLE__)
            CACHE_LINE_SIZE = atoi(readbuf+18);
            #endif

            #if defined(__linux__)
            CACHE_LINE_SIZE = atoi(readbuf);
            #endif
            
            pclose(fstream);

            if(CACHE_LINE_SIZE == 0)
            {
                printf("[Error] failed to detect CACHE_LINE_SIZE\n");
            }
            return CACHE_LINE_SIZE;
        }
        return 0;
    }

    void* aligned_malloc(const uint64 size, const uint64 alignment = 4096)
    {
        // make sure mem addr is aligned
        void* start_addr = NULL;
        int err = posix_memalign(&start_addr, alignment, size);
        if (err != 0)
        {
            if (err == EINVAL)
            {
                printf("[Error] posix_memalign EINVAL\n");
            }
            else if (err == ENOMEM)
            {
                printf("[Error] posix_memalign ENOMEM\n");
            }
            exit(1);
        }

        // printf("[Info] posix_memalign ok, start_addr = %p\n", start_addr);
        return start_addr;
    }

    void* aligned_calloc(const uint64 size, const uint64 alignment = 4096)
    {
        // make sure mem addr is aligned
        void* start_addr = aligned_malloc(size, alignment);

        if(start_addr != NULL)
        {
            memset(start_addr, 0, size);
            return start_addr;
        }
        return NULL;
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
