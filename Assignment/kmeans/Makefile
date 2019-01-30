#/bin/bash

OUTNAME := $(notdir $(shell pwd))

CC ?= gcc
CPPFLAGS += -Wall -O3

LDFLAGS = -O3 
LDLIBS = -lm

objects = kmeans.o
	
$(OUTNAME).elf : $(objects)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS) 

%.o : %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

kmeans.o : kmeans.c 

.PHONY : clean
clean : 
	-rm -f *.elf *.o

run :$(OUTNAME).elf
	./$(OUTNAME).elf > out.eps
 
