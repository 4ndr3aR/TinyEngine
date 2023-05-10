# TinyEngine Makefile
# Compiler Configuration

CC = g++ -std=c++17
CF = -Wfatal-errors
LF = -I /tmp/include -I$(HOME)/.local/include -L$(HOME)/.local/lib
EXTRA_FLAGS = -O0 -g3 -DBOOST_LOG_DYN_LINK

# General Linking

TINYLINK = -lpthread -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lGLEW -lboost_system -lboost_log -lboost_filesystem

# OS Specific Linking

UNAME := $(shell uname)

## Detect GNU/Linux
ifeq ($(UNAME), Linux)
TINYLINK += -lX11 -lGL
endif

## Detect MacOS
ifeq ($(UNAME), Darwin)
TINYLINK += -framework OpenGL -DTINYENGINE_OS_MAC
LF += -I/opt/homebrew/include -L/opt/homebrew/lib
CC = g++-12 -std=c++17
endif

CPP_SRCS = $(shell find . -iname "*.cpp" | grep -v "/tests/")
CPP_OBJS = $(patsubst %.cpp,%.o,$(CPP_SRCS))

all: main.cpp $(CPP_SRCS)
			$(CC) $(EXTRA_FLAGS) $(CPP_SRCS) $(CF) $(LF) -c
			$(CC) $(EXTRA_FLAGS) $(CPP_OBJS) $(CF) $(LF) -lTinyEngine $(TINYLINK) -o main

#all: main.cpp $(CPP_SRCS)
#			$(CC) $(EXTRA_FLAGS) main.cpp $(CF) $(LF) -lTinyEngine $(TINYLINK) -o main


DAT = shader			#data directory to embed
.PHONY: embedded
embedded: CEF = $(shell c-embed $(DAT)) c-embed.o -include /usr/local/include/c-embed.h -DCEMBED_TRANSLATE
embedded:
	$(CC) main.cpp $(CF) $(LF) -lTinyEngine $(TINYLINK) -o main ${CEF}
	rm c-embed.o
