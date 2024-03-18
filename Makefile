CXX = gcc
CXXFLAGS = -g -Wpedantic -Wall -Wno-unused-result -Wextra -Werror=uninitialized -Wvla -Werror=return-type
VAL = valgrind -q --tool=memcheck --track-origins=no --error-exitcode=1 --track-origins=yes
SRC = *.c
TARGET = program
PAVANE = pavane.txt
DRUM = drums.txt
CHANGE = change.txt
CRASH = crash.txt
SNARE = snare.txt

all:
	$(CXX) $(SRC) -o$(TARGET) $(CXXFLAGS) -lm -O
	./$(TARGET) < pieces/$(PAVANE)
	rm $(TARGET)

b:
	$(CXX) $(SRC) -o$(TARGET) $(CXXFLAGS) -lm -O
