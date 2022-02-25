#!/usr/bin/env perl

%INTEL_PMU_INFO =
(

####################################################################################################

# Instructions

####################################################################################################

    'cycles' =>
    {
        'universal' =>
        {
            'eventsel'   => '0x3c',
            'umask'      => '0x00',
            'cmask'      => ''
        },

        'group' => 'instruction general'
    },

    'inst_retired.any' =>
    {
        'universal' =>
        {
            'eventsel'   => '0xc0',
            'umask'      => '0x00',
            'cmask'      => ''
        },
        
        'group' => 'instruction general'
    },

    'uops_retired.retire_slots' =>
    {
        'universal' =>
        {
            'eventsel'   => '0xc2',
            'umask'      => '0x02',
            'cmask'      => ''
        },
        
        'group' => 'instruction general'
    },

    'uops_executed.x87' =>
    {
        'universal' => 
        {
            'eventsel'   => '0xb1',
            'umask'      => '0x10',
            'cmask'      => ''
        },

        'group' => 'instruction'
    },

    'fp_arith_inst_retired.scalar_double' =>
    {
        'universal' =>
        {
            'eventsel'   => '0xc7',
            'umask'      => '0x01',
            'cmask'      => ''
        },

        'group' => 'instruction'
    },

    'fp_arith_inst_retired.scalar_single' =>
    {
        'universal' =>
        {
            'eventsel'   => '0xc7',
            'umask'      => '0x02',
            'cmask'      => ''
        },

        'group' => 'instruction'
    },

    'fp_arith_inst_retired.128b_packed_double' =>
    {
        'universal' =>
        {
            'eventsel'   => '0xc7',
            'umask'      => '0x04',
            'cmask'      => ''
        },

        'group' => 'instruction'
    },

    'fp_arith_inst_retired.128b_packed_single' =>
    {
        'universal' =>
        {
            'eventsel'   => '0xc7',
            'umask'      => '0x08',
            'cmask'      => ''
        },

        'group' => 'instruction'
    },

    'fp_arith_inst_retired.256b_packed_double' =>
    {
        'universal' =>
        {
            'eventsel'   => '0xc7',
            'umask'      => '0x10',
            'cmask'      => ''
        },

        'group' => 'instruction'
    },

    'fp_arith_inst_retired.256b_packed_single' =>
    {
        'universal' =>
        {
            'eventsel'   => '0xc7',
            'umask'      => '0x20',
            'cmask'      => ''
        },

        'group' => 'instruction'
    },

    'br_inst_retired.all_branches' =>
    {
        'universal' =>
        {
            'eventsel'   => '0xc4',
            'umask'      => '0x00',
            'cmask'      => ''
        },

        'group' => 'instruction'
    },

    'mem_inst_retired.all_loads' =>
    {
        'universal' =>
        {
            'eventsel'   => '0xd0',
            'umask'      => '0x81',
            'cmask'      => ''
        },

        'group' => 'instruction general'
    },

    'mem_inst_retired.all_stores' =>
    {
        'universal' =>
        {
            'eventsel'   => '0xd0',
            'umask'      => '0x82',
            'cmask'      => ''
        },

        'group' => 'instruction general'
    },

####################################################################################################

# Speculation

####################################################################################################

    'uops_issued.any' =>
    {
        'sandybridge haswell skylake sunnycove' =>
        {
            'eventsel'   => '0x0e',
            'umask'      => '0x01',
            'cmask'      => ''
        },

        'goldencove' =>
        {
            'eventsel'   => '0xae',
            'umask'      => '0x01',
            'cmask'      => ''
        },
        
        'group' => 'speculation general'
    },

    'uops_executed.thread' =>
    {
        'universal' =>
        {
            'eventsel'   => '0xb1',
            'umask'      => '0x01',
            'cmask'      => ''
        },

        'group' => 'speculation'
    },

    'baclears.any' =>
    {
        'sandybridge haswell' =>
        {
            'eventsel'   => '0xe6',
            'umask'      => '0x1f',
            'cmask'      => ''
        },

        'skylake sunnycove goldencove' =>
        {
            'eventsel'   => '0xe6',
            'umask'      => '0x01',
            'cmask'      => ''
        },
        
        'group' => 'speculation'
    },

    'br_inst_retired.conditional' =>
    {
        'sandybridge haswell skylake' =>
        {
            'eventsel'   => '0xc4',
            'umask'      => '0x01',
            'cmask'      => ''
        },

        'sunnycove goldencove' =>
        {
            'eventsel'   => '0xc4',
            'umask'      => '0x11',
            'cmask'      => ''
        },

        'group' => 'speculation'
    },

    'br_inst_retired.near_call' =>
    {
        'universal' =>
        {
            'eventsel'   => '0xc4',
            'umask'      => '0x02',
            'cmask'      => ''
        },

        'group' => 'speculation'
    },

    'br_inst_retired.near_return' =>
    {
        'universal' =>
        {
            'eventsel'   => '0xc4',
            'umask'      => '0x08',
            'cmask'      => ''
        },

        'group' => 'speculation'
    },

    'br_misp_retired.all_branches' =>
    {
        'universal' =>
        {
            'eventsel'   => '0xc5',
            'umask'      => '0x00',
            'cmask'      => ''
        },

        'group' => 'speculation'
    },

    'br_misp_retired.conditional' =>
    {
        'sandybridge haswell skylake' =>
        {
            'eventsel'   => '0xc5',
            'umask'      => '0x01',
            'cmask'      => ''
        },

        'sunnycove goldencove' =>
        {
            'eventsel'   => '0xc5',
            'umask'      => '0x11',
            'cmask'      => ''
        },

        'group' => 'speculation'
    },

    'br_misp_retired.near_call' =>
    {
        'universal' =>
        {
            'eventsel'   => '0xc5',
            'umask'      => '0x02',
            'cmask'      => ''
        },

        'group' => 'speculation'
    },

    'int_misc.recovery_cycles' =>
    {
        'sandybridge haswell skylake sunnycove' =>
        {
            'eventsel'   => '0x0d',
            'umask'      => '0x01',
            'cmask'      => ''
        },

        'goldencove' =>
        {
            'eventsel'   => '0xad',
            'umask'      => '0x01',
            'cmask'      => ''
        },

        'group' => 'speculation'
    },

    'int_misc.clear_resteer_cycles' =>
    {
        'sandybridge haswell skylake sunnycove' =>
        {
            'eventsel'   => '0x0d',
            'umask'      => '0x80',
            'cmask'      => ''
        },

        'goldencove' =>
        {
            'eventsel'   => '0xad',
            'umask'      => '0x80',
            'cmask'      => ''
        },

        'group' => 'speculation'
    },

    'machine_clears.count' =>
    {
        'universal' =>
        {
            'eventsel'   => '0xc3',
            'umask'      => '0x01',
            'cmask'      => '1'
        },

        'group' => 'speculation'
    },

    'machine_clears.memory_ordering' =>
    {
        'universal' =>
        {
            'eventsel'   => '0xc3',
            'umask'      => '0x02',
            'cmask'      => ''
        },

        'group' => 'speculation'
    },

    'machine_clears.smc' =>
    {
        'universal' =>
        {
            'eventsel'   => '0xc3',
            'umask'      => '0x04',
            'cmask'      => ''
        },

        'group' => 'speculation'
    },

####################################################################################################

# Front-End

####################################################################################################

    'idq_uops_not_delivered.core' =>
    {
        'universal' =>
        {
            'eventsel'   => '0x9c',
            'umask'      => '0x01',
            'cmask'      => ''
        },

        'group' => 'front-end general'
    },

    'idq_uops_not_delivered.cycles_0_uops_deliv.core' =>
    {
        'sandybridge haswell skylake' =>
        {
            'eventsel'   => '0x9c',
            'umask'      => '0x01',
            'cmask'      => '4'
        },

        'sunnycove' =>
        {
            'eventsel'   => '0x9c',
            'umask'      => '0x01',
            'cmask'      => '5'
        },

        'goldencove' =>
        {
            'eventsel'   => '0x9c',
            'umask'      => '0x01',
            'cmask'      => '6'
        },

        'group' => 'front-end'
    },

    'inst.decoded.decoders' =>
    {
        'goldencove' =>
        {
            'eventsel'   => '0x75',
            'umask'      => '0x01',
            'cmask'      => ''
        },

        # officially available since skylake
        'sunnycove skylake haswell sandybridge' =>
        {
            'eventsel'   => '0x55',
            'umask'      => '0x01',
            'cmask'      => ''
        },

        'group' => 'front-end'
    },

    'idq.mite_cycles_any' =>
    {
        'universal' =>
        {
            'eventsel'   => '0x79',
            'umask'      => '0x04',
            'cmask'      => '1'
        },

        'group' => 'front-end'
    },

    'idq.mite_uops' =>
    {
        'universal' =>
        {
            'eventsel'   => '0x79',
            'umask'      => '0x04',
            'cmask'      => ''
        },

        'group' => 'front-end'
    },

    'idq.dsb_cycles_any' =>
    {
        'universal' =>
        {
            'eventsel'   => '0x79',
            'umask'      => '0x08',
            'cmask'      => '1'
        },

        'group' => 'front-end'
    },

    'idq.dsb_uops' =>
    {
        'universal' =>
        {
            'eventsel'   => '0x79',
            'umask'      => '0x08',
            'cmask'      => ''
        },

        'group' => 'front-end'
    },

    'lsd.uops' =>
    {
        'universal' =>
        {
            'eventsel'   => '0xa8',
            'umask'      => '0x01',
            'cmask'      => ''
        },

        'group' => 'front-end'
    },

    'idq.ms_uops' =>
    {
        'sandybridge haswell skylake sunnycove' =>
        {
            'eventsel'   => '0x79',
            'umask'      => '0x30',
            'cmask'      => ''
        },

        'goldencove' =>
        {
            'eventsel'   => '0x79',
            'umask'      => '0x20',
            'cmask'      => ''
        },

        'group' => 'front-end'
    },

    # 'icache.hit' =>
    # {
    #     # officially available since haswell 
    #     'universal' =>
    #     {
    #         'eventsel'   => '0x80',
    #         'umask'      => '0x01',
    #         'cmask'      => ''
    #     },

    #     'group' => 'front-end'
    # },

    'icache.misses' =>
    {
        # haswell / goldencove has this counter, while sandybridge / skylake / sunnycove doesn't
        'universal' =>
        {
            'eventsel'   => '0x80',
            'umask'      => '0x02',
            'cmask'      => ''
        },

        'group' => 'front-end'
    },

    'icache_accesses' =>
    {
        # haswell / goldencove has this counter, while sandybridge / skylake / sunnycove doesn't
        'universal' =>
        {
            'eventsel'   => '0x80',
            'umask'      => '0x03',
            'cmask'      => ''
        },

        'group' => 'front-end'
    },

    'icache_data.stalls' =>
    {
        # haswell / goldencove has this counter, while sandybridge / skylake / sunnycove doesn't
        'universal' =>
        {
            'eventsel'   => '0x80',
            'umask'      => '0x04',
            'cmask'      => ''
        },

        'group' => 'front-end'
    },

    'icache_tag.stalls' =>
    {
        # haswell / goldencove has this counter, while sandybridge / skylake / sunnycove doesn't
        'universal' =>
        {
            'eventsel'   => '0x83',
            'umask'      => '0x04',
            'cmask'      => ''
        },

        'group' => 'front-end'
    },

    'dsb2mite_switches.penalty_cycles' =>
    {
        # haswell / goldencove has this counter, while sandybridge / skylake / sunnycove doesn't
        'sandybridge haswell skylake sunnycove' =>
        {
            'eventsel'   => '0xab',
            'umask'      => '0x02',
            'cmask'      => ''
        },

        'goldencove' =>
        {
            'eventsel'   => '0x61',
            'umask'      => '0x02',
            'cmask'      => ''
        },

        'group' => 'front-end'
    },

    'itlb_misses.stlb_hit' =>
    {
        # haswell / goldencove has this counter, while sandybridge / skylake / sunnycove doesn't
        'sandybridge' =>
        {
            'eventsel'   => '0x85',
            'umask'      => '0x10',
            'cmask'      => ''
        },

        'haswell' =>
        {
            'eventsel'   => '0x85',
            'umask'      => '0x60',
            'cmask'      => ''
        },

        'skylake sunnycove' =>
        {
            'eventsel'   => '0x85',
            'umask'      => '0x20',
            'cmask'      => ''
        },

        'goldencove' =>
        {
            'eventsel'   => '0x11',
            'umask'      => '0x20',
            'cmask'      => ''
        },

        'group' => 'front-end'
    },

    'itlb_misses.walk_completed' =>
    {
        # haswell / goldencove has this counter, while sandybridge / skylake / sunnycove doesn't
        'sandybridge' =>
        {
            'eventsel'   => '0x85',
            'umask'      => '0x02',
            'cmask'      => ''
        },

        'haswell skylake sunnycove' =>
        {
            'eventsel'   => '0x85',
            'umask'      => '0x0e',
            'cmask'      => ''
        },

        'goldencove' =>
        {
            'eventsel'   => '0x11',
            'umask'      => '0x0e',
            'cmask'      => ''
        },

        'group' => 'front-end'
    },

####################################################################################################

# Back-End

####################################################################################################
    
    'mem_load_retired.fb_hit' =>
    {
        'universal' =>
        {
            'eventsel'   => '0xd1',
            'umask'      => '0x40',
            'cmask'      => ''
        },

        'group' => 'back-end general'
    },

    'mem_load_retired.l1_hit' =>
    {
        'universal' =>
        {
            'eventsel'   => '0xd1',
            'umask'      => '0x01',
            'cmask'      => ''
        },

        'group' => 'back-end general'
    },

    'mem_load_retired.l1_miss' =>
    {
        'universal' =>
        {
            'eventsel'   => '0xd1',
            'umask'      => '0x08',
            'cmask'      => ''
        },

        'group' => 'back-end general'
    },

    'mem_load_retired.l2_miss' =>
    {
        'universal' =>
        {
            'eventsel'   => '0xd1',
            'umask'      => '0x10',
            'cmask'      => ''
        },

        'group' => 'back-end general'
    },

    'mem_load_retired.l3_miss' =>
    {
        'universal' =>
        {
            'eventsel'   => '0xd1',
            'umask'      => '0x20',
            'cmask'      => ''
        },

        'group' => 'back-end general'
    },

    'resource_stalls.any' =>
    {
        'universal' =>
        {
            'eventsel'   => '0xa2',
            'umask'      => '0x01',
            'cmask'      => ''
        },

        'group' => 'back-end'
    },

    'exe_activity.bound_on_stores' =>
    {
        'sunnycove goldencove' =>
        {
            'eventsel'   => '0xa6',
            'umask'      => '0x40',
            'cmask'      => '2'
        },

        'sandybridge haswell skylake' =>
        {
            'eventsel'   => '0xa6',
            'umask'      => '0x40',
            'cmask'      => ''
        },

        'group' => 'back-end'
    },

    'l1d_pending_miss.pending' =>
    {
        'universal' =>
        {
            'eventsel'   => '0x48',
            'umask'      => '0x01',
            'cmask'      => ''
        },

        'group' => 'back-end'
    },

    'l1d_pending_miss.fb_full' =>
    {
        'skylake sunnycove goldencove' =>
        {
            'eventsel'   => '0x48',
            'umask'      => '0x02',
            'cmask'      => ''
        },

        'sandybridge haswell' =>
        {
            'eventsel'   => '0x48',
            'umask'      => '0x02',
            'cmask'      => '1'
        },

        'group' => 'back-end'
    },

    'mem_inst_retired.stlb_miss_loads' =>
    {
        'universal' =>
        {
            'eventsel'   => '0xd0',
            'umask'      => '0x11',
            'cmask'      => ''
        },

        'group' => 'back-end general'
    },

    'dtlb_misses.stlb_hit' =>
    {
        'sandybridge' =>
        {
            'eventsel'   => '0x08',
            'umask'      => '0x10',
            'cmask'      => ''
        },

        'haswell' =>
        {
            'eventsel'   => '0x08',
            'umask'      => '0x60',
            'cmask'      => ''
        },

        'skylake sunnycove' =>
        {
            'eventsel'   => '0x08',
            'umask'      => '0x20',
            'cmask'      => ''
        },

        'goldencove' =>
        {
            'eventsel'   => '0x12',
            'umask'      => '0x20',
            'cmask'      => ''
        },

        'group' => 'back-end general'
    },

    'dtlb_load_misses.walk_completed' =>
    {
        'sandybridge' =>
        {
            'eventsel'   => '0x08',
            'umask'      => '0x02',
            'cmask'      => ''
        },

        'haswell skylake sunnycove' =>
        {
            'eventsel'   => '0x08',
            'umask'      => '0x0e',
            'cmask'      => ''
        },

        'goldencove' =>
        {
            'eventsel'   => '0x12',
            'umask'      => '0x0e',
            'cmask'      => ''
        },

        'group' => 'back-end general'
    },

    'cycle_activity.stalls_l1d_miss' =>
    {
        'sandybridge haswell skylake sunnycove' =>
        {
            'eventsel'   => '0xa3',
            'umask'      => '0x0c',
            'cmask'      => '12'
        },

        'goldencove' =>
        {
            'eventsel'   => '0x47',
            'umask'      => '0x03',
            'cmask'      => '3'
        },

        'group' => 'back-end'
    },

    'cycle_activity.stalls_l2_miss' =>
    {
        'sandybridge haswell skylake sunnycove' =>
        {
            'eventsel'   => '0xa3',
            'umask'      => '0x05',
            'cmask'      => '5'
        },

        'goldencove' =>
        {
            'eventsel'   => '0x47',
            'umask'      => '0x05',
            'cmask'      => '5'
        },

        'group' => 'back-end'
    },

    # officially available since skylake
    'cycle_activity.stalls_l3_miss' =>
    {
        'sandybridge haswell skylake sunnycove' =>
        {
            'eventsel'   => '0xa3',
            'umask'      => '0x06',
            'cmask'      => '6'
        },

        'goldencove' =>
        {
            'eventsel'   => '0x47',
            'umask'      => '0x09',
            'cmask'      => '9'
        },

        'group' => 'back-end'
    },

    # goldencove officially doesn't have this counter
    'cycle_activity.stalls_mem_any' =>
    {
        'universal' =>
        {
            'eventsel'   => '0xa3',
            'umask'      => '0x14',
            'cmask'      => '20'
        },

        'group' => 'back-end'
    },

####################################################################################################

# Complementary

####################################################################################################
    
    'rs_events.empty_cycles' =>
    {
        'universal' =>
        {
            'eventsel'   => '0x5e',
            'umask'      => '0x01',
            'cmask'      => ''
        },

        'group' => 'complementary'
    },

    'l1d.replacement' =>
    {
        'universal' =>
        {
            'eventsel'   => '0x51',
            'umask'      => '0x01',
            'cmask'      => ''
        },

        'group' => 'complementary general'
    },

    'page_walker_loads.itlb_l1' =>
    {
        'universal' =>
        {
            'eventsel'   => '0xbc',
            'umask'      => '0x21',
            'cmask'      => ''
        },

        'group' => 'complementary'
    },

    'page_walker_loads.itlb_l2' =>
    {
        'universal' =>
        {
            'eventsel'   => '0xbc',
            'umask'      => '0x22',
            'cmask'      => ''
        },

        'group' => 'complementary'
    },

    'page_walker_loads.itlb_l3' =>
    {
        'universal' =>
        {
            'eventsel'   => '0xbc',
            'umask'      => '0x24',
            'cmask'      => ''
        },

        'group' => 'complementary'
    },

    'page_walker_loads.itlb_mem' =>
    {
        'universal' =>
        {
            'eventsel'   => '0xbc',
            'umask'      => '0x28',
            'cmask'      => ''
        },

        'group' => 'complementary'
    },

    'page_walker_loads.dtlb_l1' =>
    {
        'universal' =>
        {
            'eventsel'   => '0xbc',
            'umask'      => '0x11',
            'cmask'      => ''
        },

        'group' => 'complementary'
    },

    'page_walker_loads.dtlb_l2' =>
    {
        'universal' =>
        {
            'eventsel'   => '0xbc',
            'umask'      => '0x12',
            'cmask'      => ''
        },

        'group' => 'complementary'
    },

    'page_walker_loads.dtlb_l3' =>
    {
        'universal' =>
        {
            'eventsel'   => '0xbc',
            'umask'      => '0x14',
            'cmask'      => ''
        },

        'group' => 'complementary'
    },

    'page_walker_loads.dtlb_mem' =>
    {
        'universal' =>
        {
            'eventsel'   => '0xbc',
            'umask'      => '0x18',
            'cmask'      => ''
        },

        'group' => 'complementary'
    }
)
