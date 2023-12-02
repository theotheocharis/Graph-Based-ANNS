# Compiler and compiler flags
CXX = g++
CFLAGS = -std=c++17 -Wall -Wextra -O3

# Directories
SRC = src/
INCLUDE = include/
RESOURCES = resources/
BUILD = build/

OBJS_LSH = $(BUILD)main_lsh.o $(BUILD)parser.o $(BUILD)utils.o $(BUILD)image.o $(BUILD)hash_function.o $(BUILD)hash_table.o $(BUILD)lsh.o
SRCS_LSH = $(SRC)main_lsh.cpp $(SRC)parser.cpp $(SRC)utils.cpp $(SRC)image.cpp $(SRC)hash_function.cpp $(SRC)hash_table.cpp $(SRC)lsh.cpp

OBJS_CUBE = $(BUILD)main_cube.o $(BUILD)parser.o $(BUILD)utils.o $(BUILD)image.o $(BUILD)hash_function.o $(BUILD)hash_table.o $(BUILD)cube.o
SRCS_CUBE = $(SRC)main_cube.cpp $(SRC)parser.cpp $(SRC)utils.cpp $(SRC)image.cpp $(SRC)hash_function.cpp $(SRC)hash_table.cpp $(SRC)cube.cpp

OBJS_GRAPH = $(BUILD)main_graph.o $(BUILD)parser.o $(BUILD)utils.o $(BUILD)image.o $(BUILD)mrng.o $(BUILD)gnns.o $(BUILD)lsh.o $(BUILD)hash_function.o $(BUILD)hash_table.o
SRCS_GRAPH = $(SRC)main_graph.cpp $(SRC)parser.cpp $(SRC)utils.cpp $(SRC)image.cpp $(SRC)mrng.cpp $(SRC)gnns.cpp $(SRC)lsh.cpp $(SRC)hash_function.cpp $(SRC)hash_table.cpp

LSH = $(BUILD)lsh

CUBE = $(BUILD)cube

GRAPH = $(BUILD)graph_search

# Objects
$(BUILD)main_lsh.o: $(SRC)main_lsh.cpp
	$(CXX) -o $(BUILD)main_lsh.o $(CFLAGS) -c $(SRC)main_lsh.cpp

$(BUILD)main_cube.o: $(SRC)main_cube.cpp
	$(CXX) -o $(BUILD)main_cube.o $(CFLAGS) -c $(SRC)main_cube.cpp

$(BUILD)main_graph.o: $(SRC)main_graph.cpp
	$(CXX) -o $(BUILD)main_graph.o $(CFLAGS) -c $(SRC)main_graph.cpp

$(BUILD)parser.o: $(SRC)parser.cpp $(INCLUDE)parser.hpp
	$(CXX) -o $(BUILD)parser.o $(CFLAGS) -c $(SRC)parser.cpp

$(BUILD)utils.o: $(SRC)utils.cpp $(INCLUDE)utils.hpp
	$(CXX) -o $(BUILD)utils.o $(CFLAGS) -c $(SRC)utils.cpp

$(BUILD)image.o: $(SRC)image.cpp $(INCLUDE)image.hpp
	$(CXX) -o $(BUILD)image.o $(CFLAGS) -c $(SRC)image.cpp

$(BUILD)hash_function.o: $(SRC)hash_function.cpp $(INCLUDE)hash_function.hpp
	$(CXX) -o $(BUILD)hash_function.o $(CFLAGS) -c $(SRC)hash_function.cpp

$(BUILD)hash_table.o: $(SRC)hash_table.cpp $(INCLUDE)hash_table.hpp
	$(CXX) -o $(BUILD)hash_table.o $(CFLAGS) -c $(SRC)hash_table.cpp

$(BUILD)cube.o: $(SRC)cube.cpp $(INCLUDE)cube.hpp
	$(CXX) -o $(BUILD)cube.o $(CFLAGS) -c $(SRC)cube.cpp

$(BUILD)lsh.o: $(SRC)lsh.cpp $(INCLUDE)lsh.hpp
	$(CXX) -o $(BUILD)lsh.o $(CFLAGS) -c $(SRC)lsh.cpp

$(BUILD)mrng.o: $(SRC)mrng.cpp $(INCLUDE)mrng.hpp
	$(CXX) -o $(BUILD)mrng.o $(CFLAGS) -c $(SRC)mrng.cpp

$(BUILD)gnns.o: $(SRC)gnns.cpp $(INCLUDE)gnns.hpp
	$(CXX) -o $(BUILD)gnns.o $(CFLAGS) -c $(SRC)gnns.cpp

# Executables
$(LSH): $(OBJS_LSH)
	$(CXX) $(CFLAGS) -o $(LSH) $(OBJS_LSH)

$(CUBE): $(OBJS_CUBE)
	$(CXX) $(CFLAGS) -o $(CUBE) $(OBJS_CUBE)

$(GRAPH): $(OBJS_GRAPH)
	$(CXX) $(CFLAGS) -o $(GRAPH) $(OBJS_GRAPH)

all: $(CUBE) $(LSH) $(GRAPH)

clean:
	rm -f $(OBJS_LSH) $(OBJS_CUBE) $(OBJS_GRAPH) $(LSH) $(CUBE) $(GRAPH)

lsh: $(LSH)
	./$(LSH) -d resources/input.dat -q resources/query.dat -o resources/outputLSH.txt

cube: $(CUBE)
	./$(CUBE) -d resources/input.dat -q resources/query.dat -o resources/outputCube.txt

graph1: $(GRAPH)
	./$(GRAPH) -d resources/input.dat -q resources/query.dat -k 50 -E 30 -R 3 -N 5 -m 1 -o resources/outputGraph.txt

graph2: $(GRAPH)
	./$(GRAPH) -d resources/input.dat -q resources/query.dat -k 50 -E 30 -R 1 -N 1 -l 20 -m 2 -o resources/outputGraph.txt