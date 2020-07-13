#pragma once

#include <iostream>
#include <cstdio>
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
    const uint64 CACHE_BLOCK_SIZE = 64;
    const uint64 PFN_MASK         = ((((uint64)1)<<55)-1);
    const uint64 PFN_PRESENT_FLAG = (((uint64)1)<<63);

    #define page_map_file     "/proc/self/pagemap"

    void* aligned_malloc(const uint64 size, const uint64 alignment = 4096)
    {
        // make sure mem addr is aligned
        void* start_addr = NULL;
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
            printf("open %s failed\n", page_map_file);
            exit(-1);
        }

        if((off_t)-1 == lseek(fd, pfn_item_offset, SEEK_SET))
        {
            printf("lseek %s failed\n", page_map_file);
            exit(-1);
        }

        if(sizeof(uint64) != read(fd, &pfn_item, sizeof(uint64)))
        {
            printf("read %s failed for addr %llx\n", page_map_file, vir);
            exit(-1);
        }

        if(0==(pfn_item & PFN_PRESENT_FLAG))
        {
            printf("page is not present for addr %llx\n", vir);
            exit(-1);
        }
        uint64 phy = (pfn_item & PFN_MASK) * page_size + vir % page_size;
        //printf("virtual-to-physical mapping found for %llx page -> addr %llx\n", vir, phy);
        return phy;
    }
}