FROM ubuntu
WORKDIR /workdir
ENV GIT_SSL_NO_VERIFY 1
ENV TERM xterm
RUN apt-get update \
    && apt-get install -y --no-install-recommends libncurses-dev git \
    && apt-get -y clean \
    && rm -rf /var/lib/apt/lists/* \
    && git clone https://github.com/K-Gamma/vvvvvv.git /workdir/vvvvvv

CMD [ "/bin/bash" ]