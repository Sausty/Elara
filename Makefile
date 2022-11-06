##
## EPITECH PROJECT, 2022
## Makefile
## File description:
## Makefile for the Elara project
##

SRC_DIR = src
INCLUDE = -I$(VULKAN_SDK)/Include
NAME = Elara

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
	clang $(SRC_DIR)/*.c $(SRC_DIR)/*.cpp -o bin/$(NAME)$(EXTENSION) $(INCLUDE) $(LIBS) -Wno-nullability-completeness -D_CRT_SECURE_NO_WARNINGS -Wno-switch

clean:
	rm -f bin/*.o

fclean: clean
	rm -f bin/$(NAME)$(EXTENSION)

re: clean all

run:
	bin/$(NAME)$(EXTENSION)