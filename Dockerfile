FROM ubuntu:latest
ARG DEBIAN_FRONTEND=noninteractive
ENV TZ=US/Eastern
RUN set -ex;apt-get update -y && \
    apt-get install -y tzdata

RUN set -ex;apt-get update && \
    apt-get install -y --no-install-recommends \
    git \
    build-essential \
    pkg-config \
    cmake \
    libgtest-dev \
    valgrind \
    clang-format \
    libpcsclite-dev && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists
COPY . /app
WORKDIR /app/build
RUN set -ex;cmake ../;make;cd bin;
