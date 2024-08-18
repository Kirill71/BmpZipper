.PHONY: all build run clean

QT_BINARIES_PATH := ~/Qt/6.7.2/macos
# QT_PATH will be installed in Dockerfile
ifneq ($(QT_PATH),)
	QT_BINARIES_PATH := $(QT_PATH)
endif

NINJA_PATH := $(shell which ninja)
WORK_DIR := $(shell pwd)/images

all: build

build:
	./scripts/build.sh $(NINJA_PATH) $(QT_BINARIES_PATH)

run:
	./scripts/run.sh $(WORK_DIR)

clean:
	./scripts/clean.sh
