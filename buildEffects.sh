#!/bin/bash
# Build Multiple Effects
# Written by Ethan "flibitijibibo" Lee
# Use: ./buildEffects.sh HLSL FXB

set -e

cd "`dirname "$0"`"

FILES=`ls $1`
for f in $FILES
do
	WINEDEBUG=fixme-all,err-all wine fxc.exe /T fx_2_0 $1/$f /Fo "$2/`basename $f .fx`.fxb"
done
