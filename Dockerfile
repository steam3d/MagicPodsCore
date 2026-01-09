FROM ubuntu:22.04 AS system_stage

RUN apt-get update \
    && apt-get install -y --no-install-recommends \
        cmake \
        gcc \
        g++ \
        make \
        pkg-config \
        ca-certificates \
        doxygen \
        graphviz \
        libsystemd-dev \
        git \
        zlib1g-dev \
        bluez \
        libbluetooth-dev \
        libpulse-dev \
    && rm -rf /var/lib/apt/lists/*

FROM system_stage AS build_stage

ADD ./src /app/src
ADD ./dependencies /app/dependencies
ADD ./config /app/config
ADD ./CMakeLists.txt /app/CMakeLists.txt

WORKDIR /app/build

RUN cmake -DCMAKE_BUILD_TYPE=Release ../
RUN cmake --build ./

FROM scratch AS copy_results_stage

COPY --from=build_stage /app/build/MagicPodsCore /


ENTRYPOINT ["./MagicPodsCore"]
