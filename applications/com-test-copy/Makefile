include ../shared.mk

C_SOURCES:=$(wildcard *.c)
# CFLAGS+=-Werror
CFLAGS += $(shell pkg-config --cflags json-c)
LDFLAGS += $(shell pkg-config --libs json-c)

include ../end.mk

