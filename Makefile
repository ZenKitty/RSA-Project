EXE = rsa
CFLAGS = -Wall
CXXFLAGS = -Wall
LDLIBS =
CC = gcc
CXX = g++

.PHONY: all
all: $(EXE)

# Implicit rules defined by Make, but you can redefine if needed
#
#rsa: rsa.cpp
#	$(CXX) $(CXXFLAGS) rsa.cpp $(LDLIBS) -o rsa

.PHONY: clean
clean:
	rm -f $(EXE)