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

our %TOP_INFO               = ();
$TOP_INFO{'root_dir'}       = Veronica::Common::get_script_path();
$TOP_INFO{'intel_pmu_info'} = $TOP_INFO{'root_dir'}.'/intel_pmu_info.pl';

require "$TOP_INFO{'intel_pmu_info'}";

&cmd_parse();
&gen_perf_cmd();

####################################################################################################

# Init.

####################################################################################################

sub cmd_parse
{

}

sub gen_perf_cmd
{
    
}