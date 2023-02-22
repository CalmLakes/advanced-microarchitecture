CC = g++
OPT = -O3
GDB = -g
#OPT = -g

INC = -I.
LIB = -L.

CFLAGS = $(OPT) $(INC) $(LIB) $(GDB) -Wall

LIBS = -l721sim -lpthread -lz -ldl 

SOURCES := $(wildcard *.cc)
OBJECTS := $(patsubst %.cc, %.o, $(SOURCES))

#################################

all: $(OBJECTS)
	$(CC) $(CFLAGS) -o 721sim $^ $(LIBS)
	@echo "-----------DONE WITH 721sim-----------"

%.o: %.cc
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o 721sim core
clobber:
	rm -f *.o

init:
	source /mnt/designkits/spec_2006_2017/O2_fno_bbreorder/activate.bash

build_dir:
	mkdir 473.astar_rivers_ref.252.0.28.gz-1; cd 473.astar_rivers_ref.252.0.28.gz-1
	ln -s /mnt/designkits/spec_2006_2017/O2_fno_bbreorder/app_storage/pk
	atool-simenv mkgen 473.astar_rivers_ref --checkpoint 473.astar_rivers_ref.252.0.28.gz

