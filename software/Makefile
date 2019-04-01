src = $(wildcard *.c)
obj = $(src:.c=.o)

CFLAGS = -O2 -Wall
LDFLAGS = -lm

ifeq ($(PREFIX),)
	PREFIX := /usr/local
endif

all: timestamper

timestamper: $(obj)
	    $(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

install: timestamper
	install -d $(DESTDIR)$(PREFIX)/bin/
	install -m 755 $< $(DESTDIR)$(PREFIX)/bin/

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/timestamper

.PHONY: clean
clean:
	rm -f $(obj) timestamper

