
all: lib misc scalar vec table struct section nsection vec-section user-mat large gen_perf perf

include ../Makefile.inc

DEPS = ../src/libscapar.a

SRC   = $(wildcard *.c)
DIR   = $(notdir $(SRC))
OBJS  = $(patsubst %.c,%.o,$(DIR))
BIN   = $(patsubst %.c,%,$(DIR))


misc.o: misc.c $(DEPS)
scalar.o: scalar.c $(DEPS)
vec.o: vec.c $(DEPS)
table.o: table.c $(DEPS)
struct.o: struct.c $(DEPS)
section.o: section.c $(DEPS)
nsection.o: nsection.c $(DEPS)
vec-section.o: vec-section.c $(DEPS)
user-mat.o: user-mat.c $(DEPS)
large.o: large.c $(DEPS)
gen_perf.o: gen_perf.c $(DEPS)
perf.o: perf.c $(DEPS)

lib:
	@(cd ../src; make)

clean:
	rm -fv *.o core.* $(BIN)

mem:
	@(./mem.sh)
