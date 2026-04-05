CC ?= gcc
CFLAGS ?= -O2 -Wall -Wextra -std=c17
INCLUDES = -I../src
SRC = ../src/alignment.c ../src/instance_io.c ../src/DATA_struct.c

TARGETS = small_tests testingNM_ALL_INST testingDP_ALL_INST DIST1vsDIST2

all: $(TARGETS)

small_tests: small_tests.c $(SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $< $(SRC)

testingNM_ALL_INST: testingNM_ALL_INST.c $(SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $< $(SRC)

testingDP_ALL_INST: testingDP_ALL_INST.c $(SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $< $(SRC)

DIST1vsDIST2: DIST1vsDIST2.c $(SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $< $(SRC)

clean:
	rm -f $(TARGETS) *.exe *.o

.PHONY: all clean

