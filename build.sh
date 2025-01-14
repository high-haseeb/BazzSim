#!/bin/sh
set -xe

mkdir -p build
cd build
gcc ./../main.c -o main `pkgconf raylib --libs --cflags`
./main ./../map.txt
