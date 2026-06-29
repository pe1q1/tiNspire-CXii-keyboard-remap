DEBUG ?= FALSE
GCC = nspire-gcc
LD = nspire-ld
GENZEHN = genzehn

EXE = qwerty_keymap
OBJS = qwerty_keymap.o
GCCFLAGS = -Wall -Wextra -marm -std=c99
LDFLAGS = -Wl,--nspireio
ZEHNFLAGS = --name "qwerty_keymap"
DISTDIR = build

ifeq ($(DEBUG),FALSE)
GCCFLAGS += -Os
else
GCCFLAGS += -O0 -g
endif

all: $(DISTDIR)/$(EXE).tns

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $< -o $@

$(DISTDIR)/$(EXE).elf: $(OBJS)
	mkdir -p $(DISTDIR)
	$(LD) $^ -o $@ $(LDFLAGS)

$(DISTDIR)/$(EXE).tns: $(DISTDIR)/$(EXE).elf
	$(GENZEHN) --input $< --output $@.zehn $(ZEHNFLAGS)
	make-prg $@.zehn $@
	rm $@.zehn

clean:
	rm -f $(OBJS) $(DISTDIR)/$(EXE).tns $(DISTDIR)/$(EXE).elf $(DISTDIR)/$(EXE).tns.zehn

.PHONY: all clean
