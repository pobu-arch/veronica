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

our %TOP_INFO    = ();

&init_check();
require "$TOP_INFO{'speccpu_helper'}";
require "$TOP_INFO{'geekbench_helper'}";
require "$TOP_INFO{'parsec_helper'}";
require "$TOP_INFO{'gap_helper'}";

require "$TOP_INFO{'standalone_helper'}";
require "$TOP_INFO{'dlrm_helper'}";

require "$TOP_INFO{'common_helper'}";
require "$TOP_INFO{'download_helper'}";
&main_workflow();

####################################################################################################

# Init.

####################################################################################################

