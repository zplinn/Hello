FROM livectf/livectf:finals-nsjail as base
# Alternatively: FROM livectf/livectf:finals-socat as base

FROM base as build

# Space-separated list of required packages
ARG REQUIRED_PACKAGES="curl qemu-user-binfmt"

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    ${REQUIRED_PACKAGES} \
    && rm -rf /var/lib/apt/lists/*

RUN cd / && curl -L -O https://github.com/zyedidia/riscv-gnu-toolchain-prebuilt/releases/download/v1/riscv64-unknown-elf-multilib.tar.gz
RUN cd / && tar -xf riscv64-unknown-elf-multilib.tar.gz
RUN rm /riscv64-unknown-elf-multilib.tar.gz

COPY build.sh /build.sh
COPY src/ /src/
RUN mkdir /build /handout
RUN cd / && /build.sh
RUN tar -czf /handout.tar.gz /handout

FROM base as run

# Install qemu-user-binfmt so the challenge can run without extra work
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    qemu-user-binfmt \
    && rm -rf /var/lib/apt/lists/*

COPY --from=build /build/challenge /home/livectf/
COPY nsjail.conf /home/livectf/nsjail.conf
COPY --from=build /handout.tar.gz /handout.tar.gz
COPY --chown=root:flag config.toml /home/livectf/.config.toml
RUN chmod 440 /home/livectf/.config.toml
