//void __attribute__ ((noinline))
#pragma once
namespace veronica
{
    #define KB (1024ULL)
    #define MB (1024 * KB)
    #define GB (1024 * MB)

    #if defined __GNUC__
        #define likely(x)       __builtin_expect(!!(x), 1)
        #define unlikely(x)     __builtin_expect(!!(x), 0)
    #else
        #define likely(x)       (!!x)
        #define unlikely(x)     (!!x)
    #endif

    #if !defined ISA_X64 && !defined ISA_AARCH64 && !defined ISA_RV64
        #error "[ERROR-Veronica] Select one of -DISA_X64 or -DISA_AARCH64 or -DISA_RV64"
    #endif

    #if defined ISA_X64
        #define NOP_1           asm volatile("nop\n\t");
        #define NOP_2           {NOP_1 NOP_1}
        #define NOP_4           {NOP_2 NOP_2}
        #define NOP_8           {NOP_4 NOP_4}
        #define NOP_16          {NOP_8 NOP_8}
        #define NOP_32          {NOP_16 NOP_16}
        #define NOP_64          {NOP_32 NOP_32}

        #define MFENCE          asm volatile("mfence\n\t");
    #endif // ISA_X64

    #if defined ISA_AARCH64
        #define MFENCE          asm volatile("dmb ish": : :"memory");
    #endif // ISA_AARCH64
}