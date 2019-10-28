#!/usr/bin/env perl -w

package Veronica::Common;

use v5.10;
use strict;
use warnings;
use Cwd 'abs_path';
use File::Basename;
my @EXPORT = qw(get_script_path mkdir_or_die open_or_die say_level);

our $MSG_LEVEL = 5;

####################################################################################################

# Misc.

####################################################################################################

sub get_script_path
{
    return dirname(abs_path($0));
}

sub mkdir_or_die
{
    my ($dirpath, $die_msg) = @_;

    mkdir $dirpath if !-e $dirpath;
    
    die "[error] fail to create dir at $dirpath due to $!" . ($die_msg ne '' ? ", $die_msg" : '' )
    if !-e $dirpath;
}

sub open_or_die
{
    my ($file_to_open, $die_msg) = @_;

    die "[error] fail to open $file_to_open" . ($die_msg ne '' ? ", $die_msg" : '' )
    if !open my $FILE_HANDLE, "$file_to_open";

    return $FILE_HANDLE;
}

sub set_msg_level
{
    my ($level) = @_;
    our $MSG_LEVEL = $level;
}

sub say_level
{
    my ($level, $string) = @_;
    if($level <= $MSG_LEVEL && $string ne '')
    {
        my $prefix = '';
        if($level == 5)
        {
            $prefix = '[LIB-Script] ';
        }
        elsif($level == 4)
        {
            $prefix = '[INFO-Script] ';
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
            $prefix = '[BUG-Script] ';
        }
        elsif($level == 0)
        {
            $prefix = '';
        }

        say "${prefix}${string}";
    }
}
1;