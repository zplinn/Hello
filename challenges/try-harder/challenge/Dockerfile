FROM livectf/livectf:finals-nsjail as base
# Alternatively: FROM livectf/livectf:finals-socat as base

FROM base as build

# Space-separated list of required packages
ARG REQUIRED_PACKAGES="build-essential python3 git autoconf automake"

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    ${REQUIRED_PACKAGES} \
    && rm -rf /var/lib/apt/lists/*

RUN git clone https://github.com/guillermocalvo/exceptions4c.git /exceptions4c && \
    cd /exceptions4c && \
    autoreconf --install && \
    ./configure && \
    make all
COPY build.sh /build.sh
COPY src/ /src/
RUN mkdir /build /handout
RUN cd / && /build.sh
RUN tar -czf /handout.tar.gz /handout

FROM base as run

COPY --from=build /build/challenge /home/livectf/crackme
COPY src/server.sh /home/livectf/challenge
COPY nsjail.conf /home/livectf/nsjail.conf
COPY --from=build /handout.tar.gz /handout.tar.gz
COPY --chown=root:flag config.toml /home/livectf/.config.toml
RUN chmod 440 /home/livectf/.config.toml
