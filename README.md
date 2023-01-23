## Build

The container will build the current source tree, which has to be mounted onto the image on `/home/builder/programa2mp`.

In order to access the built artifacts, mount a directory onto the image on `/home/builder/packages`.

```shell
$ docker build -t spacesur/programa2mp:3.0.0 .
$ mkdir packages
$ docker run -v `pwd`:/home/builder/programa2mp -v `pwd`:/home/builder/packages -t spacesur/programa2mp:3.0.0
```