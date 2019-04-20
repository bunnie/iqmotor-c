CC=g++
CFLAGS=-std=gnu++0x
INCLUDES=-Iiq-module-communication-cpp/src -Iiq-module-communication-cpp/inc

OUTPUTNAME=iq_demo

OBJS=iq-module-communication-cpp/src/generic_interface.o iq-module-communication-cpp/inc/client_communication.o iq-module-communication-cpp/src/byte_queue.o iq-module-communication-cpp/src/crc_helper.o iq-module-communication-cpp/src/packet_finder.o iqmotor.o

default: simplest_demo

# How to build an object file (x.o) from a corresponding C source file (x.c)
.c.o:
	$(CC) $(INCLUDES) $(CFLAGS) -c -o $@ $<

# How to build an object file from a corresponding C++ source file (x.cpp)
.cpp.o:
	$(CXX) $(INCLUDES) $(CFLAGS) -c -o $@ $<

simplest_demo: simplest_demo.o $(OBJS) 
	$(CC) $(CFLAGS) $(INC) -o $(OUTPUTNAME) $(OBJS) $@.o

turn_demo: turn_demo.o $(OBJS) 
	$(CC) $(CFLAGS) $(INC) -o $(OUTPUTNAME) $(OBJS) $@.o

copy_demo: copy_demo.o $(OBJS) 
	$(CC) $(CFLAGS) $(INC) -o $(OUTPUTNAME) $(OBJS) $@.o

.PHONY: clean
clean:
	rm *.o
	rm iq-module-communication-cpp/src/*.o
	rm iq-module-communication-cpp/inc/*.o
	rm $(OUTPUTNAME)
