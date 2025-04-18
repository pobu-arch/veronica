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

using namespace std;

namespace veronica
{
    #define PATH_UNCACHED_DEV "/home/bowen/uncached_mem_dev"
    #define PATH_PAGE_MAP     "/proc/self/pagemap"

    const uint64 PFN_MASK         = ((((uint64)1)<<55)-1);
    const uint64 PFN_PRESENT_FLAG = (((uint64)1)<<63);

    void* uncached_mmap(const char *dev, unsigned int size)
    {
        #if defined __linux__
            int fd = open(dev == NULL ? PATH_UNCACHED_DEV : dev, O_RDWR, 0);
            if (fd == -1)
            {
                cout << "[error] couldn't open device" << endl;
                exit(-1);
            }

            unsigned int page_size = (unsigned int)get_page_size();
            unsigned int page_mask = (unsigned int)get_page_mask(); 

            if (size & ~page_mask) size = (size & page_mask) + page_size;

            void *map = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            if (map == MAP_FAILED)
            {
                cout << "[error] mmap has failed" << endl;
                exit(-1);
            }

            cout << "[info] mmap has completed" << endl;
            return map;
        #else
            //#error "NOT SUPPORTED OS"
            return NULL;
        #endif
    }

    void mlock_buffer(const void *addr, const uint64 &size)
    {
        if(mlock(addr, size) == -1)
        {
            cout << "[error] unable to pin memory pages" << endl;
            exit(-1);
        }
        else
        {
            //cout << "[info] memory pages were pinned successfully" << endl;
            return;
        }
    }

    // need to have sudo
    uint64 mem_addr_vir2phy(const uint64 &vir)
    {
        int fd;
        int page_size = getpagesize();
        unsigned long vir_page_idx    = vir/page_size;
        unsigned long pfn_item_offset = vir_page_idx * sizeof(uint64);
        uint64 pfn_item;

        fd = open(PATH_PAGE_MAP, O_RDONLY);
        if(fd < 0)
        {
            cout << "[error] open " << PATH_PAGE_MAP << " failed" << endl;
            exit(-1);
        }

        if(lseek(fd, pfn_item_offset, SEEK_SET) == (off_t)-1)
        {
            cout << "[error] lseek " << PATH_PAGE_MAP << " failed" << endl;
            exit(-1);
        }

        if(read(fd, &pfn_item, sizeof(uint64)) != sizeof(uint64))
        {
            cout << "[error] read " << PATH_PAGE_MAP << " for addr 0x" << hex << vir << " failed" << dec << endl;
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

    void* aligned_malloc(const uint64 &size, const uint64 &alignment = 4096)
    {
        // make sure mem addr is aligned
        void* start_addr = NULL;
        int err = posix_memalign(&start_addr, alignment, size);
        if (err != 0)
        {
            if (err == EINVAL)
            {
                cout << "[error] posix_memalign EINVAL" << endl;
            }
            else if (err == ENOMEM)
            {
                cout << "[error] posix_memalign ENOMEM" << endl;
            }
            exit(-1);
        }

        // printf("[info] posix_memalign ok, start_addr = %p\n", start_addr);
        return start_addr;
    }

    void* aligned_calloc(const uint64 &size, const uint64 &alignment = 4096)
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

    
#if defined ISA_X64
    // can be called from user mode
    static inline void flush_cache_line_x86(uint64 addr)
    {
        //asm volatile("clflush %0" : "+m" (*(char*)p));
        //asm volatile("clflush %0" : "+m" (*p));

        void *__p = (void *) addr;
        asm volatile("clflush %0" : "+m" (*(char*)__p));
    }

    void flush_cache_range(void* start, void* end)
    {
        char* __start = (char*) start;
        char* __end   = (char*) end;
        
        uint64 cache_line_size = get_cache_line_size();
        if(cache_line_size != 0)
        {
            while(__start < __end)
            {
                flush_cache_line_x86((uint64)(__start));
                __start += cache_line_size;
            }
        }
        else
        {
            cout << "[error] detected cache line size is " << cache_line_size << endl;
            exit(-1);
        }
    }
    
    // TODO need root access ?
    static inline void invalidate_tlb_entry_x86(void *p)
    {
        //addr = (unsigned long) addr;
        //asm volatile("invlpg (%0)" ::"r" (addr) : "memory");
        asm volatile("invlpg %0" : "+m" (*(char*)p));
    }
#endif
}
