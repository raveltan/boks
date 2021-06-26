# Boks
A simple UN*X container written in C. 
> **WARNING:** Boks currently only tested on linux. functionallity under other UN*X based distribution is not tested.

# Get the filesystem
Filesystem in boks acts like docker images, this allows container to have it's own PID and system root.

In order to get the filesystem, first option on getting the filesystem is by clonning the repo and extract it from the ubuntufs.tar,
the filesystem available in the repo is based on the ubuntu 20.04 docker image.
``` bash
git clone https://github.com/raveltan/boks.git
cd boks
tar xf ubuntufs.tar -C {{destionation}}
```
or you can also download it manually by using docker
``` bash
docker run -d --rm --name ubuntufs ubuntu:20.04 sleep 1000
docker export ubuntufs -o ubuntufs.tar
docker stop ubuntufs
mkdir -p ~/ubuntufs
tar xf ubuntufs.tar -C ~/ubuntufs/
```

# Running Boks
The easiest way to run boks is by getting the binary executable from the release section.
You can also build boks from scratch.
> As boks uses *CAP_SYS_ADMIN* capability boks needs to be run as root.

## Building Boks
To build boks, make sure that you have a recent version of C compiler and make
> Boks release is build using gcc, clang compatibility is not guaranteed.
```bash
git clone https://github.com/raveltan/boks.git
cd boks
make
```

# References
- https://docs.google.com/presentation/d/10vFQfEUvpf7qYyksNqiy-bAxcy-bvF0OnUElCOtTTRc/edit#slide=id.g101199134a_0_76
- https://blog.lizzie.io/linux-containers-in-500-loc.html
- https://www.youtube.com/watch?v=8fi7uSYlOdc
- https://man7.org/linux/man-pages/man7/namespaces.7.html
- https://man7.org/linux/man-pages/man2/clone.2.html
- https://linux.die.net/man/2/sethostname
