# ----------------------------
# Makefile Options
# ----------------------------

NAME = DEMO
ICON = icon.png
DESCRIPTION = "CE C Toolchain Demo"
COMPRESSED = NO

CFLAGS = -Wall -Wextra -Oz
CXXFLAGS = -Wall -Wextra -Oz

# ----------------------------

all: build-font

include $(shell cedev-config --makefile)

FONTPACKNAME = TermFont
FONTDIR = $(SRCDIR)/gfx

SHELL := /bin/bash

build-font:
	python3 scripts/convert-fonttiles.py $(FONTDIR)/$(FONTPACKNAME).png $(BINDIR)/$(FONTPACKNAME).bin
	convbin --iformat bin --oformat 8xv --input $(BINDIR)/$(FONTPACKNAME).bin --output $(BINDIR)/$(FONTPACKNAME).8xv --name $(FONTPACKNAME)
