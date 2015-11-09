##############################################################################
#       CS 143 Project Fall 2015
#       Jordan, Luciana, Vincent, Vivian
###############################################################################
CC = g++
FLAGS = -g -std=c++11

# The following line is a relative directory reference that assumes the Eigen
# folder--which your program will depend on--is located one directory above the
# directory that contains this Makefile.
INCLUDE = 
SOURCES = *.cpp

EXENAME = proj

all: $(SOURCES)
	$(CC) $(FLAGS) -o $(EXENAME) $(INCLUDE) $(SOURCES)

clean:
	rm -f *.o $(EXENAME)
.PHONY: all clean


