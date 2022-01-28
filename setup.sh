#!/usr/bin/env bash

DIR=''
case $OSTYPE in
    linux-gnu*)
        DIR=$(cd $(dirname "${BASH_SOURCE[0]}") >/dev/null && pwd)
        echo "OSTYPE is Linux"
        ;;
    darwin*)
        DIR=$(cd "$(dirname "$0")";pwd)
        echo "OSTYPE is MacOSX"
        ;;
    *)
        echo $OSTYPE
        ;;
esac

echo "Please add the following commands to your shrc file to make it effective everytime you login:"
echo "export VERONICA=$DIR"
echo "export PATH=\"\$VERONICA/perl/perf_wrapper:\$PATH\""
