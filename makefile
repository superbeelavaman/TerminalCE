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

include $(shell cedev-config --makefile)

FONTPACKNAME = termFont
FONTDIR = $(SRCDIR)/gfx
DEPS = $(BINDIR)/$(FONTPACKNAME).8xv

$(BINDIR)/$(FONTPACKNAME).8xv: $(SRCDIR)/gfx/$(FONTPACKNAME).bin
	$(Q)convbin --iformat bin --input $(SRCDIR)/gfx/$(FONTPACKNAME).bin --oformat 8xv --output $(BINDIR)/$(FONTPACKNAME).8xv --archive --name $(FONTPACKNAME)
