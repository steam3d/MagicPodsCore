FROM ubuntu:22.04 AS builder

RUN apt-get update
RUN apt-get install cmake -y
RUN apt-get install gcc -y
RUN apt-get install g++ -y
RUN apt-get install pkg-config -y
RUN apt-get install doxygen -y
RUN apt install graphviz -y
RUN apt-get install libsystemd-dev -y
RUN apt-get install git -y
RUN apt-get install zlib1g-dev -y
RUN apt-get install bluez -y
RUN apt-get install libbluetooth-dev -y

ADD ./src /app/src
ADD ./dependencies /app/dependencies
ADD ./CMakeFiles /app/CMakeFiles
ADD ./CMakeLists.txt /app/CMakeLists.txt

WORKDIR /app/build

RUN set -ex; cmake ../. && cmake --build .

ENTRYPOINT ["./MagicPodsCore"]