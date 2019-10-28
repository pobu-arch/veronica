#!/usr/bin/env bash

DIR=$(cd $(dirname "${BASH_SOURCE[0]}") >/dev/null && pwd)
echo "export VERONICA_PERL=$DIR"
export VERONICA_PERL=$DIR
