COMPILE_FLAG = g++
DEBUG = -g
CFLAGS = -Wall -c $(Debug)
LFLAGS = -o 
PARTITION = Partitioner
DISTRIBUTOR = Distributor
KERNEL = Kernel
TCL = TCL_Shell
SFLAG = -Wall -shared

INCLUDE = -I/home/singhs/work/Boost/boost_1_33_1 \
	  -I/usr/include/tcl \
	  -I $(PARTITION) -I $(DISTRIBUTOR) -I $(KERNEL) -I $(TCL)

LINK = -L/usr/lib/libtcl8.5.so


#Source Files

PARTITION_SRCS = $(PARTITION)/partition.cpp
DISTRIBUTOR_SRCS = $(DISTRIBUTOR)/distributor.cpp
KERNEL_SRCS = $(KERNEL)/kernel.cpp
TCL_SRCS = $(TCL)/cpptcl.cc $(TCL)/diGAnaTCL.cpp


#Source Headers

PARTITION_HDRS = $(PARTITION)/partition.hxx
DISTRIBUTOR_HDRS = $(DISTRIBUTOR)/distributor.hxx
KERNEL_HDRS = $(KERNEL)/kernel.hxx
TCL_HDRS = $(TCL)/cpptcl.h

all:
	$(COMPILE_FLAG) $(INCLUDE) $(LINK) $(SFLAGS) $(TCL_SRCS) \
		                                     $(PARTITION_SRCS) \
		                                     $(DISTRIBUTOR_SRCS) \
		                                     $(KERNEL_SRCS) \
						     $(LFLAGS) mymodule.so

partition.o: $(PARTITION_SRCS)
	$(COMPILE_FLAG) $(INCLUDE) $(CFLAGS) $(PARTITION_SRCS) $(LFLAGS) partition.o

distributor.o: $(DISTRIBUTOR_SRCS)
	$(COMPILE_FLAG) $(INCLUDE) $(CFLAGS) $(DISTRIBUTOR_SRCS) $(LFLAGS) distributor.o

kernel.o: $(KERNEL_SRCS)
	$(COMPILE_FLAG) $(INCLUDE) $(CFLAGS) $(KERNEL_SRCS) $(LFLAGS) kernel.o

clean:
	rm -rf partition.o distributor.o kernel.o mymodule.so
