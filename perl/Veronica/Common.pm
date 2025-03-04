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

our $LOG_LEVEL = 4;

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

sub set_log_level
{
    my ($level) = @_;
    our $LOG_LEVEL = $level;
}

sub log_level
{
    my ($string, $level) = @_;

    $level = 4 if !defined $level;
    die "[ERROR-Script] log level is $level, which should NOT be larger than 4\n\n" if $level > 4;

    if($level <= $LOG_LEVEL && defined $string && $string ne '')
    {
        my $prefix = '';
        if($level == 4)
        {
            $prefix = '[DEBUG-Script] ';
        }
        elsif($level == 3)
        {
            $prefix = '[INFO-Script] ';
        }
        elsif($level == 2)
        {
            $prefix = '[WARNING-Script] ';
        }
        elsif($level == 1)
        {
            $prefix = '[CRITICAL-Script] ';
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
    $die_msg = '' if !defined $die_msg;

    mkdir $dirpath if !-e $dirpath;
    
    &Veronica::Common::log_level("failed to create dir at $dirpath due to $!" . ($die_msg ne '' ? ", $die_msg" : '' ), -1)
    if !-e $dirpath;
}

# $file_to_open should be appended with open pattern !
sub open_or_die
{
    my ($file_to_open, $die_msg) = @_;
    $die_msg = '' if !defined $die_msg;

    &Veronica::Common::log_level("failed to open $file_to_open, $!" . ($die_msg ne '' ? ", $die_msg" : '' ), -1)
    if !open my $FILE_HANDLE, "$file_to_open";

    return $FILE_HANDLE;
}

sub read_filelist
{
    my ($name, $find_path, $filelist_path) = @_;
    my %filelist = ();
    
    if(-e $filelist_path)
    {
        my $filelist_handle = &Veronica::Common::open_or_die("<$filelist_path");
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
                &Veronica::Common::log_level("file $line doesn't exist at $overall_path", -1);
            }
        }
        close $filelist_handle;

        &Veronica::Common::log_level("filelist contains " . (scalar keys %filelist). " files for $name ...", 3);
    }
    else
    {
        &Veronica::Common::log_level("filelist doesn't exist at $filelist_path", -1);
    }

    return %filelist;
}

sub override_symbol_link
{
    my ($src_file, $dest_link) = @_;
    system "rm -irf $dest_link" if -e $dest_link;
    system "ln -s $src_file $dest_link";
}

sub remove_tailing_blank_char
{
    my ($string) = @_; 
    return '' if !defined $string or $string eq '';
    
    chomp $string; $string =~ s/\s+$//;
    return $string;
}
1;