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

