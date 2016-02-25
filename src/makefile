CXX := g++
CXXFLAGS := -O3

OBJECTS := BfProgram.o FastProgram.o Optimizer.o RuntimeEnvironment.o VeryFastProgram.o main.o
EXECUTABLE := qf

all: $(EXECUTABLE)

debug: CXXFLAGS := -O0 -g
debug: all

install: $(EXECUTABLE)
	install $(EXECUTABLE) /usr/bin/$(EXECUTABLE)

.PHONY: clean
clean:
	rm $(EXECUTABLE) $(OBJECTS)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@

%.o: %.cpp
	$(CXX) $< -c -o $@ $(CXXFLAGS)


