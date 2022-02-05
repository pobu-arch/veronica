#!/usr/bin/env perl

use v5.10;
use strict;
use warnings;
use File::Find;
use File::Copy;
use Time::HiRes qw(gettimeofday tv_interval);

use lib "$ENV{'VERONICA'}/perl";
use Veronica::Common;
use Veronica::Threads;

our $THIS_DIR       = Veronica::Common::get_script_path();
our $COUNTER_GROUP  = 'general';
our $MICROARCH      = '';

require "$THIS_DIR/intel_pmu_info.pl";

&cmd_parse();
&cmd_gen();

####################################################################################################

# Command-Line Parsing

####################################################################################################

sub cmd_parse()
{
    Veronica::Common::log_level("parsing arguments ...", 5);

    if(@ARGV < 1)
    {
        die '[error-script] no enough parameters for this script';
    }

    foreach my $current_arg (@ARGV)
    {
        if($current_arg =~ /-microarch=(?<microarch>\w+)/)
        {
            $MICROARCH = $+{microarch};
        }
        elsif($current_arg =~ /-group=(?<group>\w+)/)
        {
            $COUNTER_GROUP = $+{group};
        }
        else
        {
            die "[error-script] unknown parameters $current_arg";
        }
    }

    die "[error-script] please specify the microarchitecture" if $MICROARCH eq '';

    Veronica::Common::log_level("the selected microarchitecture is $MICROARCH".
                                ", metrics group is $COUNTER_GROUP", 5);
}


####################################################################################################

# Perf Command Generation

####################################################################################################

sub cmd_gen
{
    
}

