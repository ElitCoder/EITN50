CC		:= gcc
CFLAGS	:= -Wall
CLAGS	+= -O3
LDLIBS	:= -ltspi

all: clean ass9
	./ass9
ass9: ass9.o
ass9.o: ass9.c

clean:
	rm -f ass9.o ass9