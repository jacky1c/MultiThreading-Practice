# variables
CXX = gcc
EXEC = createThreadsProgram

# leads comments in a line
# Build all: default target
all : $(EXEC)

# Separate compilation to build object files
createThreadsProgram.o : counterFunctions.c threadFunction.c createThreadsProgram.c
	$(CXX) -c createThreadsProgram.c -lpthraed

counterFunctions.o : counterFunctions.c
	$(CXX) -c counterFunctions.c -lpthraed

threadFunction.o : threadFunction.c
	$(CXX) -c threadFunction.c -lpthraed

# Linking
#demo is a target which depends upon main.o and greet.o
#"g++ main.o greet.o -o demo" is the command to produce the executable file
#You need to use a TAB before g++
$(EXEC) : createThreadsProgram.o counterFunctions.o threadFunction.o
	$(CXX) createThreadsProgram.o counterFunctions.o threadFunction.o -o $(EXEC) -lpthread

# Testing
check : all
	./$(EXEC)

# Clean up all build targets so that one may get a clean build
clean :
	rm -f *.o $(EXEC)
