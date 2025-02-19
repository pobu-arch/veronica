#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <dlfcn.h>
#include "pthread/qos.h"
#include "pobu_kperf.hpp"
//#include "pobu_kperf_patch.hpp"

// -----------------------------------------------------------------------------
// Demo
// -----------------------------------------------------------------------------

#define PERFORMANCE

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
    {   "cycles", {
            "FIXED_CYCLES"                 // Apple A7-A15
            //"CPU_CLK_UNHALTED.THREAD",   // Intel Core 1th-10th
            //"CPU_CLK_UNHALTED.CORE",     // Intel Yonah, Merom
    }},
    {   "instructions", {
            "FIXED_INSTRUCTIONS"           // Apple A7-A15
           // "INST_RETIRED.ANY"           // Intel Yonah, Merom, Core 1th-10th
    }},
    
// Configurable counters start here
    
    // Instruction
    {   "branches.retired", {
            "INST_BRANCH"                            // Apple, 3-bit
    }},
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
    {   "uops_retired", {
            "RETIRE_UOP"                             // Apple, 1-bit
    }},
    
    // Speculation
    {   "uops_issued", {
            "SCHEDULE_UOP"                          // Apple
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
    {   "map_rewinding_cycles", {
            "MAP_REWIND"                            // Apple
    }},
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
    {   "l1i_cache_demand_misses", {
            "L1I_CACHE_MISS_DEMAND"                 // Apple
    }},
    // {   "l1i_tlb_refills", {
    //         "L1I_TLB_FILL"                          // Apple
    // }},
    {   "l1i_tlb_demand_misses", {
            "L1I_TLB_MISS_DEMAND"                   // Apple
    }},
    {   "l2_tlb_misses_inst", {
            "L2_TLB_MISS_INSTRUCTION"               // Apple
    }},
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
    // {   "core_active_cycles", {
    //         "CORE_ACTIVE_CYCLE"                     // Apple
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

int profile_func(int argc, char ** argv) {
    // if one changes this function name 'renamed_main' below
    // then the helper function within the benchmarks repo should be changed correspondingly
    int res;
    try {
        res = renamed_main(argc, argv);
    }catch(int res){
        printf("[kperf-warning] return code profile_func from profile func is %d\n", res);
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

    printf("\n");
    
    // load dylib
    if (!lib_init()) {
        printf("[kperf-error] %s\n", lib_err_msg);
        return 1;
    }
    
    // get PMU version
    // check permission
    int force_ctrs = 0;
    if (kpc_force_all_ctrs_get(&force_ctrs)) {
        printf("[kperf-error] Permission denied, xnu/kpc requires root privileges.\n");
        return 1;
    }
    
    // load pmc db
    kpep_db *db = NULL;
    if ((ret = kpep_db_create(NULL, &db))) {
        printf("[kperf-error] cannot load pmc database: %d.\n", ret);
        return 1;
    }
    printf("[kperf-info] loaded db: %s (%s)\n", db->name, db->marketing_name);
    printf("[kperf-info] number of fixed counters: %zu\n", db->fixed_counter_count);
    printf("[kperf-info] number of configurable counters: %zu\n", db->config_counter_count);
    
    // create a config
    kpep_config *cfg = NULL;
    if ((ret = kpep_config_create(db, &cfg))) {
        printf("[kperf-error] failed to create kpep config: %d (%s).\n",
               ret, kpep_config_error_desc(ret));
        return 1;
    }
    if ((ret = kpep_config_force_counters(cfg))) {
        printf("[kperf-error] failed to force counters: %d (%s).\n",
               ret, kpep_config_error_desc(ret));
        return 1;
    }
    
    // get events
    const usize ev_count = sizeof(profile_events) / sizeof(profile_events[0]);
    kpep_event *ev_arr[ev_count] = { 0 };
    for (usize i = 0; i < ev_count; i++) {
        const event_alias *alias = profile_events + i;
        ev_arr[i] = get_event(db, alias);
        if (!ev_arr[i]) {
            printf("[kperf-error] cannot find event: %s.\n", alias->alias);
            return 1;
        }
    }
    
    // add event to config
    for (usize i = 0; i < ev_count; i++) {
        kpep_event *ev = ev_arr[i];
        if ((ret = kpep_config_add_event(cfg, &ev, 0, NULL))) {
            printf("[kperf-error] failed to add event: %d (%s).\n",
                   ret, kpep_config_error_desc(ret));
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
        printf("[kperf-error] failed get kpc classes: %d (%s).\n",
               ret, kpep_config_error_desc(ret));
        return 1;
    }
    if ((ret = kpep_config_kpc_count(cfg, &reg_count))) {
        printf("[kperf-error] failed get kpc count: %d (%s).\n",
               ret, kpep_config_error_desc(ret));
        return 1;
    }
    if ((ret = kpep_config_kpc_map(cfg, counter_map, sizeof(counter_map)))) {
        printf("[kperf-error] failed get kpc map: %d (%s).\n",
               ret, kpep_config_error_desc(ret));
        return 1;
    }
    if ((ret = kpep_config_kpc(cfg, regs, sizeof(regs)))) {
        printf("[kperf-error] failed get kpc registers: %d (%s).\n",
               ret, kpep_config_error_desc(ret));
        return 1;
    }
    
    // set config to kernel
    if ((ret = kpc_force_all_ctrs_set(1))) {
        printf("[kperf-error] failed force all ctrs: %d.\n", ret);
        return 1;
    }
    if ((classes & KPC_CLASS_CONFIGURABLE_MASK) && reg_count) {
        if ((ret = kpc_set_config(classes, regs))) {
            printf("[kperf-error] failed set kpc config: %d.\n", ret);
            return 1;
        }
    }
    
    // start counting
    if ((ret = kpc_set_counting(classes))) {
        printf("[kperf-error] failed set counting: %d.\n", ret);
        return 1;
    }
    if ((ret = kpc_set_thread_counting(classes))) {
        printf("[kperf-error] failed set thread counting: %d.\n", ret);
        return 1;
    }

    #if defined EFFICIENCY
        int err = pthread_set_qos_class_self_np(QOS_CLASS_BACKGROUND, 0);
    #elif defined PERFORMANCE
        int err = pthread_set_qos_class_self_np(QOS_CLASS_USER_INTERACTIVE, 0);
    #endif
    if (err) {
        printf("[kperf-error] qos class setup error %d\n", err);
        return 1;
    }

    // get counters before
    if ((ret = kpc_get_thread_counters(0, KPC_MAX_COUNTERS, counters_0))) {
        printf("[kperf-error] failed get thread counters before: %d.\n", ret);
        return 1;
    }

    profile_func(argc, argv);

    // get counters after
    if ((ret = kpc_get_thread_counters(0, KPC_MAX_COUNTERS, counters_1))) {
        printf("[kperf-error] failed get thread counters after: %d.\n", ret);
        return 1;
    }
    
    // stop counting
    kpc_set_counting(0);
    kpc_set_thread_counting(0);
    kpc_force_all_ctrs_set(0);
    
    // result
    printf("\n");
    for (usize i = 0; i < ev_count; i++) {
        const event_alias *alias = profile_events + i;
        usize idx = counter_map[i];
        u64 val = counters_1[idx] - counters_0[idx];
        printf("[kperf-result] %s - %llu\n", alias->alias, val);
    }
    printf("\n");
    
    return 0;
}
