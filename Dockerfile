FROM ubuntu:22.04 AS builder
LABEL authors="Clemens Elflein"

RUN apt-get update && apt-get install -y  \
    gcc-arm-none-eabi git \
    libasio-dev iproute2 \
    python3 python3-venv python3-pip \
    cmake \
    make \
    && rm -rf /var/lib/apt/lists/*

COPY . /project

WORKDIR /project
RUN mkdir build
RUN cd build && cmake .. --preset=Release && cd Release && make -j$(nproc)


FROM scratch
COPY --from=builder /project/build/Release/xbot-firmware.bin /xbot-firmware.bin
COPY --from=builder /project/build/Release/xbot-firmware.elf /xbot-firmware.elf
