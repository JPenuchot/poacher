# Project version
NAME    = poacher
VERSION = 0.0.1

CLANG_FORMAT_STYLE = Mozilla

# Paths
PREFIX    ?= /usr/local
MANPREFIX ?= $(PREFIX)/share/man

# Compiler/Linker
CXX ?= clang++

CPP_FLAGS += -O3 -march=native -fPIC
CPP_FLAGS += -DVERSION=\"$(VERSION)\"
CPP_FLAGS += -Wall -Wextra -Werror -Wnull-dereference -Wold-style-cast\
               -Wdouble-promotion -Wshadow

CPP_FLAGS += -std=c++2a

INCLUDES += -Iinclude
CPP_FLAGS += $(INCLUDES)

LDFLAGS += -O3
LDFLAGS += -lm
