CC=g++
CFLAGS= -std=gnu++0x

OUTPUTNAME=iq_demo

# add all source files here but with '.o' instead of '.c'
OBJS=bidir_copy_demo.o generic_interface.o byte_queue.o crc_helper.o packet_finder.o iqmotor.o

default: all

all: $(OBJS)
	$(CC) $(CFLAGS) -o $(OUTPUTNAME) $(OBJS)

debug: $(OBJS)
	$(CC) $(CFLAGS) -g -o $(OUTPUTNAME) $(OBJS)

opt: $(OBJS)
	$(CC) $(CFLAGS) -O3 -o $(OUTPUTNAME) $(OBJS)

.PHONY: clean
clean:
	rm *.o
	rm $(OUTPUTNAME)
