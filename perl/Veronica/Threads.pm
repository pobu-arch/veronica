#!/usr/bin/env perl -w

package Veronica::Threads;

use v5.10;
use strict;
use warnings;
use threads;
use lib $ENV{'VERONICA_PERL'};
use Veronica::Common;
my @EXPORT = qw(set_num_core make_at_least_n_thread_slots thread_start thread_main);

our $NUM_CORE    = 8;
our %THREAD_POOL = ();

sub set_num_core
{
    my ($num) = @_;
    $NUM_CORE = $num;
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
                my $result = $thread->join();

                $error   = $THREAD_POOL{$thread->tid()}{'status'} if $error ne 0;
                $logfile = $THREAD_POOL{$thread->tid()}{'logfile'};
                my $log_handle = Veronica::Common::open_or_die($logfile);
                print $log_handle "\n\n";
                print $log_handle $THREAD_POOL{$thread->tid()}{'run_cmd'};
                print $log_handle $THREAD_POOL{$thread->tid()}{'result'};
                close $log_handle;

                $freed_slot++;
                Veronica::Common::say_level(5, 'Thread '.$thread->tid().' joined');
                delete $THREAD_POOL{$thread->tid()};
            }
        }
        last if($freed_slot >= $target_num);

        #sleep(1);
    }
    return $error;
}

# $logfile should be appended with open pattern ! 
sub thread_start
{
    my ($run_cmd, $logfile) = @_;
    my $error = 0;
    
    if(scalar threads->list() >= $NUM_CORE)
    {
        $error = join_n_thread_with_log(1);
    }
    
    my $thread = threads->new(\&thread_main, "$run_cmd");
    Veronica::Common::say_level(5, 'Thread '.$thread->tid().' launched');

    $THREAD_POOL{$thread->tid()}{'run_cmd'} = $run_cmd;
    $THREAD_POOL{$thread->tid()}{'logfile'} = $logfile;

    return $error if $error;
}

sub thread_main
{
    my ($run_cmd) = @_;
    my $result = `$run_cmd 2>&1`;
    my $thread = threads->self();
    my $tid    = $thread->tid();

    $THREAD_POOL{$thread->tid()}{'result'}  = $result;
    $THREAD_POOL{$thread->tid()}{'status'}  = $?;
}

1;