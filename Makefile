##
## EPITECH PROJECT, 2022
## Makefile
## File description:
## Makefile for the Elara project
##

SRC_DIR = src
SRC     = *.c
INCLUDE = -I$(VULKAN_SDK)/Include

ifeq ($(OS),Windows_NT)
	LIBS = -lkernel32 -luser32
	EXTENSION = .exe
else
	LIBS = -lX11 -lm -lX11-xcb -lxcb -L/usr/X11R6/lib
endif

all: build

setup:
	mkdir bin

build:
	clang $(SRC_DIR)/$(SRC) -o bin/Elara$(EXTENSION) $(INCLUDE) $(LIBS)

clean:
	rm -f bin/*.o
	rm -f bin/Elara$(EXTENSION)

run:
	bin/Elara$(EXTENSION)