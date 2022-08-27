#!/usr/bin/env perl -w

package Veronica::Profiling;

use v5.10;
use strict;
use warnings;
use File::Basename;
use File::Copy qw(move);
use Cwd 'abs_path';
use Carp qw/cluck/;
use Time::HiRes qw(gettimeofday tv_interval);

use lib "$ENV{'VERONICA'}/perl";
use Veronica::Common;
use Veronica::Threads;
use Veronica::System;

sub macosx_powermetrics_thread_launch
{
    my ($flag_file, $result_log_file) = @_;
    #my ($flag_file, $result_log_file) = split '--pobu--', $input_string;
    my $sampling_time_in_ms = 1000;
    my $sampling_count      = 1;
    
    `touch $flag_file`;
    system "rm $result_log_file" if -e $result_log_file;
    
    while(-e $flag_file)
    {
        system "sudo /usr/bin/powermetrics -s cpu_power -i $sampling_time_in_ms -n $sampling_count >> $result_log_file";
    }

    return 0;
}

sub macosx_powermetrics_begin
{
    my ($flag_file, $result_log_file) = @_;
    my $os_type    = Veronica::System::get_os_type();
    my $status     = 0;
    if($os_type eq 'MACOSX')
    {
        # TODO: not binding cores
        $status = Veronica::Threads::thread_start(
                    "starting powermetrics ...",
                    '', \&macosx_powermetrics_thread_launch,
                    $flag_file, $result_log_file);
        
        return $status if $status != 0; 
        
        # wait till the powermetrics begins to dump
        while(!-e $result_log_file){};
    }
    else
    {
        Veronica::Common::log_level("Unmatched OS type $os_type for powermetrics\n", -1);
    }
    return $status;
}

sub macosx_powermetrics_end
{
    my ($flag_file) = @_;
    my $status = 0;

    if(-e $flag_file)
    {
        `rm $flag_file`;
        $status = Veronica::Threads::join_n_thread_with_log(scalar threads->list());
    }
    return $status;
}

sub macosx_get_kperf_filename
{
    return "pobu_kperf";
}

sub macosx_get_kperf_info
{
    my $kperf_dir       = "$ENV{'VERONICA'}/tools/MacOSKperf";
    my $kperf_filename  = (macosx_get_kperf_filename).'.cpp';
    my $full_path       = "$kperf_dir/$kperf_filename";

    if(!-e $full_path)
    {
        Veronica::Common::log_level("$full_path doesn't exist", -1);
    }
    else
    {
        return ($kperf_dir, $kperf_filename);
    }
}

sub macosx_inject_kperf
{
    my @obj_files = @_;
    my $objcopy_exist   = (`which llvm-objcopy` !~ 'not found');
    my $kperf_filename  = macosx_get_kperf_filename();
    my $os_type         = Veronica::System::get_os_type();
    
    if($os_type eq 'MACOSX')
    {
        if(!$objcopy_exist)
        {
            Veronica::Common::log_level("llvm-objcopy doesn't exist", -1);
        } 
        else
        {
            Veronica::Common::log_level("\n", 0);
            foreach my $obj (@obj_files)
            {
                if($obj !~ $kperf_filename)
                {
                    Veronica::Common::log_level("processing $obj", 5);
                    system "llvm-objcopy --redefine-sym _main=_renamed_main $obj";
                    system "llvm-objcopy --redefine-sym _exit=_renamed_exit $obj";
                }
            }
            
        }
    }
}
1;