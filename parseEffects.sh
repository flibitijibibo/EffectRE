#!/bin/bash
# Parse Multiple Effects
# Written by Ethan "flibitijibibo" Lee
# Use: ./parseEffects.sh FXB

set -e

cd "`dirname "$0"`"

FILES=`ls $1 | grep '\.fxb'`
for f in $FILES
do
	./testparse glsl120 $1/$f
done
