include ../../build/software/include/generated/variables.mak
include $(SOC_DIRECTORY)/software/common.mak

CC=riscv64-unknown-elf-gcc
CXX=riscv64-unknown-elf-gcc

CFLAGS=-MD -MP -Os -march=rv32im -mabi=ilp32 -D__vexriscv__ -fomit-frame-pointer -Wall -fno-builtin -nostdinc -fexceptions -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes 
INCLUDES=-I/home/bunnie/code/zappy-fpga/deps/litex/litex/soc/software/include/base -I/home/bunnie/code/zappy-fpga/deps/litex/litex/soc/software/include -I/home/bunnie/code/zappy-fpga/deps/litex/litex/soc/common -I/home/bunnie/code/zappy-fpga/build/software/include -I. -I..
AR=riscv64-unknown-elf-ar

OBJS=client_communication.o byte_queue.o crc_helper.o packet_finder.o iqmotor.o bipbuffer.o multi_turn_angle_control_client.o power_monitor_client.o

default: libiq.a

# -std=gnu99   gnu++0x
# How to build an object file (x.o) from a corresponding C source file (x.c)
.c.o:
	$(CC) $(INCLUDES) -std=gnu99 $(CFLAGS) -c -o $@ $<

# How to build an object file from a corresponding C++ source file (x.cpp)
.cpp.o:
	$(CXX) $(INCLUDES) -std=gnu99 $(CFLAGS) -c -o $@ $<

libiq.a: $(OBJS) 
	$(AR) clr libiq.a $(OBJS)

.PHONY: clean
clean:
	rm -r *.o
	rm -r *.a
