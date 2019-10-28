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

echo "export VERONICA_PERL=$DIR"
export VERONICA_PERL=$DIR
