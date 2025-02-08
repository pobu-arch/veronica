#!/usr/bin/env perl

use v5.10;
use strict;
use warnings;
use File::Find;
use File::Copy;
use Time::HiRes qw(gettimeofday tv_interval);

use lib "$ENV{'VERONICA'}/perl";
use Veronica::Common;
use Veronica::Threads;

our $THIS_DIR       = Veronica::Common::get_script_path();
our $COUNTER_GROUP  = 'general';
our $MICROARCH      = '';

our %INTEL_PMU_INFO;
require "$THIS_DIR/intel_pmu_info.pl";
our @INTEL_MICRO_ARCH_ARRAY = ('sandybridge', 'haswell', 'skylake', 'sunnycove', 'goldencove');

&cmd_parse();
&cmd_gen();

####################################################################################################

# Command-Line Parsing

####################################################################################################

sub cmd_parse()
{
    Veronica::Common::log_level("parsing arguments ...", 3);

    if(@ARGV < 1)
    {
        Veronica::Common::log_level('no enough parameters for this script', -1);
    }

    foreach my $current_arg (@ARGV)
    {
        if($current_arg =~ /-microarch=(?<microarch>\w+)/)
        {
            $MICROARCH = $+{microarch};
        }
        elsif($current_arg =~ /-group=(?<group>\w+)/)
        {
            $COUNTER_GROUP = $+{group};
        }
        else
        {
            Veronica::Common::log_level("unknown parameters $current_arg", -1);
        }
    }

    Veronica::Common::log_level("please specify the microarchitecture", -1) if $MICROARCH eq '';

    Veronica::Common::log_level("the selected microarchitecture is $MICROARCH".
                                ", metrics group is $COUNTER_GROUP", 3);
}


####################################################################################################

# Perf Command Generation

####################################################################################################

sub verify_intel_pmu_info
{
    my @event_name_array = keys %INTEL_PMU_INFO;
    my %verify_info = ();
    foreach my $event_name (sort @event_name_array)
    {
        my @event_microarch_array = keys %{$INTEL_PMU_INFO{$event_name}};
        foreach my $intel_microarch (@INTEL_MICRO_ARCH_ARRAY)
        {
            foreach my $event_microarch (@event_microarch_array)
            {
                if($event_microarch eq 'universal' or $event_microarch =~ $intel_microarch)
                {
                    $verify_info{$intel_microarch}++   if exists $verify_info{$intel_microarch};
                    $verify_info{$intel_microarch} = 1 if !exists $verify_info{$intel_microarch};
                    last;
                }
            }    
        }

        foreach my $intel_microarch (@INTEL_MICRO_ARCH_ARRAY)
        {
            if(!exists $verify_info{$intel_microarch} or
                       $verify_info{$intel_microarch} != 1)
            {
                Veronica::Common::log_level("intel PMU info error at $event_name $intel_microarch", -1);
            }
            delete $verify_info{$intel_microarch};
        }
    }
}

sub identify_microarch
{
    &verify_intel_pmu_info();
    foreach my $intel_microarch (@INTEL_MICRO_ARCH_ARRAY)
    {
        return 'intel' if $intel_microarch eq $MICROARCH;
    }
    
    Veronica::Common::log_level("unsupported microarchtecture $MICROARCH", -1);
}

sub cmd_gen
{
    my $vendor = &identify_microarch();

    my @event_name_array = keys %INTEL_PMU_INFO if $vendor eq 'intel';
    my @sample_queue;

    my $perf_cmd        = 'sudo perf stat -e ';
    my $perf_prefix     = $MICROARCH eq 'goldencove' ? 'cpu_core' : 'cpu';
    my $event_count     = 0;

    foreach my $event_name (sort @event_name_array)
    {
        my $group           = $INTEL_PMU_INFO{$event_name}{'group'};
        my $microarch_match = '';

        my @microarch_array = keys %{$INTEL_PMU_INFO{$event_name}};
        foreach my $microarch (@microarch_array)
        {
            if($microarch eq 'universal' or $microarch =~ $MICROARCH)
            {
                $microarch_match = $microarch;
                last;
            }
        }

        if($microarch_match ne '' and $group =~ $COUNTER_GROUP)
        {
            my $eventsel        = $INTEL_PMU_INFO{$event_name}{$microarch_match}{'eventsel'};
            my $umask           = $INTEL_PMU_INFO{$event_name}{$microarch_match}{'umask'};
            my $cmask           = $INTEL_PMU_INFO{$event_name}{$microarch_match}{'cmask'};
            
            my $event_string    = "event=$eventsel,umask=$umask".
                                  ($cmask ne '' ? ",cmask=$cmask" : '').
                                  ",name=$event_name";
            
            $perf_cmd .= ',' if $event_count;
            $perf_cmd .= $perf_prefix."\/".$event_string."\/";
            $event_count++;
        }
    }

    Veronica::Common::log_level("It has $event_count counters in total", 3);
    Veronica::Common::log_level("\n", 0);
    Veronica::Common::log_level($perf_cmd, 0);
    Veronica::Common::log_level("\n", 0);
}

