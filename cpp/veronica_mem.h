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
    #define UNCACHED_DEV_PATH "/home/bowen/uncached_mem_dev"

    void* uncached_mmap(char *dev, unsigned int size)
    {
        #if defined(__linux__)
        int fd = open(dev == NULL ? UNCACHED_DEV_PATH : dev, O_RDWR, 0);
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
    

    void* aligned_malloc(const uint64 size, const uint64 alignment = 4096)
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
