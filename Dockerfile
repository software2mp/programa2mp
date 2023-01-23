# GCC support can be specified at major, minor, or micro version
# (e.g. 8, 8.2 or 8.2.0).
# See https://hub.docker.com/r/library/gcc/ for all supported GCC
# tags from Docker Hub.
# See https://docs.docker.com/samples/library/gcc/ for more on how to use this image
FROM spacesur/surilib-environment:2.0.0

USER builder
WORKDIR /home/builder

RUN mkdir -p /home/builder/packages

CMD cd /home/builder/programa2mp/ ; ./build.sh

LABEL Name=programa2mp Version=3.0.0
