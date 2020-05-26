#pragma once

#include <iostream>
#include <errno.h>
#include <sys/mman.h>

namespace veronica
{
    const uint64 CACHE_BLOCK_SIZE = 64;
    const uint64 PAGE_SIZE        = 4096;

    void* page_aligned_malloc(uint64 size)
    {
        // make sure mem addr is aligned
        void* start_addr = NULL;
        uint64 alignment = PAGE_SIZE;
        int err = posix_memalign(&start_addr, alignment, size);
        if (err != 0)
        {
            if (err == EINVAL)
            {
                printf("[error] posix_memalign EINVAL\n");
            }
            else if (err == ENOMEM)
            {
                printf("[error] posix_memalign ENOMEM\n");
            }
            exit(1);
        }

        printf("[info] posix_memalign ok, start_addr = %p\n", start_addr);
        return start_addr;
    }
}