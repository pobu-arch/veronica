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

my @EXPORT = qw(get_script_path filter_path read_filelist set_msg_level log_level mkdir_or_die open_or_die override_symbol_link  get_os_type get_endian);

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
        return 'MacOSX';
    }
    elsif($result =~ 'Linux')
    {
        return 'Linux';
    }
    else
    {
        &log_level('unsupported OS', -1);
    }
}
sub get_endian
{
    my $result = `echo -n I | od -o | head -n1 | cut -f2 -d" " | cut -c6`;
    return $result ? 'little' : 'big';
}
1;