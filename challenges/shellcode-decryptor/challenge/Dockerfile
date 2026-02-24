FROM livectf/livectf:finals-socat as base

FROM base as build

# Space-separated list of required packages
ARG REQUIRED_PACKAGES="build-essential python3 git cmake build-essential flex bison ninja-build bsdmainutils xxd"

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    ${REQUIRED_PACKAGES} \
    && rm -rf /var/lib/apt/lists/*

RUN git clone https://github.com/vector35/scc \
    && cd scc \
    && git submodule set-url asmx86 https://github.com/Vector35/asmx86.git \
    && git submodule update --init --recursive \
    && cmake -S . -B build -G Ninja \
    && cmake --build build \
    && cp build/scc /bin/scc \
    && rm -rf /scc

COPY build.sh /build.sh
COPY src/ /src/
RUN mkdir /build /handout
RUN cd / && /build.sh
RUN tar -czf /handout.tar.gz /handout

FROM base as run

# Space-separated list of required packages
ARG REQUIRED_PACKAGES="build-essential python3 gdb gdbserver"

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    ${REQUIRED_PACKAGES} \
    && rm -rf /var/lib/apt/lists/*

COPY --from=build /src/challenge /home/livectf/
COPY --from=build /handout.tar.gz /handout.tar.gz
COPY --chown=root:flag config.toml /home/livectf/.config.toml
RUN chmod 440 /home/livectf/.config.toml
