#!/bin/bash
# Build Single Effect
# Written by Ethan "flibitijibibo" Lee
# Use: ./buildEffect.sh SpriteEffect.fx

cd "`dirname "$0"`"

WINEDEBUG=fixme-all,err-all wine fxc.exe /T fx_2_0 $1 /Fo "`basename $1 .fx`.fxb"
