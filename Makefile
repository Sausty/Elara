##
## EPITECH PROJECT, 2022
## Makefile
## File description:
## Makefile for the Elara project
##

NAME = Elara
SRC_DIR = src
SOURCES = $(SRC_DIR)/*.c $(SRC_DIR)/*.cpp
OUTPUT = bin/$(NAME)$(EXTENSION)
INCLUDE = -I$(VULKAN_SDK)/Include
FLAGS = -Wno-nullability-completeness -Wno-switch -g -O0
DEFINES = -D_CRT_SECURE_NO_WARNINGS

ifeq ($(OS),Windows_NT)
	LIBS = -L$(VULKAN_SDK)/Lib -lkernel32 -luser32 -lvulkan-1
	EXTENSION = .exe
else
	LIBS = -L$(VULKAN_SDK)/Lib -L/usr/X11R6/lib -lX11 -lm -lX11-xcb -lxcb -lvulkan-1
endif

all: $(NAME)

setup:
	mkdir bin

$(NAME):
	clang $(SOURCES) -o $(OUTPUT) $(INCLUDE) $(LIBS) $(FLAGS) $(DEFINES)

clean:
	rm -f bin/*.o

fclean: clean
	rm -f bin/$(NAME)$(EXTENSION)

re: fclean all

run:
	bin/$(NAME)$(EXTENSION)

build_shaders:
	scripts/build_shaders.bat

.PHONY: all, setup, $(NAME), clean, fclean, re, run, build_shaders