#!/bin/bash

HERE=$(cd `dirname $0`; pwd)
SPDZROOT=$HERE/..

export PLAYERS=${PLAYERS:-3}

if test "$THRESHOLD"; then
    t="-T $THRESHOLD"
fi

. $HERE/run-common.sh

run_player malicious-shamir-party.x $* $t || exit 1
