#!/usr/bin/env perl -w

package Veronica::Common;

use v5.10;
use strict;
use warnings;
use File::Basename;
use File::Copy qw(move);
use Cwd 'abs_path';
use Carp qw/cluck/;
use Time::HiRes qw(gettimeofday tv_interval);

our $LOG_LEVEL = 5;

####################################################################################################

# Misc.

####################################################################################################

sub get_script_path
{
    return dirname(abs_path($0));
}

sub filter_path
{
    my ($new_path) = @_;

    $new_path =~ s/(?<s>\s)/\\$+{s}/xg;
    $new_path =~ s/\\$//xg;
    $new_path =~ s/\</\\\</xg;
    $new_path =~ s/\>/\\\>/xg;
    $new_path =~ s/\(/\\\(/xg;
    $new_path =~ s/\)/\\\)/xg;
    $new_path =~ s/\@/\\\@/xg;
    $new_path =~ s/\&/\\\&/xg;
    $new_path =~ s/\'/\\\'/xg;
    $new_path =~ s/\,/\\\,/xg;
    $new_path =~ s/\./\\\./xg;
    $new_path =~ s/\-/\\\-/xg;
    $new_path =~ s/^\\(?<rest>.+)/$+{rest}/;
    chomp $new_path;
    return $new_path;
}

sub read_filelist
{
    my ($name, $find_path, $filelist_path) = @_;
    my %filelist = ();
    
    if(-e $filelist_path)
    {
        my $filelist_handle = Veronica::Common::open_or_die("<$filelist_path");
        my $filenum = 0;

        while(my $line = <$filelist_handle>)
        {
            chomp $line;
            my $overall_path = "$find_path/$line";
            
            if(-e $overall_path)
            {
                $filelist{$overall_path}{'filename'} = $line;
                $filelist{$overall_path}{'filenum'}  = $filenum;
                $filenum++;
            }
            else
            {
                Veronica::Common::log_level("file $line doesn't exist at $overall_path", -1);
            }
        }
        close $filelist_handle;

        Veronica::Common::log_level("filelist contains " . (scalar keys %filelist). " files for $name ...", 5);
    }
    else
    {
        Veronica::Common::log_level("filelist doesn't exist at $filelist_path", -1);
    }

    return %filelist;
}

sub set_log_level
{
    my ($level) = @_;
    our $LOG_LEVEL = $level;
}

sub log_level
{
    my ($string, $level) = @_;

    if($level <= $LOG_LEVEL && $string ne '')
    {
        my $prefix = '';
        if($level == 6)
        {
            $prefix = '[DEBUG-Script] ';
        }
        elsif($level == 5)
        {
            $prefix = '[INFO-Script] ';
        }
        elsif($level == 4)
        {
            $prefix = '[UNINPLEMENTED-Script] ';
        }
        elsif($level == 3)
        {
            $prefix = '[WARNING-Script] ';
        }
        elsif($level == 2)
        {
            $prefix = '[CRITICAL-Script] ';
        }
        elsif($level == 1)
        {
            $prefix = '[ERROR-Script] ';
        }
        elsif($level == 0)
        {
            $prefix = '';
        }
        elsif($level == -1)
        {
            say "\n";
            cluck "$@n";  # display the error line num
            say "\n";
            die "[ERROR-Script] $string\n\n";
        }
        else
        {
            say "\n";
            cluck "$@n";  # display the error line num
            say "\n";
            die "[ERROR-Script] Wrong Parameters\n\n";
        }
        
        chomp $string;
        say "${prefix}"."${string}";
    }
}

sub mkdir_or_die
{
    my ($dirpath, $die_msg) = @_;

    mkdir $dirpath if !-e $dirpath;
    
    &log_level("fail to create dir at $dirpath due to $!" . ($die_msg ne '' ? ", $die_msg" : '' ), -1)
    if !-e $dirpath;
}

# $file_to_open should be appended with open pattern !
sub open_or_die
{
    my ($file_to_open, $die_msg) = @_;

    &log_level("fail to open $file_to_open" . ($die_msg ne '' ? ", $die_msg" : '' ), -1)
    if !open my $FILE_HANDLE, "$file_to_open";

    return $FILE_HANDLE;
}

sub override_symbol_link
{
    my ($src_file, $dest_link) = @_;
    system "rm -irf $dest_link" if -e $dest_link;
    system "ln -s $src_file $dest_link";
}

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
        &log_level('unsupported OS', -1);
    }
}

sub parse_arch_type
{
    my ($string) = @_;

    # detect target arch type from compiler info
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
        &log_level('unsupported arch', -1);
    }
}

sub get_host_arch_type
{
    my $result = `uname -a`;
    return &parse_arch_type($result);
}

sub get_target_arch_type
{
    my ($compiler) = @_;

    my $result = `$compiler -v 2>&1`;
    my $arch   = '';

    # detect compiler info
    if($result =~ /--target=(?<arch>\w+)/g)
    {
        $arch = $+{arch};
    }
    elsif($result =~ /Target\:\s+(?<arch>\w+)/g)
    {
        $arch = $+{arch};
    }
    else
    {
        &log_level('unsupported compiler info format', -1);
    }

    return &parse_arch_type($arch);
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
    my $os_type = &get_os_type();
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
    my $os_type = &get_os_type();
    if($os_type eq 'MACOSX')
    {
        my $num_logical_cores_per_socket  = &get_num_logical_core_per_socket();
        my $num_physical_cores_per_socket = &get_num_physical_core_per_socket();
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
    my $os_type = &get_os_type();
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
    my $os_type = &get_os_type();
    if($os_type eq 'MACOSX')
    {
        my $result = `sysctl hw.logicalcpu`;
        return $+{num} if($result =~ /(?<num>\d+)/g);
        return 0; # default value
    }
    elsif($os_type eq 'LINUX')
    {
        return (&get_threads_per_core() * &get_num_physical_core_per_socket());
    }
}

sub print_sys_info
{
    my ($compiler) = @_;

    my $endian                  = &get_endian();
    my $thp_status              = &get_thp_status();
    my $page_size               = &get_page_size();
    my $cache_line_size         = &get_cache_line_size();
    my $os_type                 = &get_os_type();
    my $host_arch_type          = &get_host_arch_type();
    my $target_arch_type        = &get_target_arch_type($compiler);
    my $num_physical_core       = &get_num_physical_core_per_socket();
    my $num_threads_per_core    = &get_threads_per_core();

    log_level("\n", 0);
    log_level("this machine is $endian-endian", 5);
    log_level("transparent huge page status is $thp_status", 5);
    log_level("page size is ".($page_size/1024)." KB, cache line size is $cache_line_size bytes", 5);
    log_level("host OS is $os_type, host arch is $host_arch_type, target arch is $target_arch_type", 5);
    log_level("there are $num_physical_core physical cores per socket, and $num_threads_per_core threads per physical core", 5);
    log_level("\n", 0);
}
1;