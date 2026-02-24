FROM livectf/livectf:finals-nsjail AS base
# Alternatively: FROM livectf/livectf:finals-socat as base

FROM base AS build

# Space-separated list of required packages
ARG REQUIRED_PACKAGES="build-essential python3 python3-pip"

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    ${REQUIRED_PACKAGES} \
    && rm -rf /var/lib/apt/lists/*

RUN pip install pycryptodome lief --break-system-packages

COPY build.sh /build.sh
COPY src/ /src/
RUN mkdir /build /handout
RUN cd / && /build.sh
RUN tar -czf /handout.tar.gz /handout

FROM base AS run

# Space-separated list of required packages
ARG REQUIRED_PACKAGES="python3"

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    ${REQUIRED_PACKAGES} \
    && rm -rf /var/lib/apt/lists/*

COPY --from=build /build/challenge /home/livectf/crackme
COPY src/server.py /home/livectf/challenge
COPY nsjail.conf /home/livectf/nsjail.conf
COPY --from=build /handout.tar.gz /handout.tar.gz
COPY --chown=root:flag config.toml /home/livectf/.config.toml
RUN chmod 440 /home/livectf/.config.toml
