# File: Standard Makefile for CSC415
#
# Description - This make file should be used for all your projects
# It should be modified as needed for each homework
#
# ROOTNAME should be set you your lastname_firstname_HW.  Except for
# and group projects, this will not change throughout the semester
#
# HW should be set to the assignment number (i.e. 1, 2, 3, etc.)
#
# FOPTION can be set to blank (nothing) or to any thing starting with an 
# underscore (_).  This is the suffix of your file name.
#
# With these three options above set your filename for your homework
# assignment will look like:  bierman_robert_HW1_main.c 
#
# RUNOPTIONS can be set to default values you want passed into the program
# this can also be overridden on the command line
#
# OBJ - You can append to this line for additional files necessary for
# your program, but only when you have multiple files.  Follow the convention
# but hard code the suffix as needed.
#
# To Use the Makefile - Edit as above
# then from the command line run:  make
# That command will build your program, and the program will be named the same
# as your main c file without an extension.
#
# You can then execute from the command line: make run
# This will actually run your program
#
# Using the command: make clean
# will delete the executable and any object files in your directory.
#


ROOTNAME=fsshell
HW=
FOPTION=
RUNOPTIONS=SampleVolume 10000000 512
CC=gcc
CFLAGS= -g -Iinclude
LIBS =pthread
DEPS = 
OBJ_DIR=obj

# Add any additional objects to this list
ADDOBJ= $(OBJ_DIR)/fsInit.o $(OBJ_DIR)/vcb.o $(OBJ_DIR)/freeSpace.o $(OBJ_DIR)/bitmap.o $(OBJ_DIR)/dirEntry.o $(OBJ_DIR)/parsePath.o $(OBJ_DIR)/dirFunc.o $(OBJ_DIR)/fs_utility.o $(OBJ_DIR)/fs_stat.o $(OBJ_DIR)/fs_delete.o $(OBJ_DIR)/b_io.o
ARCH = $(shell uname -m)

ifeq ($(ARCH), aarch64)
	ARCHOBJ=$(OBJ_DIR)/fsLowM1.o
else
	ARCHOBJ=$(OBJ_DIR)/fsLow.o
endif

OBJ = $(OBJ_DIR)/$(ROOTNAME)$(HW)$(FOPTION).o $(ADDOBJ) $(ARCHOBJ)


vpath %.c src

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: %.c $(DEPS) | $(OBJ_DIR)
	$(CC) -c -o $@ $< $(CFLAGS)

$(OBJ_DIR)/fsLow.o: fsLow.o | $(OBJ_DIR)
	cp fsLow.o $(OBJ_DIR)/fsLow.o

$(OBJ_DIR)/fsLowM1.o: fsLowM1.o | $(OBJ_DIR)
	cp fsLowM1.o $(OBJ_DIR)/fsLowM1.o
	
$(ROOTNAME)$(HW)$(FOPTION): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) -lm -l readline -l $(LIBS)

# clean:
# 	rm $(ROOTNAME)$(HW)$(FOPTION).o $(ADDOBJ) $(ROOTNAME)$(HW)$(FOPTION)

clean:
	rm -rf $(OBJ_DIR)
	rm -f $(ROOTNAME)$(HW)$(FOPTION)

run: $(ROOTNAME)$(HW)$(FOPTION)
	./$(ROOTNAME)$(HW)$(FOPTION) $(RUNOPTIONS)

vrun: $(ROOTNAME)$(HW)$(FOPTION)
	valgrind ./$(ROOTNAME)$(HW)$(FOPTION) $(RUNOPTIONS)


