##
## EPITECH PROJECT, 2022
## Makefile
## File description:
## Makefile for the Elara project
##

SRC_DIR = src
INCLUDE = -I$(VULKAN_SDK)/Include

ifeq ($(OS),Windows_NT)
	LIBS = -L$(VULKAN_SDK)/Lib -lkernel32 -luser32 -lvulkan-1
	EXTENSION = .exe
else
	LIBS = -L$(VULKAN_SDK)/Lib -L/usr/X11R6/lib -lX11 -lm -lX11-xcb -lxcb -lvulkan-1
endif

all: build

setup:
	mkdir bin

build:
	clang $(SRC_DIR)/*.c $(SRC_DIR)/*.cpp -o bin/Elara$(EXTENSION) $(INCLUDE) $(LIBS) -Wno-nullability-completeness

clean:
	rm -f bin/*.o
	rm -f bin/Elara$(EXTENSION)

run:
	bin/Elara$(EXTENSION)