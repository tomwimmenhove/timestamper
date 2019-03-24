src = $(wildcard *.c)
obj = $(src:.c=.o)

CFLAGS = -O2 -Wall
LDFLAGS = 

all: timestamper

timestamper: $(obj)
	    $(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) timestamper

