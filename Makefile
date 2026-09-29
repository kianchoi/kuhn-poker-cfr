#Edited by Kian Choi 9/28/26

CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++20 -O2 -Iinclude -Itests
SOLVER = build/kuhn_solver
TESTS = build/kuhn_test

all: $(SOLVER)

$(TESTS): build/test_main.o build/GameState.o build/InfoSetTable.o build/Solve.o build/Evaluator.o build/GameStateTests.o build/InfoSetTests.o build/SolveTests.o build/EvaluatorTests.o
	$(CXX) $(CXXFLAGS) -o $@ $^

$(SOLVER): build/main.o build/GameState.o build/InfoSetTable.o build/Solve.o build/Evaluator.o
	$(CXX) $(CXXFLAGS) -o $@ $^

build/main.o: src/main.cpp include/GameState.hpp include/InfoSetTable.hpp include/Solve.hpp include/Evaluator.hpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c src/main.cpp -o $@

build/test_main.o: tests/test_main.cpp tests/GameStateTests.hpp tests/InfoSetTests.hpp tests/SolveTests.hpp tests/EvaluatorTests.hpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c tests/test_main.cpp -o $@

build/GameState.o: src/GameState.cpp include/GameState.hpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c src/GameState.cpp -o $@

build/GameStateTests.o: tests/GameStateTests.cpp tests/GameStateTests.hpp include/GameState.hpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c tests/GameStateTests.cpp -o $@

build/InfoSetTable.o: src/InfoSetTable.cpp include/InfoSetTable.hpp include/GameState.hpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c src/InfoSetTable.cpp -o $@

build/InfoSetTests.o: tests/InfoSetTests.cpp tests/InfoSetTests.hpp include/InfoSetTable.hpp include/GameState.hpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c tests/InfoSetTests.cpp -o $@

build/Solve.o: src/Solve.cpp include/Solve.hpp include/InfoSetTable.hpp include/GameState.hpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c src/Solve.cpp -o $@

build/SolveTests.o: tests/SolveTests.cpp tests/SolveTests.hpp include/InfoSetTable.hpp include/GameState.hpp include/Solve.hpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c tests/SolveTests.cpp -o $@

build/Evaluator.o: src/Evaluator.cpp include/Solve.hpp include/InfoSetTable.hpp include/GameState.hpp include/Evaluator.hpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c src/Evaluator.cpp -o $@

build/EvaluatorTests.o: tests/EvaluatorTests.cpp include/InfoSetTable.hpp include/GameState.hpp include/Solve.hpp include/Evaluator.hpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c tests/EvaluatorTests.cpp -o $@

.PHONY: all run test clean

run: $(SOLVER)
	./$(SOLVER)

test: $(TESTS)
	./$(TESTS)

clean:
	rm -rf build