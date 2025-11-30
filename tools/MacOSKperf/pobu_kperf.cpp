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

    // -------------------------------------------------------------------------
    // Fixed counters
    // -------------------------------------------------------------------------
    //{ "instructions",        { "FIXED_INSTRUCTIONS" }},
    // { "cycles",              { "FIXED_CYCLES" }},

    // -------------------------------------------------------------------------
    // Configurable counters
    // -------------------------------------------------------------------------
    { "core_active_cycles",                  { "CORE_ACTIVE_CYCLE" }},

    // -------------------------------------------------------------------------
    // ARM Stall
    // -------------------------------------------------------------------------
    // { "arm_stall",                               { "ARM_STALL" }},
     { "arm_stall.frontend",                      { "ARM_STALL_FRONTEND" }},
     { "arm_stall.backend",                       { "ARM_STALL_BACKEND" }},
    // { "arm_stall_slot",                          { "ARM_STALL_SLOT" }},
    // { "arm_stall_slot.frontend",                 { "ARM_STALL_SLOT_FRONTEND" }},
    // { "arm_stall_slot.backend",                  { "ARM_STALL_SLOT_BACKEND" }},
    // { "map_idle",                                { "MAP_DISPATCH_BUBBLE" }},
    // { "map_stall_cycles",                        { "MAP_STALL" }},
    // { "map_stalls.dispatch",                     { "MAP_STALL_DISPATCH" }},
    // { "map_stalls.l1i",                          { "MAP_DISPATCH_BUBBLE_IC" }},
    // { "map_stalls.itlb",                         { "MAP_DISPATCH_BUBBLE_ITLB" }},
    // { "map_stalls.slot",                         { "MAP_DISPATCH_BUBBLE_SLOT" }},
    // { "map_stalls.recovery",                     { "MAP_RECOVERY" }},
    // { "map_stalls.nonrecovery",                  { "MAP_STALL_NONRECOVERY" }},
    // { "schedule.empty",                          { "SCHEDULE_EMPTY" }},
    // { "schedule_duty",                           { "SCHEDULE_UOP_ANY" }},

    // -------------------------------------------------------------------------
    // Instructions 
    // -------------------------------------------------------------------------
      { "insts_retired",                      { "INST_ALL" }},        // counters_mask = 252
    // { "uops_retired",                       { "RETIRE_UOP" }},      // counters_mask = 128
    // { "branches.retired",                   { "INST_BRANCH" }},     // counters_mask = 252
    // { "int_alu_insts.retired",              { "INST_INT_ALU" }},             // counters_mask = 128
    // { "int_load_insts.retired",             { "INST_INT_LD" }},              // counters_mask = 252
    // { "int_store_insts.retired",            { "INST_INT_ST" }},              // counters_mask = 128
    // { "load_store_insts.retired",           { "INST_LDST" }},                // counters_mask = 128
    // { "simd_alu_insts.retired",             { "INST_SIMD_ALU" }},            // counters_mask = 128
    // { "simd_loads.retired",                 { "INST_SIMD_LD" }},             // counters_mask = 252
    // { "simd_stores.retired",                { "INST_SIMD_ST" }},             // counters_mask = 252
    // { "inst_sme.alu",                       { "INST_SME_ENGINE_ALU" }},      // counters_mask = 252
    // { "inst_sme.st",                        { "INST_SME_ENGINE_ST" }},       // counters_mask = 252
    // { "inst_sme.ld",                        { "INST_SME_ENGINE_LD" }},       // counters_mask = 252
    // { "inst_sme.scalarfp",                  { "INST_SME_ENGINE_SCALARFP" }}, // counters_mask = 252
    // { "inst_sme.packing_fused",             { "INST_SME_ENGINE_PACKING_FUSED" }},
    // { "inst_simd_alu.vec",                  { "INST_SIMD_ALU_VEC" }},        // counters_mask = 128

    // -------------------------------------------------------------------------
    // Speculation
    // -------------------------------------------------------------------------
    // { "arm_br_pred",                        { "ARM_BR_PRED" }},
    // { "arm_br_mis_pred",                    { "ARM_BR_MIS_PRED" }},
    // { "branches_mispredicted.retired",      { "BRANCH_MISPRED_NONSPEC" }},
    //  { "uops_issued",                        { "MAP_UOP" }},
    // { "map_rewinding_cycles",               { "MAP_REWIND" }},
    // { "branches.cond",                      { "INST_BRANCH_COND" }},         // counters_mask = 252
    // { "branches.taken",                     { "INST_BRANCH_TAKEN" }},        // counters_mask = 252
    // { "branches.call",                      { "INST_BRANCH_CALL" }},         // counters_mask = 252
    // { "branches.ret",                       { "INST_BRANCH_RET" }},          // counters_mask = 252
    // { "branches.indir",                     { "INST_BRANCH_INDIR" }},        // counters_mask = 252
    // { "mispred.cond",                       { "BRANCH_COND_MISPRED_NONSPEC" }}, // counters_mask = 252
    // { "mispred.call.indir",                 { "BRANCH_CALL_INDIR_MISPRED_NONSPEC" }}, // counters_mask = 252
    // { "mispred.ret.indir",                  { "BRANCH_RET_INDIR_MISPRED_NONSPEC" }},  // counters_mask = 252
    // { "mispred.indir",                      { "BRANCH_INDIR_MISPRED_NONSPEC" }},      // counters_mask = 252
    // { "map_int_uops.speculative",           { "MAP_INT_UOP" }},
    // { "map_load_store_uops.speculative",    { "MAP_LDST_UOP" }},
    // { "load_uops.speculative",              { "LD_UNIT_UOP" }},
    // { "store_uops.speculative",             { "ST_UNIT_UOP" }},

    // -------------------------------------------------------------------------
    // Frontend
    // -------------------------------------------------------------------------
    // { "fetch_restart_exclude_branch_prediction", { "FETCH_RESTART" }},
    { "l1i_cache_demand_misses",                 { "L1I_CACHE_MISS_DEMAND" }},
    { "l1i_tlb_refills",                         { "L1I_TLB_FILL" }},
    { "l1i_tlb_demand_misses",                   { "L1I_TLB_MISS_DEMAND" }},
    { "l2_tlb_misses_inst",                      { "L2_TLB_MISS_INSTRUCTION" }},
    // { "page_table_walk_inst",                    { "MMU_TABLE_WALK_INSTRUCTION" }},

    // -------------------------------------------------------------------------
    // Backend
    // -------------------------------------------------------------------------
    // { "arm_l1d_cache",                           { "ARM_L1D_CACHE" }},
    // { "arm_l1d_cache.rd",                        { "ARM_L1D_CACHE_RD" }},
    // { "arm_l1d_cache_refill",                    { "ARM_L1D_CACHE_REFILL" }},
    // { "arm_l1d_cache_lmiss.rd",                  { "ARM_L1D_CACHE_LMISS_RD" }},
    // { "l1d_load_misses.retired",                 { "L1D_CACHE_MISS_LD_NONSPEC" }},   // counters_mask = 252
    // { "l1d_load_misses",                         { "L1D_CACHE_MISS_LD" }},
    // { "l1d_store_misses.retired",                { "L1D_CACHE_MISS_ST_NONSPEC" }},   // counters_mask = 252
    // { "l1d_store_misses",                        { "L1D_CACHE_MISS_ST" }},
    // { "l1d_writeback",                           { "L1D_CACHE_WRITEBACK" }},
    // { "l1d_tlb_accesses",                        { "L1D_TLB_ACCESS" }},
    // { "l1d_tlb_refills",                         { "L1D_TLB_FILL" }},
    // { "l1d_tlb_misses.retired",                  { "L1D_TLB_MISS_NONSPEC" }},        // counters_mask = 252
    // { "l1d_tlb_misses",                          { "L1D_TLB_MISS" }},
    // { "l2_tlb_misses_data",                      { "L2_TLB_MISS_DATA" }},
    // { "page_table_walk_data",                    { "MMU_TABLE_WALK_DATA" }},
    // { "store_mem_violation.retired",             { "ST_MEMORY_ORDER_VIOLATION_NONSPEC" }},   // counters_mask = 252
    // { "st_mem_order_viol_ld.retired",            { "ST_MEM_ORDER_VIOL_LD_NONSPEC" }}

    // -------------------------------------------------------------------------
    // Others
    // -------------------------------------------------------------------------
    // { "page_fault.retired",                      { "MMU_VIRTUAL_MEMORY_FAULT_NONSPEC" }},
    // { "map_simd_uops.speculative",               { "MAP_SIMD_UOP" }},
    // { "non_temporal_store_uops.speculative",     { "ST_NT_UOP" }},
    // { "non_temporal_load_uops.speculative",      { "LD_NT_UOP" }},
    // { "load_store_uops_cross_64B.retired",       { "LDST_X64_UOP" }},
    // { "load_store_uops_cross_page.retired",      { "LDST_XPG_UOP" }},
    // { "other_flushes",                           { "FLUSH_RESTART_OTHER_NONSPEC" }},
    // { "cycles_with_pending_interrupt",           { "INTERRUPT_PENDING" }},
    // { "barriers.retired",                        { "INST_BARRIER" }},              // counters_mask = 252
    // { "atomic_or_exclusive_successful",          { "ATOMIC_OR_EXCLUSIVE_SUCC" }},
    // { "atomic_or_exclusive_fail",                { "ATOMIC_OR_EXCLUSIVE_FAIL" }},

    // -------------------------------------------------------------------------
    // Load / Store waiting
    // -------------------------------------------------------------------------
    // { "ld_unit.wait_young_l1d_miss",             { "LD_UNIT_WAITING_YOUNG_L1D_CACHE_MISS" }},
    // { "ldst_unit.old_l1d_miss",                  { "LDST_UNIT_OLD_L1D_CACHE_MISS" }},
    // { "ldst_unit.wait_old_l1d_miss",             { "LDST_UNIT_WAITING_OLD_L1D_CACHE_MISS" }},
    // { "ldst_unit.wait_sme_instq_full",           { "LDST_UNIT_WAITING_SME_ENGINE_INST_QUEUE_FULL" }},
    // { "ldst_unit.wait_sme_mem_data",             { "LDST_UNIT_WAITING_SME_ENGINE_MEM_DATA" }},
    // { "st_barrier.blocked_by_sme_ldst",          { "ST_BARRIER_BLOCKED_BY_SME_LDST" }},
    // { "ld_blocked_by_sme_ldst",                  { "LD_BLOCKED_BY_SME_LDST" }},

    // -------------------------------------------------------------------------
    // SME Engine
    // -------------------------------------------------------------------------
    // { "sme_engine.sm_enable",                    { "SME_ENGINE_SM_ENABLE" }},
    // { "sme_engine.sm_za_enable",                 { "SME_ENGINE_SM_ZA_ENABLE" }},
    // { "sme_engine.za_enabled_sm_disabled",       { "SME_ENGINE_ZA_ENABLED_SM_DISABLED" }},

    // -------------------------------------------------------------------------
    // SME uop
    // -------------------------------------------------------------------------
    // { "map_int_sme_uop",                         { "MAP_INT_SME_UOP" }},
    // { "ldst_sme_xpg_uop",                        { "LDST_SME_XPG_UOP" }},
    // { "ldst_sme_pred_inactive",                  { "LDST_SME_PRED_INACTIVE" }},
    // { "ld_sme_normal_uop",                       { "LD_SME_NORMAL_UOP" }},
    // { "ld_sme_nt_uop",                           { "LD_SME_NT_UOP" }},
    // { "st_sme_normal_uop",                       { "ST_SME_NORMAL_UOP" }},
    // { "st_sme_nt_uop",                           { "ST_SME_NT_UOP" }}
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
            fprintf(kperf_log_fh, "[RESULT-KPerf]\t%s\t-\t%llu\n", alias->alias, val);
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