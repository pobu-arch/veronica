#!/usr/bin/env bash

git submodule init

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
echo "export PATH=\"\$VERONICA/tools/PerfWrapper:\$VERONICA/tools/FlameGraph:\$VERONICA/tools/Bash:\$VERONICA/tools/MacOSKpep:\$PATH\""
