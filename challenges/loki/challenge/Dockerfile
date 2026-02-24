# Base image with all build tools for Loki challenge
# This image is built once and reused for iterative development
FROM livectf/livectf:finals-nsjail as base

FROM base as build

# Install all build dependencies
ARG BUILD_PACKAGES="build-essential libssl-dev python3 python3-pip binutils socat"
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update -qq && \
    apt-get install -y -qq --no-install-recommends \
    ${BUILD_PACKAGES} \
    && rm -rf /var/lib/apt/lists/* > /dev/null 2>&1

# Install Python dependencies for checksum update script
RUN pip3 install pyelftools --break-system-packages

# Create working directories
RUN mkdir -p /build /handout /src /artifacts

WORKDIR /src

COPY build.sh /build.sh
COPY src/ /src/
# Directories already created in base image
RUN cd / && /build.sh
RUN tar -czf /handout.tar.gz /handout

# Runtime stage - uses original minimal base for production
FROM base as run

COPY --from=build /build/challenge /home/livectf/
COPY --from=build /handout.tar.gz /handout.tar.gz
COPY --chown=root:flag config.toml /home/livectf/.config.toml
RUN chmod 440 /home/livectf/.config.toml
