# BmpZipper Tool
BmpZipper is a cross-platform utility written with C++ and QML languages which performs compressing/decompressing 8bit '*.bmp' files.

Currently utility is tested on Linux (Debian) and macOS systems.

The main purpose of the application is the compressing and decompressing 8bit BMP pictures using RLE algorithm.

The algorithm is based on collecting specific index where each white line is encoded as 1'b and each non-white line as 0'b. The size of the index in bits is equal to image height( number of rows ) padded with 0b for byte alignment. Each non-white line encodes additionaly with 4 bytes pattern folowing next idea:
* each 4 white pixels -> encoded 0
* each 4 black pixels -> encoded 10
* other 4 pixels -> encoded 11 + pix0 + pix1 + pix2 + pix3.

Such algorithm works good for 8bit BMP pictures which have a lot of white space and black points (for example text images).


# Barch file Format:

The encoded file has `*.barch` extension. File format is specified as original
8bit BMP file format preserving original header and color info. The main difference with BMP format is Signature = 'BA' instead of 'BM' and BmpHeader.Reserved = IndexOffset instead of 0 for BMP pictures. The IndexOffset locates where original PixelData was located, and compressed PixelData starts immediately after Index.

| File Section          | Size Info                                              |
| --------------------- |:------------------------------------------------------:|
| Bmp Header            | 14 bytes, Signature = 'BA', Reserved = IndexOffset     |
| Info Header           | 40+ bytes                                              |
| Color Table           | Optional                                               |
| Index Data            | Size = Height / 8 + padding, 1 - white row, 0 - other  |
| Pixel Data Compressed | Pixel Data compressed with mentioned algorithm         |

# Build and Run

You can use `Make` just add the path to your Qt binaries, ninja and working directory to the variables `QT_BINARIES_PATH`, `NINJA_PATH` and `WORK_DIR` in `Makefile`
and then you will be able to use a simple `Make`-based interface such as:
```shell
    make clean
    make build
    make run
```

# BmpZipper Usage:
./bmp-zipper [options]

Description: BmpZipper application for compressing/decompressing 8bit `*.bmp` files

> Options:
  * -h, --help             Displays help on commandline options.
  * --help-all             Displays help, including generic Qt options.
  * -v, --version          Displays version information.
  * -d, --dir <directory>  Scan `bmp`, `barch` and `png` files in `<directory>`.

# Docker container

Also, it's possible to download or build a docker container with the BmpZipper tool running inside.
In this case you don't need installing Qt and ninja, all that you need just docker to be installed to you machine and
VNC client (https://tigervnc.org/) to be able to see the content.

if you want to build an image just do this command in root directory of BmpZipper project
```shell
    docker build -t bmp-zipper .
```

then you can run the container
```shell
    docker run -d -p 5900:5900 --name bmp-zipper-container bmp-zipper
```

after that you just need to connect your VNC client to the container, connection string is
```
    localhost:5900
```
if you want you can change the port, but you will need to re-build the container.

if you want to get inside the container just do this command:
```shell
    docker exec -it bmp-zipper-container bash
```

You can also pull the container from Docker Hub
```shell
    docker pull kyrylo21/bmp-zipper:v1.0
```
and run it like that:
```shell
    docker run -d -p 5900:5900 --name bmp-zipper-container kyrylo21/bmp-zipper:v1.0
```

The image link at Docker Hub: https://hub.docker.com/repository/docker/kyrylo21/bmp-zipper/general
