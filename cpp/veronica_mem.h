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
    #define MACRO_UNCACHED_DEV_PATH "/home/bowen/uncached_mem_dev"
    
    #if defined(__linux__)
    void* uncached_mmap(char *dev, int size)
    {
        int fd = open(dev == NULL ? MACRO_UNCACHED_DEV_PATH : dev, O_RDWR, 0);
        if (fd == -1)
        {
            printf("[Error] couldn't open device\n");
            exit(-1);
        }

        int page_size = (int)get_page_size();
        int page_mask = (int)get_page_mask(); 

        if (size & ~page_mask)
                size = (size & page_mask) + page_size;

        void *map = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (map == MAP_FAILED)
        {
            printf("[Error] mmap failed.\n");
            exit(-1);
        }

        printf("[Info] mmap()'completed %s\n", dev);
        return map;
    }
    #else
        #error "NOT SUPPORTED OS"
    #endif

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
}
