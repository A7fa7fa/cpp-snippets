#!/bin/sh

mode="release"
outname=main

if [ $# -eq 0 ]; then
    mode="debug"
fi

echo "Build <$mode> candidate..."

mkdir -p bin/release
mkdir -p bin/debug
rm -f bin/release/$outname
rm -f bin/debug/$outname

if [ $mode = "release" ]; then
    g++ -std=c++23 \
        -D NDEBUG \
        src/log/tests/*.cpp \
        -o bin/$mode/$outname
else
    g++ -std=c++23 \
        -fdiagnostics-color=always -g \
        src/log/tests/*.cpp \
         -o bin/$mode/$outname
fi

