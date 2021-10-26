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

echo "Please add this following command to your shrc file to make it effective everytime you login:"
echo "export VERONICA=$DIR"
export VERONICA=$DIR
