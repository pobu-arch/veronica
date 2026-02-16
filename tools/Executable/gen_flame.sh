#!/usr/bin/env bash

perf script -i $1 > perf.out
stackcollapse-perf.pl ./perf.out > perf.folded
flamegraph.pl perf.folded > new.svg
rm perf.out
rm perf.folded
#rm perf.data
