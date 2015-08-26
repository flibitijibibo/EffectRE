#!/bin/bash
# Parse Single Effect
# Written by Ethan "flibitijibibo" Lee
# Use: ./parseEffect.sh SpriteEffect.fxb

cd "`dirname "$0"`"

./testparse glsl120 $1
