# GCC support can be specified at major, minor, or micro version
# (e.g. 8, 8.2 or 8.2.0).
# See https://hub.docker.com/r/library/gcc/ for all supported GCC
# tags from Docker Hub.
# See https://docs.docker.com/samples/library/gcc/ for more on how to use this image
FROM spacesur/surilib-environment:2.0.0

USER builder
WORKDIR /home/builder

RUN mkdir -p /home/builder/programa2mp/
COPY ./ /home/builder/programa2mp/
RUN mkdir -p /home/builder/packages

USER root
RUN chown builder:builder -R /home/builder/programa2mp/ /home/builder/packages

USER builder 

# Build packages. Binaries will be found on /home/builder/packages inside the container
CMD cd /home/builder/programa2mp/ ; bash ./build.sh ; cp -v WinBuild/*.exe LinuxBuild/*.deb /home/builder/packages 

LABEL Name=programa2mp Version=3.0.0
