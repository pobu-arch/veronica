//void __attribute__ ((noinline))

#ifdef MACRO_ISA_X86_64
    #define NOP_1       asm volatile("nop\n\t");
    #define NOP_2       {NOP_1 NOP_1}
    #define NOP_4       {NOP_2 NOP_2}
    #define NOP_8       {NOP_4 NOP_4}
    #define NOP_16      {NOP_8 NOP_8}
    #define NOP_32      {NOP_16 NOP_16}
    #define NOP_64      {NOP_32 NOP_32}
    #define NOP_128     {NOP_64 NOP_64}
    #define NOP_256     {NOP_128 NOP_128}
    #define NOP_512     {NOP_256 NOP_256}
    #define NOP_1024    {NOP_512 NOP_512}
    #define NOP_2048    {NOP_1024 NOP_1024}
    #define NOP_4096    {NOP_2048 NOP_2048}
    #define NOP_8192    {NOP_4096 NOP_4096}
    #define NOP_16384   {NOP_8192 NOP_8192}
    #define NOP_32768   {NOP_16384 NOP_16384}
    #define NOP_65536   {NOP_32768 NOP_32768}
    #define NOP_131072  {NOP_65536 NOP_65536}

    #define FENCE       asm volatile("mfence\n\t");
#endif // MACRO_ISA_X86_64

inline void stream_load(void* start_addr)
{
    #ifdef MACRO_ISA_X86_64
        // AVX2 insts
        // every single inst should bring a new cache line
        // stride is pre-set to be 64 bytes, as most of the x86 CPUs have 64-byte cache line
        #if MACRO_CACHE_LINE_SIZE == 64
            asm volatile("movdqa 0(%0), %%xmm0\n\t"
                        "movdqa 64(%0), %%xmm1\n\t"
                        "movdqa 128(%0), %%xmm2\n\t"
                        "movdqa 192(%0), %%xmm3\n\t"
                        "movdqa 256(%0), %%xmm4\n\t"
                        "movdqa 320(%0), %%xmm5\n\t"
                        "movdqa 384(%0), %%xmm6\n\t"
                        "movdqa 448(%0), %%xmm7\n\t"
                        "movdqa 512(%0), %%xmm0\n\t"
                        "movdqa 576(%0), %%xmm1\n\t"
                        "movdqa 640(%0), %%xmm2\n\t"
                        "movdqa 704(%0), %%xmm3\n\t"
                        "movdqa 768(%0), %%xmm4\n\t"
                        "movdqa 832(%0), %%xmm5\n\t"
                        "movdqa 896(%0), %%xmm6\n\t"
                        "movdqa 960(%0), %%xmm7\n\t"
                        "movdqa 1024(%0), %%xmm0\n\t"
                        "movdqa 1088(%0), %%xmm1\n\t"
                        "movdqa 1152(%0), %%xmm2\n\t"
                        "movdqa 1216(%0), %%xmm3\n\t"
                        "movdqa 1280(%0), %%xmm4\n\t"
                        "movdqa 1344(%0), %%xmm5\n\t"
                        "movdqa 1408(%0), %%xmm6\n\t"
                        "movdqa 1472(%0), %%xmm7\n\t"
                        "movdqa 1536(%0), %%xmm0\n\t"
                        "movdqa 1600(%0), %%xmm1\n\t"
                        "movdqa 1664(%0), %%xmm2\n\t"
                        "movdqa 1728(%0), %%xmm3\n\t"
                        "movdqa 1792(%0), %%xmm4\n\t"
                        "movdqa 1856(%0), %%xmm5\n\t"
                        "movdqa 1920(%0), %%xmm6\n\t"
                        "movdqa 1984(%0), %%xmm7\n\t"
                        :
                        : "r"(start_addr)
                        );
        #else
            #error "NOT SUPPORTED CACHE_LINE_SIZE"
        #endif // MACRO_CACHE_LINE_SIZE

    #elif defined MACRO_ISA_ARMV8
        #if MACRO_CACHE_LINE_SIZE == 64
            asm volatile("ldr x7, [%0, #0]\n\t"
                        "ldr x7, [%0, #64]\n\t"
                        "ldr x7, [%0, #128]\n\t"
                        "ldr x7, [%0, #192]\n\t"
                        "ldr x7, [%0, #256]\n\t"
                        "ldr x7, [%0, #320]\n\t"
                        "ldr x7, [%0, #384]\n\t"
                        "ldr x7, [%0, #448]\n\t"
                        "ldr x7, [%0, #512]\n\t"
                        "ldr x7, [%0, #576]\n\t"
                        "ldr x7, [%0, #640]\n\t"
                        "ldr x7, [%0, #704]\n\t"
                        "ldr x7, [%0, #768]\n\t"
                        "ldr x7, [%0, #832]\n\t"
                        "ldr x7, [%0, #896]\n\t"
                        "ldr x7, [%0, #960]\n\t"
                        "ldr x7, [%0, #1024]\n\t"
                        "ldr x7, [%0, #1088]\n\t"
                        "ldr x7, [%0, #1152]\n\t"
                        "ldr x7, [%0, #1216]\n\t"
                        "ldr x7, [%0, #1280]\n\t"
                        "ldr x7, [%0, #1344]\n\t"
                        "ldr x7, [%0, #1408]\n\t"
                        "ldr x7, [%0, #1472]\n\t"
                        "ldr x7, [%0, #1536]\n\t"
                        "ldr x7, [%0, #1600]\n\t"
                        "ldr x7, [%0, #1664]\n\t"
                        "ldr x7, [%0, #1728]\n\t"
                        "ldr x7, [%0, #1792]\n\t"
                        "ldr x7, [%0, #1856]\n\t"
                        "ldr x7, [%0, #1920]\n\t"
                        "ldr x7, [%0, #1984]\n\t"
                        :
                        : "r"(start_addr)
                        );
        #elif MACRO_CACHE_LINE_SIZE == 128
        // Apple M1 uses 128-byte cache line
            asm volatile("ldr x7, [%0, #0]\n\t"
                        "ldr x7, [%0, #128]\n\t"
                        "ldr x7, [%0, #256]\n\t"
                        "ldr x7, [%0, #384]\n\t"
                        "ldr x7, [%0, #512]\n\t"
                        "ldr x7, [%0, #640]\n\t"
                        "ldr x7, [%0, #768]\n\t"
                        "ldr x7, [%0, #1024]\n\t"
                        "ldr x7, [%0, #1152]\n\t"
                        "ldr x7, [%0, #1280]\n\t"
                        "ldr x7, [%0, #1408]\n\t"
                        "ldr x7, [%0, #1536]\n\t"
                        "ldr x7, [%0, #1664]\n\t"
                        "ldr x7, [%0, #1792]\n\t"
                        "ldr x7, [%0, #1920]\n\t"
                        :
                        : "r"(start_addr)
                        );
        #else
            #error "NOT SUPPORTED CACHE_LINE_SIZE"
        #endif // MACRO_CACHE_LINE_SIZE

    #elif defined MACRO_ISA_RISCV64
        #if MACRO_CACHE_LINE_SIZE == 64
            asm volatile("ld t7, 0(%0)\n\t"
                        "ld t7, 64(%0)\n\t"
                        "ld t7, 128(%0)\n\t"
                        "ld t7, 192(%0)\n\t"
                        "ld t7, 256(%0)\n\t"
                        "ld t7, 320(%0)\n\t"
                        "ld t7, 384(%0)\n\t"
                        "ld t7, 448(%0)\n\t"
                        "ld t7, 512(%0)\n\t"
                        "ld t7, 576(%0)\n\t"
                        "ld t7, 640(%0)\n\t"
                        "ld t7, 704(%0)\n\t"
                        "ld t7, 768(%0)\n\t"
                        "ld t7, 832(%0)\n\t"
                        "ld t7, 896(%0)\n\t"
                        "ld t7, 960(%0)\n\t"
                        "ld t7, 1024(%0)\n\t"
                        "ld t7, 1088(%0)\n\t"
                        "ld t7, 1152(%0)\n\t"
                        "ld t7, 1216(%0)\n\t"
                        "ld t7, 1280(%0)\n\t"
                        "ld t7, 1344(%0)\n\t"
                        "ld t7, 1408(%0)\n\t"
                        "ld t7, 1472(%0)\n\t"
                        "ld t7, 1536(%0)\n\t"
                        "ld t7, 1600(%0)\n\t"
                        "ld t7, 1664(%0)\n\t"
                        "ld t7, 1728(%0)\n\t"
                        "ld t7, 1792(%0)\n\t"
                        "ld t7, 1856(%0)\n\t"
                        "ld t7, 1920(%0)\n\t"
                        "ld t7, 1984(%0)\n\t"
                        :
                        : "r"(start_addr)
                        );
        #else
            #error "NOT SUPPORTED CACHE_LINE_SIZE"
        #endif // MACRO_CACHE_LINE_SIZE
    #else
        #error "NOT SUPPORTED ISA"
    #endif // MACRO_ISA
}

inline void stream_store(void* start_addr)
{
    #ifdef MACRO_ISA_X86_64
        #if MACRO_CACHE_LINE_SIZE == 64
            // AVX2 insts
            asm volatile("movdqa %%xmm0, 0(%0)\n\t"
                        "movdqa %%xmm1, 64(%0)\n\t"
                        "movdqa %%xmm2, 128(%0)\n\t"
                        "movdqa %%xmm3, 192(%0)\n\t"
                        "movdqa %%xmm4, 256(%0)\n\t"
                        "movdqa %%xmm5, 320(%0)\n\t"
                        "movdqa %%xmm6, 384(%0)\n\t"
                        "movdqa %%xmm7, 448(%0)\n\t"
                        "movdqa %%xmm0, 512(%0)\n\t"
                        "movdqa %%xmm1, 576(%0)\n\t"
                        "movdqa %%xmm2, 640(%0)\n\t"
                        "movdqa %%xmm3, 704(%0)\n\t"
                        "movdqa %%xmm4, 768(%0)\n\t"
                        "movdqa %%xmm5, 832(%0)\n\t"
                        "movdqa %%xmm6, 896(%0)\n\t"
                        "movdqa %%xmm7, 960(%0)\n\t"
                        "movdqa %%xmm0, 1024(%0)\n\t"
                        "movdqa %%xmm1, 1088(%0)\n\t"
                        "movdqa %%xmm2, 1152(%0)\n\t"
                        "movdqa %%xmm3, 1216(%0)\n\t"
                        "movdqa %%xmm4, 1280(%0)\n\t"
                        "movdqa %%xmm5, 1344(%0)\n\t"
                        "movdqa %%xmm6, 1408(%0)\n\t"
                        "movdqa %%xmm7, 1472(%0)\n\t"
                        "movdqa %%xmm0, 1536(%0)\n\t"
                        "movdqa %%xmm1, 1600(%0)\n\t"
                        "movdqa %%xmm2, 1664(%0)\n\t"
                        "movdqa %%xmm3, 1728(%0)\n\t"
                        "movdqa %%xmm4, 1792(%0)\n\t"
                        "movdqa %%xmm5, 1856(%0)\n\t"
                        "movdqa %%xmm6, 1920(%0)\n\t"
                        "movdqa %%xmm7, 1984(%0)\n\t"
                        :
                        : "r"(start_addr)
                        );
        #else
            #error "NOT SUPPORTED CACHE_LINE_SIZE"
        #endif // MACRO_CACHE_LINE_SIZE

    #elif defined MACRO_ISA_ARMV8
        #if MACRO_CACHE_LINE_SIZE == 64
            asm volatile("str x7, [%0, #0]\n\t"
                        "str x7, [%0, #64]\n\t"
                        "str x7, [%0, #128]\n\t"
                        "str x7, [%0, #192]\n\t"
                        "str x7, [%0, #256]\n\t"
                        "str x7, [%0, #320]\n\t"
                        "str x7, [%0, #384]\n\t"
                        "str x7, [%0, #448]\n\t"
                        "str x7, [%0, #512]\n\t"
                        "str x7, [%0, #576]\n\t"
                        "str x7, [%0, #640]\n\t"
                        "str x7, [%0, #704]\n\t"
                        "str x7, [%0, #768]\n\t"
                        "str x7, [%0, #832]\n\t"
                        "str x7, [%0, #896]\n\t"
                        "str x7, [%0, #960]\n\t"
                        "str x7, [%0, #1024]\n\t"
                        "str x7, [%0, #1088]\n\t"
                        "str x7, [%0, #1152]\n\t"
                        "str x7, [%0, #1216]\n\t"
                        "str x7, [%0, #1280]\n\t"
                        "str x7, [%0, #1344]\n\t"
                        "str x7, [%0, #1408]\n\t"
                        "str x7, [%0, #1472]\n\t"
                        "str x7, [%0, #1536]\n\t"
                        "str x7, [%0, #1600]\n\t"
                        "str x7, [%0, #1664]\n\t"
                        "str x7, [%0, #1728]\n\t"
                        "str x7, [%0, #1792]\n\t"
                        "str x7, [%0, #1856]\n\t"
                        "str x7, [%0, #1920]\n\t"
                        "str x7, [%0, #1984]\n\t"
                        :
                        : "r"(start_addr)
                        );

        #elif MACRO_CACHE_LINE_SIZE == 128
        // Apple M1 uses 128-byte cache line
            asm volatile("str x7, [%0, #0]\n\t"
                        "str x7, [%0, #128]\n\t"
                        "str x7, [%0, #256]\n\t"
                        "str x7, [%0, #384]\n\t"
                        "str x7, [%0, #512]\n\t"
                        "str x7, [%0, #640]\n\t"
                        "str x7, [%0, #768]\n\t"
                        "str x7, [%0, #896]\n\t"
                        "str x7, [%0, #1024]\n\t"
                        "str x7, [%0, #1152]\n\t"
                        "str x7, [%0, #1280]\n\t"
                        "str x7, [%0, #1408]\n\t"
                        "str x7, [%0, #1536]\n\t"
                        "str x7, [%0, #1664]\n\t"
                        "str x7, [%0, #1792]\n\t"
                        "str x7, [%0, #1920]\n\t"
                        :
                        : "r"(start_addr)
                        );
        #else
            #error "NOT SUPPORTED CACHE_LINE_SIZE"
        #endif // MACRO_CACHE_LINE_SIZE

    #elif defined MACRO_ISA_RISCV64
        #if MACRO_CACHE_LINE_SIZE == 64
            asm volatile("sd t7, 0(%0)\n\t"
                        "sd t7, 128(%0)\n\t"
                        "sd t7, 256(%0)\n\t"
                        "sd t7, 384(%0)\n\t"
                        "sd t7, 512(%0)\n\t"
                        "sd t7, 640(%0)\n\t"
                        "sd t7, 768(%0)\n\t"
                        "sd t7, 896(%0)\n\t"
                        "sd t7, 1024(%0)\n\t"
                        "sd t7, 1152(%0)\n\t"
                        "sd t7, 1280(%0)\n\t"
                        "sd t7, 1408(%0)\n\t"
                        "sd t7, 1536(%0)\n\t"
                        "sd t7, 1664(%0)\n\t"
                        "sd t7, 1792(%0)\n\t"
                        "sd t7, 1920(%0)\n\t"
                        :
                        : "r"(start_addr)
                        );
        #else
            #error "NOT SUPPORTED CACHE_LINE_SIZE"
        #endif // MACRO_CACHE_LINE_SIZE
    #else
        #error "NOT SUPPORTED ISA"
    #endif // MACRO_ISA
}

uint64 inst_nop(uint64* input)
{
    uint64 i = 1;
    #ifdef MACRO_ISA_X86_64 
        if(input[i++] % 2147483647 == 0)
        {
            FENCE
            NOP_4096
            printf("temp\n");
            return i;
        }
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else if(input[i++] % 2147483647 == 0){FENCE; NOP_65536; return i;}
        else return i;
    #endif // MACRO_ISA_X86_64
}