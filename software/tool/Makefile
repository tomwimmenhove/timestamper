src = $(wildcard *.c)
obj = $(src:.c=.o)

CFLAGS = -O2 -Wall
LDFLAGS = 

all: skipaheadus

skipaheadus: $(obj)
	    $(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) skipaheadus

