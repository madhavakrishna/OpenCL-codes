#/bin/bash

OUTNAME := $(notdir $(shell pwd))

CC ?= gcc
CPPFLAGS += -Wall -O3

LDFLAGS = -O3 
LDLIBS = -lm

objects = floyd_basic.o
	
$(OUTNAME).elf : $(objects)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS) 

%.o : %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

floyd_basic.o : floyd_basic.c

.PHONY : clean
clean : 
	-rm -f *.elf *.o

