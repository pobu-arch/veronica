#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "pthread.h"
#include "pthread/qos.h"
#include "pobu_kperf.hpp"
#include "pobu_kperf_patch.hpp"

#define KPERF_LOG_FILE "kperf.log"

// -----------------------------------------------------------------------------
// Demo
// -----------------------------------------------------------------------------

#define PERFORMANCE
//#define EFFICIENCY

#if !defined PERFORMANCE && !defined EFFICIENCY
#define PERFORMANCE
#endif

#define EVENT_NAME_MAX 8
typedef struct {
    const char *alias; /// name for print
    const char *names[EVENT_NAME_MAX]; /// name from pmc db
} event_alias;

/// Event names from /usr/share/kpep/<name>.plist
static const event_alias profile_events[] = {

// Fixed counters start here
    {   "instructions", {
            "FIXED_INSTRUCTIONS"           // Apple A7-A15
           // "INST_RETIRED.ANY"           // Intel Yonah, Merom, Core 1th-10th
    }},    
    {   "cycles", {
            "FIXED_CYCLES"                 // Apple A7-A15
            //"CPU_CLK_UNHALTED.THREAD",   // Intel Core 1th-10th
            //"CPU_CLK_UNHALTED.CORE",     // Intel Yonah, Merom
    }},
    
// Configurable counters start here
    {   "core_active_cycles", {
            "CORE_ACTIVE_CYCLE"                     // Apple
    }},

    // Instruction
    // {   "branches.retired", {
    //         "INST_BRANCH"                            // Apple, 3-bit
    // }},
    // {   "int_alu_insts.retired", {
    //         "INST_INT_ALU"                           // Apple, 1-bit
    // }},
    // {   "int_load_insts.retired", {
    //         "INST_INT_LD"                            // Apple, 3-bit
    // }},
    // {   "int_store_insts.retired", {
    //         "INST_INT_ST"                            // Apple, 1-bit
    // }},
    // {   "load_store_insts.retired", {
    //         "INST_LDST"                              // Apple, 1-bit
    // }},
    // {   "simd_alu_insts.retired", {
    //         "INST_SIMD_ALU"                          // Apple, 1-bit
    // }},
    // {   "simd_loads.retired", {
    //         "INST_SIMD_LD"                           // Apple, 3-bit
    // }},
    // {   "simd_stores.retired", {
    //         "INST_SIMD_ST"                           // Apple, 3-bit
    // }},
    // {   "uops_retired", {
    //         "RETIRE_UOP"                             // Apple, 1-bit
    // }},
    // {   "insts_retired", {
    //         "INST_ALL"                             // Apple, 3-bit
    // }},
    
    // Speculation
    // {   "uops_issued", {
    //         "SCHEDULE_UOP"                          // Apple
    // }},
    {   "uops_issued", {
            "MAP_UOP"                               // Apple
    }},
    {   "branches_mispredicted.retired", {
            "BRANCH_MISPRED_NONSPEC"                // Apple A7-A15, since iOS 15, macOS 12, 3-bit
    }},
    // {   "branches_conditional", {
    //         "INST_BRANCH_COND"                      // Apple, 3-bit
    // }},
    // {   "branches_taken", {
    //         "INST_BRANCH_TAKEN"                     // Apple, 3-bit
    // }},
    // {   "call_branches.retired", {
    //         "INST_BRANCH_CALL"                      // Apple, 3-bit
    // }},
    // {   "return_branches.retired", {
    //         "INST_BRANCH_RET"                       // Apple, 3-bit
    // }},
    // {   "indir_branches.retired", {
    //         "INST_BRANCH_INDIR"                     // Apple, 3-bit
    // }},
    // {   "branches_conditional_mispredicted.retired", {
    //         "BRANCH_COND_MISPRED_NONSPEC"           // Apple, 3-bit
    // }},
    // {   "indir_calls_mispredicted.retired", {
    //         "BRANCH_CALL_INDIR_MISPRED_NONSPEC"     // Apple, 3-bit
    // }},
    // {   "returns_mispredicted.retired", {
    //         "BRANCH_RET_INDIR_MISPRED_NONSPEC"      // Apple, 3-bit
    // }},
    // {   "indir_branches_mispredicted.retired", {
    //         "BRANCH_INDIR_MISPRED_NONSPEC"          // Apple, 3-bit
    // }},
    // {   "fetch_restart_exclude_branch_prediction", {
    //         "FETCH_RESTART"                         // Apple
    // }},
    // {   "store_mem_violation.retired", {
    //         "ST_MEMORY_ORDER_VIOLATION_NONSPEC"     // Apple, 3-bit
    // }},
    // {   "map_rewinding_cycles", {
    //         "MAP_REWIND"                            // Apple
    // }},
    // {   "map_int_uops.speculative", {
    //         "MAP_INT_UOP"                           // Apple
    // }},
    // {   "map_load_store_uops.speculative", {
    //         "MAP_LDST_UOP"                          // Apple
    // }},
    // {   "load_uops.speculative", {
    //         "LD_UNIT_UOP"                           // Apple
    // }},
    // {   "store_uops.speculative", {
    //         "ST_UNIT_UOP"                           // Apple
    // }},

    // Front-end
    // {   "l1i_cache_demand_misses", {
    //         "L1I_CACHE_MISS_DEMAND"                 // Apple
    // }},
    // {   "l1i_tlb_refills", {
    //         "L1I_TLB_FILL"                          // Apple
    // }},
    // {   "l1i_tlb_demand_misses", {
    //         "L1I_TLB_MISS_DEMAND"                   // Apple
    // }},
    // {   "l2_tlb_misses_inst", {
    //         "L2_TLB_MISS_INSTRUCTION"               // Apple
    // }},
    // {   "page_table_walk_inst", {
    //         "MMU_TABLE_WALK_INSTRUCTION"            // Apple
    // }},

    // Back-end
    // {   "l1d_load_misses.retired", {
    //         "L1D_CACHE_MISS_LD_NONSPEC"             // Apple, 3-bit
    // }},
    // {   "l1d_load_misses", {
    //         "L1D_CACHE_MISS_LD"                     // Apple
    // }},
    // {   "l1d_store_misses.retired", {
    //         "L1D_CACHE_MISS_ST_NONSPEC"             // Apple, 3-bit
    // }},
    // {   "l1d_sotre_misses", {
    //         "L1D_CACHE_MISS_ST"                     // Apple
    // }},
    // {   "l1d_writeback", {
    //         "L1D_CACHE_WRITEBACK"                   // Apple
    // }},
    // {   "l1d_tlb_accesses", {
    //         "L1D_TLB_ACCESS"                        // Apple
    // }},
    // {   "l1d_tlb_refills", {
    //         "L1D_TLB_FILL"                          // Apple
    // }},
    // {   "l1d_tlb_misses.retired", {
    //         "L1D_TLB_MISS_NONSPEC"                  // Apple, 3-bit
    // }},
    // {   "l1d_tlb_misses", {
    //         "L1D_TLB_MISS"                          // Apple
    // }},
    // {   "l2_tlb_misses_data", {
    //         "L2_TLB_MISS_DATA"                      // Apple
    // }},
    // {   "page_table_walk_data", {
    //         "MMU_TABLE_WALK_DATA"                   // Apple
    // }},
    // {   "map_bubbles", {
    //         "MAP_DISPATCH_BUBBLE"                   // Apple
    // }},
    // {   "map_stall_cycles", {
    //         "MAP_STALL"                             // Apple
    // }},
    // {   "map_stalls_due_to_dispatch", {
    //         "MAP_STALL_DISPATCH"                    // Apple
    // }},
    // {   "map_bubbles.l1i", {
    //         "MAP_DISPATCH_BUBBLE_IC"               // Apple
    // }},

    // Others
    // {   "page_fault.retired", {
    //         "MMU_VIRTUAL_MEMORY_FAULT_NONSPEC"      // Apple
    // }},
    // {   "map_simd_uops.speculative", {
    //         "MAP_SIMD_UOP"                          // Apple
    // }},
    // {   "non_temporal_store_uops.speculative", {
    //         "ST_NT_UOP"                             // Apple
    // }},
    // {   "non_temporal_load_uops.speculative", {
    //         "LD_NT_UOP"                             // Apple
    // }},
    // {   "load_store_uops_cross_64B.retired", {
    //         "LDST_X64_UOP"                          // Apple
    // }},
    // {   "load_store_uops_cross_page.retired", {
    //         "LDST_XPG_UOP"                          // Apple
    // }}
    // {   "other_flushes", {
    //         "FLUSH_RESTART_OTHER_NONSPEC"           // Apple
    // }},
    // {   "cycles_with_pending_interrupt", {
    //         "INTERRUPT_PENDING"                     // Apple
    // }},
    // {   "barriers.retired", {
    //         "INST_BARRIER"                          // Apple, 3-bit
    // }},
    // {   "atomic_or_exclusive_successful", {
    //         "ATOMIC_OR_EXCLUSIVE_SUCC"              // Apple
    // }},
    // {   "atomic_or_exclusive_fail", {
    //         "ATOMIC_OR_EXCLUSIVE_FAIL"              // Apple
    // }}
};

static void apply_core_bias(FILE* kperf_log_fh) {

    qos_class_t cls = QOS_CLASS_DEFAULT;
#ifdef PERFORMANCE
    cls = QOS_CLASS_USER_INTERACTIVE;
#else
    cls = QOS_CLASS_BACKGROUND;
#endif
    
    pthread_set_qos_class_self_np(cls, 0);
    int rel = 0;
    qos_class_t cur = QOS_CLASS_UNSPECIFIED;
    (void)pthread_get_qos_class_np(pthread_self(), &cur, &rel);
#ifdef PERFORMANCE    
    fprintf(kperf_log_fh, "[INFO-KPerf] set QoS to %d (rel %d) PERFORMANCE\n", cur, rel);
#else
    fprintf(kperf_log_fh, "[INFO-KPerf] set QoS to %d (rel %d) EFFICIENCY\n", cur, rel);
#endif
    
    fflush(kperf_log_fh);
}

int profile_func(int argc, char ** argv, FILE *kperf_log_fh) {
    // if one changes this function name 'renamed_main' below
    // then the helper function within the benchmarks repo should be changed correspondingly
    int res;
    apply_core_bias(kperf_log_fh);
    try {
        res = renamed_main(argc, argv);
    }catch(int res){
        if(res != 0)
        {
            fprintf(kperf_log_fh, "[WARNING-KPerf] return code from profile func is %d\n", res);
        }else{
            fprintf(kperf_log_fh, "[INFO-KPerf] return code from profile func is %d\n", res);
        }
        fflush(kperf_log_fh);
        return res;
    }
    return res;
}

static kpep_event *get_event(kpep_db *db, const event_alias *alias) {
    for (usize j = 0; j < EVENT_NAME_MAX; j++) {
        const char *name = alias->names[j];
        if (!name) break;
        kpep_event *ev = NULL;
        if (kpep_db_event(db, name, &ev) == 0) {
            return ev;
        }
    }
    return NULL;
}

int main(int argc, char ** argv) {
    int ret = 0;

    FILE *kperf_log_fh = fopen(KPERF_LOG_FILE, "w");
    if(kperf_log_fh == NULL)
    {
        printf("[Error-KPerf] Cannot open log file %s to write.\n", KPERF_LOG_FILE);
        return 1;
    }
    
    // load dylib
    if (!lib_init()) {
        fprintf(kperf_log_fh, "[ERROR-KPerf] %s\n", lib_err_msg);
        fflush(kperf_log_fh);
        fclose(kperf_log_fh);
        return 1;
    }
    
    // get PMU version
    // check permission
    int force_ctrs = 0;
    if (kpc_force_all_ctrs_get(&force_ctrs)) {
        fprintf(kperf_log_fh, "[ERROR-KPerf] Permission denied, xnu/kpc requires root privileges.\n");
        fflush(kperf_log_fh);
        fclose(kperf_log_fh);
        return 1;
    }
    
    // load pmc db
    kpep_db *db = NULL;
    if ((ret = kpep_db_create(NULL, &db))) {
        fprintf(kperf_log_fh, "[ERROR-KPerf] cannot load pmc database: %d.\n", ret);
        fflush(kperf_log_fh);
        fclose(kperf_log_fh);
        return 1;
    }
    fprintf(kperf_log_fh, "[INFO-KPerf] loaded db: %s (%s)\n", db->name, db->marketing_name);
    fprintf(kperf_log_fh, "[INFO-KPerf] number of fixed counters: %zu\n", db->fixed_counter_count);
    fprintf(kperf_log_fh, "[INFO-KPerf] number of configurable counters: %zu\n", db->config_counter_count);
    
    // create a config
    kpep_config *cfg = NULL;
    if ((ret = kpep_config_create(db, &cfg))) {
        fprintf(kperf_log_fh, "[ERROR-KPerf] Failed to create kpep config: %d (%s).\n",
               ret, kpep_config_error_desc(ret));
        fflush(kperf_log_fh);
        fclose(kperf_log_fh);
        return 1;
    }
    if ((ret = kpep_config_force_counters(cfg))) {
        fprintf(kperf_log_fh, "[ERROR-KPerf] Failed to force counters: %d (%s).\n",
               ret, kpep_config_error_desc(ret));
        fflush(kperf_log_fh);
        fclose(kperf_log_fh);
        return 1;
    }
    
    // get events
    const usize ev_count = sizeof(profile_events) / sizeof(profile_events[0]);
    kpep_event *ev_arr[ev_count] = { 0 };
    for (usize i = 0; i < ev_count; i++) {
        const event_alias *alias = profile_events + i;
        ev_arr[i] = get_event(db, alias);
        if (!ev_arr[i]) {
            fprintf(kperf_log_fh, "[ERROR-KPerf] Cannot find event: %s.\n", alias->alias);
            fflush(kperf_log_fh);
            fclose(kperf_log_fh);
            return 1;
        }
    }
    
    // add event to config
    for (usize i = 0; i < ev_count; i++) {
        kpep_event *ev = ev_arr[i];
        if ((ret = kpep_config_add_event(cfg, &ev, 0, NULL))) {
            fprintf(kperf_log_fh, "[ERROR-KPerf] Failed to add event: %d (%s).\n",
                   ret, kpep_config_error_desc(ret));
            fflush(kperf_log_fh);
            fclose(kperf_log_fh);
            return 1;
        }
    }
    
    // prepare buffer and config
    u32 classes = 0;
    usize reg_count = 0;
    kpc_config_t regs[KPC_MAX_COUNTERS] = { 0 };
    usize counter_map[KPC_MAX_COUNTERS] = { 0 };
    u64 counters_0[KPC_MAX_COUNTERS] = { 0 };
    u64 counters_1[KPC_MAX_COUNTERS] = { 0 };
    if ((ret = kpep_config_kpc_classes(cfg, &classes))) {
        fprintf(kperf_log_fh, "[ERROR-KPerf] Failed get kpc classes: %d (%s).\n",
               ret, kpep_config_error_desc(ret));
        fflush(kperf_log_fh);
        fclose(kperf_log_fh);
        return 1;
    }
    if ((ret = kpep_config_kpc_count(cfg, &reg_count))) {
        fprintf(kperf_log_fh, "[ERROR-KPerf] Failed get kpc count: %d (%s).\n",
               ret, kpep_config_error_desc(ret));
        fflush(kperf_log_fh);
        fclose(kperf_log_fh);
        return 1;
    }
    if ((ret = kpep_config_kpc_map(cfg, counter_map, sizeof(counter_map)))) {
        fprintf(kperf_log_fh, "[ERROR-KPerf] Failed get kpc map: %d (%s).\n",
               ret, kpep_config_error_desc(ret));
        fflush(kperf_log_fh);
        fclose(kperf_log_fh);
        return 1;
    }
    if ((ret = kpep_config_kpc(cfg, regs, sizeof(regs)))) {
        fprintf(kperf_log_fh, "[ERROR-KPerf] Failed get kpc registers: %d (%s).\n",
               ret, kpep_config_error_desc(ret));
        fflush(kperf_log_fh);
        fclose(kperf_log_fh);
        return 1;
    }
    
    // set config to kernel
    if ((ret = kpc_force_all_ctrs_set(1))) {
        fprintf(kperf_log_fh, "[ERROR-KPerf] Failed force all ctrs: %d.\n", ret);
        fflush(kperf_log_fh);
        fclose(kperf_log_fh);
        return 1;
    }
    if ((classes & KPC_CLASS_CONFIGURABLE_MASK) && reg_count) {
        if ((ret = kpc_set_config(classes, regs))) {
            fprintf(kperf_log_fh, "[ERROR-KPerf] Failed set kpc config: %d.\n", ret);
            fflush(kperf_log_fh);
            fclose(kperf_log_fh);
            return 1;
        }
    }
    
    // start counting
    if ((ret = kpc_set_counting(classes))) {
        fprintf(kperf_log_fh, "[ERROR-KPerf] Failed set counting: %d.\n", ret);
        fflush(kperf_log_fh);
        fclose(kperf_log_fh);
        return 1;
    }
    if ((ret = kpc_set_thread_counting(classes))) {
        fprintf(kperf_log_fh, "[ERROR-KPerf] Failed set thread counting: %d.\n", ret);
        fflush(kperf_log_fh);
        fclose(kperf_log_fh);
        return 1;
    }
    
    // get counters before
    if ((ret = kpc_get_thread_counters(0, KPC_MAX_COUNTERS, counters_0))) {
        fprintf(kperf_log_fh, "[ERROR-KPerf] Failed get thread counters before: %d.\n", ret);
        fflush(kperf_log_fh);
        fclose(kperf_log_fh);
        return 1;
    }

    profile_func(argc, argv, kperf_log_fh);

    // get counters after
    if ((ret = kpc_get_thread_counters(0, KPC_MAX_COUNTERS, counters_1))) {
        fprintf(kperf_log_fh, "[ERROR-KPerf] Failed get thread counters after: %d.\n", ret);
        fflush(kperf_log_fh);
        fclose(kperf_log_fh);
        return 1;
    }
    
    // stop counting
    kpc_set_counting(0);
    kpc_set_thread_counting(0);
    kpc_force_all_ctrs_set(0);

    // result
    for (usize i = 0; i < ev_count; i++) {
            const event_alias *alias = profile_events + i;
            usize idx = counter_map[i];
            u64 val = counters_1[idx] - counters_0[idx];
            fprintf(kperf_log_fh, "[RESULT-KPerf] %s - %llu\n", alias->alias, val);
    }
    fflush(kperf_log_fh);
    fclose(kperf_log_fh);
    return 0;
}

#ifdef __cplusplus
extern "C"
#endif

void renamed_exit(int res)
{
    // asm volatile("dsb ld" ::: "memory");
    #ifdef __cplusplus
        throw res;
    #else
        return res;
    #endif
}