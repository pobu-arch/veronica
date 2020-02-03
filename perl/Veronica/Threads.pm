#!/usr/bin/env perl -w

package Veronica::Threads;

use v5.10;
use strict;
use warnings;
use threads;
use lib "$ENV{'VERONICA'}/perl";
use Veronica::Common;
my @EXPORT = qw(set_num_core join_n_thread_with_log thread_start);

our $NUM_CORE    = 8;
our %THREAD_POOL = ();

sub set_num_core
{
    my ($num) = @_;
    $NUM_CORE = $num;
}

sub system_entry
{
    my (@parameters) = @_;
    return system "@parameters";
}

sub backquote_entry
{
    my (@parameters) = @_;
    return `@parameters`;
}

sub join_n_thread_with_log
{
    my ($target_num) = @_;
    my $freed_slot = 0;
    my $error = 0;
    my $logfile;

    while(1)
    {
        foreach my $thread (threads->list())
        {
            if($thread->is_joinable())
            {
                $thread->join();

                $error         = $THREAD_POOL{$thread->tid()}{'status'} if $error ne 0;
                my $result     = $THREAD_POOL{$thread->tid()}{'result'};
                my $info       = $THREAD_POOL{$thread->tid()}{'info'};

                if(exists $THREAD_POOL{$thread->tid()}{'logfile'})
                {
                    my $logfile    = $THREAD_POOL{$thread->tid()}{'logfile'};
                    my $log_handle = Veronica::Common::open_or_die($logfile);
                    print $log_handle "\n\n";
                    print $log_handle $info;
                    print $log_handle $result if defined $result;
                    close $log_handle;
                }

                $freed_slot++;
                Veronica::Common::say_level('Thread '.$thread->tid().' joined with info '.$info, 5);
                delete $THREAD_POOL{$thread->tid()};
            }
        }
        last if(scalar threads->list() == 0);
        last if($freed_slot >= $target_num);

        #sleep(1);
    }
    return $error;
}

# $logfile should be appended with open pattern !
sub thread_start
{
    my ($info, $logfile, $entry_func_ref, @parameters) = @_;
    my $error = 0;
    
    if(scalar threads->list() >= $NUM_CORE)
    {
        $error = join_n_thread_with_log(1);
    }
    
    my $thread = threads->new($entry_func_ref, @parameters);
    my $tid = $thread->tid();

    $THREAD_POOL{$thread->tid()}{'status'}  = $?;
    $THREAD_POOL{$thread->tid()}{'result'}  = '';
    $THREAD_POOL{$thread->tid()}{'info'}    = $info;
    $THREAD_POOL{$thread->tid()}{'logfile'} = $logfile if $logfile ne '';
    
    Veronica::Common::say_level('Thread '.$thread->tid().' launched with info '.$info, 5);

    return $error if $error;
}

1;