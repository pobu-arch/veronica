#!/usr/bin/env perl -w

package Veronica::System;

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

sub get_os_type
{
    my $result = `uname -a`;
    if($result =~ 'Darwin')
    {
        return 'MACOSX';
    }
    elsif($result =~ 'Linux')
    {
        return 'LINUX';
    }
    else
    {
        log_level('unsupported OS', -1);
    }
}

sub parse_isa_type
{
    my ($string) = @_;

    # detect target ISA type from compiler info
    if($string =~ 'x86_64')
    {
        return 'X86_64';
    }
    elsif($string =~ 'aarch64|ARM64|arm64')
    {
        return 'ARM64';
    }
    elsif($string =~ 'riscv64')
    {
        return 'RISCV64';
    }
    elsif($string =~ 'riscv32')
    {
        return 'RISCV32';
    }
    else
    {
        log_level('unsupported ISA', -1);
    }
}

sub get_host_isa_type
{
    my $result = `uname -a`;
    return parse_isa_type($result);
}

sub get_target_isa_type
{
    my ($compiler) = @_;

    my $result = `$compiler -v 2>&1`;
    my $isa    = '';

    # detect compiler info
    if($result =~ /--target=(?<isa>\w+)/g)
    {
        $isa = $+{isa};
    }
    elsif($result =~ /Target\:\s+(?<isa>\w+)/g)
    {
        $isa = $+{isa};
    }
    else
    {
        log_level("compiler info is : $result", 1);
        log_level('unsupported compiler info format', -1);

    }

    return parse_isa_type($isa);
}

sub get_endian
{
    my $result = `echo -n I | od -o | head -n1 | cut -f2 -d" " | cut -c6`;
    return $result ? 'little' : 'big';
}

sub get_thp_status
{
    my $path = "/sys/kernel/mm/transparent_hugepage/enabled";
    my $result = 'undetectable';
    $result = `cat $path` if -e $path;
    chomp $result;
    return "< $result >";
}

sub get_page_size
{
    my $result = `getconf PAGESIZE`;
    chomp $result;
    return $result if $result =~ /\d+/;
    return 0; # default value
}

sub get_cache_line_size
{
    my $os_type = get_os_type();
    my $result  = 0;
    if($os_type eq 'MACOSX')
    {
        $result = `sysctl hw.cachelinesize`;
    }
    elsif($os_type eq 'LINUX')
    {
        my $path = "/sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size";
        $result = `cat $path` if -e $path;
    }
    
    return $+{num} if($result =~ /(?<num>\d+)/);
    return 0; # default value
}

sub get_threads_per_core
{
    my $os_type = get_os_type();
    if($os_type eq 'MACOSX')
    {
        my $num_logical_cores_per_socket  = get_num_logical_core_per_socket();
        my $num_physical_cores_per_socket = get_num_physical_core_per_socket();
        if($num_physical_cores_per_socket != 0)
        {
            return $num_logical_cores_per_socket / $num_physical_cores_per_socket;
        }
        else
        {
            # use this as the default value if the detection failed
            return 1;
        }
    }
    elsif($os_type eq 'LINUX')
    {
        my $result = `lscpu`;
        my $threads_per_core = 1; # default value
           $threads_per_core = $+{num} if ($result =~ /Thread\(s\)\s+per\s+core\:\s+(?<num>\d+)/g);
        return $threads_per_core;
    }
}

sub get_num_physical_core_per_socket
{
    my $os_type = get_os_type();
    if($os_type eq 'MACOSX')
    {
        my $result = `sysctl hw.physicalcpu`;
        return $+{num} if($result =~ /(?<num>\d+)/g);
        return 0; # default value
    }
    elsif($os_type eq 'LINUX')
    {
        my $result = `lscpu`;
        return $+{num} if($result =~ /Core\(s\)\s+per\s+socket\:\s+(?<num>\d+)/g);
        return 0; # default value
    }
}

sub get_num_logical_core_per_socket
{
    my $os_type = get_os_type();
    if($os_type eq 'MACOSX')
    {
        my $result = `sysctl hw.logicalcpu`;
        return $+{num} if($result =~ /(?<num>\d+)/g);
        return 0; # default value
    }
    elsif($os_type eq 'LINUX')
    {
        return (get_threads_per_core() * get_num_physical_core_per_socket());
    }
}

sub print_sys_info
{
    my ($compiler) = @_;

    my $endian                  = get_endian();
    my $thp_status              = get_thp_status();
    my $page_size               = get_page_size();
    my $cache_line_size         = get_cache_line_size();
    my $os_type                 = get_os_type();
    my $host_isa_type           = get_host_isa_type();
    my $target_isa_type         = get_target_isa_type($compiler);
    my $num_physical_core       = get_num_physical_core_per_socket();
    my $num_threads_per_core    = get_threads_per_core();

    Veronica::Common::log_level("\n", 0);
    Veronica::Common::log_level("this machine is $endian-endian", 5);
    Veronica::Common::log_level("transparent huge page status is $thp_status", 5);
    Veronica::Common::log_level("page size is ".($page_size/1024)." KB, cache line size is $cache_line_size bytes", 5);
    Veronica::Common::log_level("host OS is $os_type, host ISA is $host_isa_type, target ISA is $target_isa_type", 5);
    Veronica::Common::log_level("there are $num_physical_core physical cores per socket, and $num_threads_per_core threads per physical core", 5);
    Veronica::Common::log_level("\n", 0);
}
1;