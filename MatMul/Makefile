#/bin/bash

OUTNAME := $(notdir $(shell pwd))

CC ?= gcc
CPPFLAGS += -Wall -O3

LDFLAGS = -O3 
LDLIBS = -lOpenCL

objects = matmul.o common.o
	
$(OUTNAME).elf : $(objects)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS) 

%.o : %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

matmul.o : matmul.c def.h 
common.o : common.c

.PHONY : clean 
clean : 
	-rm -f *.elf *.o 
