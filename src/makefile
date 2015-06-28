CXX := g++
CXXFLAGS := -O3

OBJECTS := BfProgram.o FastProgram.o Optimizer.o RuntimeEnvironment.o VeryFastProgram.o main.o
EXECUTABLE := qf

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@

%.o: %.cpp
	$(CXX) $< -c -o $@ $(CXXFLAGS)


