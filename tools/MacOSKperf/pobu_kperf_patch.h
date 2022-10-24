extern void renamed_exit(int res);
extern void renamed_exit(int res);

void renamed_exit(int res)
{
    // asm volatile("dsb ld" : : : "memory");
    // asm volatile("dsb ld" : : : "memory");
    // asm volatile("dsb ld" : : : "memory");
    // printf("[kperf-warning] exit code %d\n", res);
    throw res;
}