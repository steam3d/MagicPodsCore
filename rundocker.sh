#!/bin/bash
docker build -t mpd .
docker run -d --name mpdc mpd
docker cp mpdc:/app/build/MagicPodsCore .
#docker cp mpdc:/app/build/MagicPodsCore '/home/ubuntu/Documents/DeckyHelloWorld/defaults/' 

docker stop mpdc
docker rm mpdc

# created containers
# docker container ls