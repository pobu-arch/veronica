#!/usr/bin/env perl -w

package Veronica::Common;

use v5.10;
use strict;
use warnings;
use Cwd 'abs_path';
use File::Basename;
use File::Copy qw(move);
use Carp qw/cluck/;
my @EXPORT = qw(get_script_path filter_path mkdir_or_die open_or_die override_symbol_link set_msg_level say_level get_os_type);

our $MSG_LEVEL = 5;

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

sub say_level
{
    my ($string, $level) = @_;

    if($level <= $MSG_LEVEL && $string ne '')
    {
        my $prefix = '';
        if($level == 5)
        {
            $prefix = '[INFO-Script] ';
        }
        elsif($level == 4)
        {
            $prefix = '[MESG-Script] ';
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
            print "\n";
            cluck "$@n";
            print "\n";
            die "[ERROR-Script] $string";
        }
        
        chomp $string;
        say "${prefix}"."${string}";
    }
}

sub mkdir_or_die
{
    my ($dirpath, $die_msg) = @_;

    mkdir $dirpath if !-e $dirpath;
    
    &say_level("fail to create dir at $dirpath due to $!" . ($die_msg ne '' ? ", $die_msg" : '' ), -1)
    if !-e $dirpath;
}

# $file_to_open should be appended with open pattern !
sub open_or_die
{
    my ($file_to_open, $die_msg) = @_;

    &say_level("fail to open $file_to_open" . ($die_msg ne '' ? ", $die_msg" : '' ), -1)
    if !open my $FILE_HANDLE, "$file_to_open";

    return $FILE_HANDLE;
}

sub override_symbol_link
{
    my ($src_file, $dest_link) = @_;
    system "rm -irf $dest_link" if -e $dest_link;
    system "ln -s $src_file $dest_link";
}

sub set_msg_level
{
    my ($level) = @_;
    our $MSG_LEVEL = $level;
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
        &say_level('unsupported OS', -1);
    }
}
1;