#!/usr/bin/env perl -w

package Veronica::Threads;

use v5.10;
use strict;
use warnings;
use threads;
use lib "$ENV{'VERONICA'}/perl";
use Veronica::Common;

our $NUM_CORE_LIMIT = 8;
our %THREAD_POOL    = ();
our $PLACE_HOLDER   = '--VERONICA--';

sub set_num_core
{
    my ($num) = @_;
    $NUM_CORE_LIMIT = $num;
}

sub get_num_core
{
    return $NUM_CORE_LIMIT;
}

sub system_entry
{
    my (@parameters) = @_;
    return system "@parameters";
}

sub backquote_entry
{
    my (@parameters) = @_;
    my $thread = threads->self();
    my $result = `@parameters 2>&1`;
    my $status = $?;
    return "$status"."$PLACE_HOLDER"."$result";
}

sub join_n_thread_with_log
{
    my ($target_num) = @_;
    my $freed_slot   = 0;
    my $logfile;
    
    my $return_value = 0;
    my $error        = 0;

    while(1)
    {
        foreach my $thread (threads->list())
        {
            if($thread->is_joinable())
            {
                my $return = $thread->join();
                my @split_output = split $PLACE_HOLDER, $return;
                $THREAD_POOL{$thread->tid()}{'status'} = $split_output[0];
                $THREAD_POOL{$thread->tid()}{'result'} = $split_output[1] if defined $split_output[1];

                if(exists $THREAD_POOL{$thread->tid()}{'logfile'})
                {
                    my $logfile    = $THREAD_POOL{$thread->tid()}{'logfile'};
                    my $log_handle = Veronica::Common::open_or_die($logfile);
                    
                    print $log_handle "\n\n";
                    print $log_handle $THREAD_POOL{$thread->tid()}{'info'} . "  --  ";
                    print $log_handle $THREAD_POOL{$thread->tid()}{'parameters'};
                    print $log_handle "\n\n";
                    print $log_handle $THREAD_POOL{$thread->tid()}{'result'}
                          if exists $THREAD_POOL{$thread->tid()}{'result'};
                    close $log_handle;
                }

                $freed_slot++;
                $return_value = $THREAD_POOL{$thread->tid()}{'status'};
                Veronica::Common::log_level("\n",0);
                if($return_value)
                {
                    Veronica::Common::log_level('Thread '.$thread->tid().' detected error with info - '.$THREAD_POOL{$thread->tid()}{'info'}, 1);
                    Veronica::Common::log_level('Thread '.$thread->tid().' error message is - '.$THREAD_POOL{$thread->tid()}{'result'}, 1) if exists $THREAD_POOL{$thread->tid()}{'result'};
                    $error = 1;
                }
                else
                {
                    Veronica::Common::log_level('Thread '.$thread->tid().' succeessfully joined with info - '.$THREAD_POOL{$thread->tid()}{'info'}, 5);
                }
                delete $THREAD_POOL{$thread->tid()};
            }
        }

        if($error)
        {
            Veronica::Common::log_level('Evicting all the threads due to the detected error', 1);
            
            # empty the thread pool
            while(scalar threads->list())
            {
                foreach my $thread (threads->list())
                {
                    $thread->join() if($thread->is_joinable());
                }
            }
            return $error;
        }

        last if($freed_slot >= $target_num || scalar threads->list() == 0);
        #sleep(1);
    }
    return 0;
}

# $logfile should be appended with open pattern !
sub thread_start
{
    my ($info, $logfile, $entry_func_ref, @parameters) = @_;
    my $error = 0;
    
    if(scalar threads->list() >= $NUM_CORE_LIMIT)
    {
        $error = join_n_thread_with_log(1);
        return $error if $error;
    }
    
    my $thread = threads->new($entry_func_ref, @parameters);
    my $tid = $thread->tid();
    $THREAD_POOL{$thread->tid()}{'info'}        = $info;
    $THREAD_POOL{$thread->tid()}{'parameters'}  = "@parameters";
    $THREAD_POOL{$thread->tid()}{'logfile'}     = $logfile if $logfile ne '';
    
    Veronica::Common::log_level("\n",0);
    Veronica::Common::log_level('Thread '.$thread->tid().' launched with info - '.$info, 5);

    return $error;
}

1;