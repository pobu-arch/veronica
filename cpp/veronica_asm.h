inline void stream_load(void* start_addr)
{
#ifdef X86_64
    // AVX2 insts
    // every single inst should bring a new cache line
    // stride is pre-set to be 64 bytes, as most of the x86 CPUs have 64-byte cache line
    asm volatile("movdqa 0(%0), %%xmm0\n\t"
                "movdqa 128(%0), %%xmm2\n\t"
                "movdqa 256(%0), %%xmm4\n\t"
                "movdqa 384(%0), %%xmm6\n\t"
                "movdqa 512(%0), %%xmm0\n\t"
                "movdqa 640(%0), %%xmm2\n\t"
                "movdqa 768(%0), %%xmm4\n\t"
                "movdqa 896(%0), %%xmm6\n\t"
                "movdqa 1024(%0), %%xmm0\n\t"
                "movdqa 1152(%0), %%xmm2\n\t"
                "movdqa 1280(%0), %%xmm4\n\t"
                "movdqa 1408(%0), %%xmm6\n\t"
                "movdqa 1536(%0), %%xmm0\n\t"
                "movdqa 1664(%0), %%xmm2\n\t"
                "movdqa 1792(%0), %%xmm4\n\t"
                "movdqa 1920(%0), %%xmm4\n\t"
                :
                : "r"(start_addr)
                );

#endif

#ifdef ARMV8
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
#endif

#ifdef RISCV64
        asm volatile("ld t7, 0(%0)\n\t"
                     "ld t7, 128(%0)\n\t"
                     "ld t7, 256(%0)\n\t"
                     "ld t7, 384(%0)\n\t"
                     "ld t7, 512(%0)\n\t"
                     "ld t7, 640(%0)\n\t"
                     "ld t7, 768(%0)\n\t"
                     "ld t7, 896(%0)\n\t"
                     "ld t7, 1024(%0)\n\t"
                     "ld t7, 1152(%0)\n\t"
                     "ld t7, 1280(%0)\n\t"
                     "ld t7, 1408(%0)\n\t"
                     "ld t7, 1536(%0)\n\t"
                     "ld t7, 1664(%0)\n\t"
                     "ld t7, 1792(%0)\n\t"
                     "ld t7, 1920(%0)\n\t"
                    :
                    : "r"(start_addr)
                    );
#endif
}

inline void stream_store(void* start_addr)
{

#ifdef X86_64
    // AVX2 insts
    asm volatile("movdqa %%xmm0, 0(%0)\n\t"
                "movdqa %%xmm2, 128(%0)\n\t"
                "movdqa %%xmm4, 256(%0)\n\t"
                "movdqa %%xmm6, 384(%0)\n\t"
                "movdqa %%xmm0, 512(%0)\n\t"
                "movdqa %%xmm2, 640(%0)\n\t"
                "movdqa %%xmm4, 768(%0)\n\t"
                "movdqa %%xmm6, 896(%0)\n\t"
                "movdqa %%xmm0, 1024(%0)\n\t"
                "movdqa %%xmm2, 1152(%0)\n\t"
                "movdqa %%xmm4, 1280(%0)\n\t"
                "movdqa %%xmm6, 1408(%0)\n\t"
                "movdqa %%xmm0, 1536(%0)\n\t"
                "movdqa %%xmm2, 1664(%0)\n\t"
                "movdqa %%xmm4, 1792(%0)\n\t"
                "movdqa %%xmm6, 1920(%0)\n\t"
                :
                : "r"(start_addr)
                );

#endif

#ifdef ARMV8
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
#endif

#ifdef RISCV64
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
#endif
}