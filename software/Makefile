## This file is part of the TimeStamper distribution (https://github.com/tomwimmenhove/timestamper)
## Copyright (c) 2019 Tom wimmenhove
## 
## This program is free software: you can redistribute it and/or modify  
## it under the terms of the GNU General Public License as published by  
## the Free Software Foundation, version 3.
##
## This program is distributed in the hope that it will be useful, but 
## WITHOUT ANY WARRANTY; without even the implied warranty of 
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
## General Public License for more details.
##
## You should have received a copy of the GNU General Public License 
## along with this program. If not, see <http://www.gnu.org/licenses/>.
##

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

