CXX = g++
CXXFLAGS = -g -Wall -pedantic -std=c++17

# List all source files here
SRCS = main.cpp Cache.cpp
OBJS = $(SRCS:.cpp=.o)

# Files to submit to Gradescope (if applicable)
FILES_TO_SUBMIT = $(shell ls *.cpp *.h README.txt 2> /dev/null)

# Rule for compiling .cpp to .o
%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Default target (when typing "make") builds the executable csim
csim : $(OBJS)
	$(CXX) -o $@ $(OBJS)

# Alias target so "make csim" works too
all: csim

# Target to create a solution.zip file for Gradescope submission
.PHONY: solution.zip
solution.zip :
	zip -9r $@ $(FILES_TO_SUBMIT)

# Generate header file dependencies
depend :
	$(CXX) $(CXXFLAGS) -M $(SRCS) > depend.mak

depend.mak :
	touch $@

# Clean target removes executable and object files
clean :
	rm -f csim *.o

# Include dependency file if it exists
-include depend.mak

