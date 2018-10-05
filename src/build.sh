#!/bin/bash
rm main > /dev/null &
set e
make main -j6
./main
