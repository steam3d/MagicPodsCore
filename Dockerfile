FROM ubuntu:22.04 AS system_stage

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

FROM system_stage AS build_stage

ADD ./src /app/src
ADD ./dependencies /app/dependencies
ADD ./config /app/config
ADD ./CMakeLists.txt /app/CMakeLists.txt

WORKDIR /app/build

RUN cmake -DCMAKE_BUILD_TYPE=Release ../
RUN cmake --build ./

FROM scratch as copy_results_stage

COPY --from=build_stage /app/build/MagicPodsCore /


ENTRYPOINT ["./MagicPodsCore"]