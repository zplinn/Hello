FROM livectf/livectf:finals-nsjail as base
# Alternatively: FROM livectf/livectf:finals-socat as base

FROM base as build

# Space-separated list of required packages
ARG REQUIRED_PACKAGES="build-essential wget"

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    ${REQUIRED_PACKAGES} \
    && rm -rf /var/lib/apt/lists/*


RUN wget -v -O capstone.tar.gz https://github.com/capstone-engine/capstone/archive/5.0.6.tar.gz \
    && tar xf capstone.tar.gz \
    && cd capstone-5.0.6 \
    && ./make.sh \
    && ./make.sh install \
    && cd ..

COPY build.sh /build.sh
COPY src/ /src/
RUN mkdir /build /handout
RUN cd / && /build.sh
RUN tar -czf /handout.tar.gz /handout

FROM base as run

COPY --from=build /handout.tar.gz /handout.tar.gz
RUN tar xf /handout.tar.gz --directory=/home/livectf --strip-components=1
RUN cp /home/livectf/libcapstone.so.5 /lib
COPY --chown=root:flag config.toml /home/livectf/.config.toml
RUN chmod 440 /home/livectf/.config.toml
