# Thanks to https://rhthomas.github.io/post/avr-make/

TARGET = timestamper

# Programmer serial port
PROGTTY = /dev/ttyCH340USB0

# device parameters
DEVICE     = atmega328p
F_CPU      = 16000000
#F_CPU      = 8000000

# compilers
CC      = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude -b57600 -P $(PROGTTY) -c avrisp -p m328p
#CFLAGS  = -mmcu=$(DEVICE) -Os -Wall -std=c99 -DF_CPU=$(F_CPU)
CFLAGS  = -O2 -std=c99 -mmcu=$(DEVICE) -DF_CPU=$(F_CPU) -Wa,-ah,-L

#--------------------------------------
# No need to edit the below code
#--------------------------------------

# source files and object directory
SOURCES = $(wildcard *.c)
OBJECTS = $(patsubst %.c, %.o, $(SOURCES))

# get file dependencies then build hex
all: Makefile.deps $(TARGET).hex

# include object dependencies
-include Makefile.deps

$(TARGET).elf: $(OBJECTS)
	$(CC) -mmcu=$(DEVICE) -o $@ $^

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

# generate object dependencies
Makefile.deps: $(SOURCES)
	$(CC) $(CFLAGS) -MM $^ > Makefile.deps

# remove build files
clean:
	@rm -f *.o $(TARGET).elf $(TARGET).hex Makefile.deps

# flash to micro-controller
flash: $(TARGET).hex
	$(AVRDUDE) -U flash:w:$<

