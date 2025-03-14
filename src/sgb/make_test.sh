#!/bin/bash

if [ -f /home/ubuntu/Documents/MagicPodsCore/src/sgb/main ]; then
    rm /home/ubuntu/Documents/MagicPodsCore/src/sgb/main
fi

g++ -fdiagnostics-color=always -g \
    /home/ubuntu/Documents/MagicPodsCore/src/sgb/main.cpp \
    /home/ubuntu/Documents/MagicPodsCore/src/sgb/Crc16.cpp \
    /home/ubuntu/Documents/MagicPodsCore/src/sgb/GalaxyBudsHelper.cpp \
    -o /home/ubuntu/Documents/MagicPodsCore/src/sgb/main

/home/ubuntu/Documents/MagicPodsCore/src/sgb/./main