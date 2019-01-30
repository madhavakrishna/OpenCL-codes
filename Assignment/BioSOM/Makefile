#/bin/bash

OUTNAME := $(notdir $(shell pwd))

CC ?= gcc
CPPFLAGS += -Wall -O3

LDFLAGS = -O3 
LDLIBS = -lm

objects = BioSOM.o BioSOM_Training.o funcs.o
	
$(OUTNAME).elf : $(objects)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS) 

%.o : %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

BioSOM_Training.o : BioSOM_Training.c defs.h funcs.h
BioSOM.o : BioSOM.c defs.h funcs.h
funcs.o : funcs.c

.PHONY : clean
clean : 
	-rm -f *.elf *.o

