# File: Standard Makefile for CSC415
#
# Using the command: make clean
# will delete the executable and any object files in your directory.
#


ROOTNAME=hexdump
HW=
FOPTION=
RUNOPTIONS=
CC=gcc
CFLAGS= -g -I.
LIBS =pthread
DEPS = 
OBJ = $(ROOTNAME)$(HW)$(FOPTION).o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) 

$(ROOTNAME)$(HW)$(FOPTION): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) -l $(LIBS)

clean:
	rm *.o $(ROOTNAME)$(HW)$(FOPTION)

run: $(ROOTNAME)$(HW)$(FOPTION)
	./$(ROOTNAME)$(HW)$(FOPTION) $(RUNOPTIONS)
