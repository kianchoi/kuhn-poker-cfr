//Edited by Kian Choi 9/28/26

#include <iostream>
#include <stdexcept>
#include <cstddef>
#include <array>
#include "GameState.hpp"
#include "InfoSetTable.hpp"
#include "Solve.hpp"
#include "Evaluator.hpp"

int main() {
    //print strategy, starting EV, best responses, nash convergence, and exploitability for solves trained for 'iterations' iterations
    Solve solver;
    Evaluator evaluator;

    //iteration counts used to demonstrate CFR convergence
    //change this array to train models on different iteration counts
    std::array<int, 7> iterationCounts = {1, 10, 100, 1000, 10000, 100000, 1000000};

    //print startingEV, best response EV, exploitability for solves trained on varying numbers of iterations
    for (int iterations : iterationCounts) {

        if (iterations < 0) {
            throw std::runtime_error("Cannot run for negative iterations.");
        }
    
        InfoSetTable iTable;
        solver.train(iTable, iterations);

        if (iterations == 1) {
             std::cout << "Ran for 1 iteration:" << '\n';
        }
        else {
            std::cout << "Ran for " << iterations << " iterations:" << '\n';
        }

        //updateNodeEvalAndPrintStartingEV also populates NodeEvaluation HashMap
        evaluator.updateNodeEvalAndPrintStartingEV(iTable, true);
    
        //prints EVs where the solver perfectly exploits the fixed strategy of the other player
        evaluator.printBestResponses(iTable);

        //prints the exploitability metric, half the nash convergence metric and a test of how close to equilibrium the solution is
        evaluator.printConvergenceMetrics(iTable);

        std::cout << '\n';
    }

    //train one final strategy for detailed output

    //change this variable if you want to adjust the iteration count
    int iterations = 1000000;
    InfoSetTable iTable;

    if (iterations < 0) {
            throw std::runtime_error("Cannot run for negative iterations.");
    }

    solver.train(iTable, iterations);

    if (iterations == 1) {
            std::cout << "Final Strategy (Trained for 1 iteration):" << '\n';
    }
    else {
            std::cout << "Final Strategy (Trained for " << iterations << " iterations):" << '\n';
    }  

    //updateNodeEvalAndPrintStartingEV also populates NodeEvaluation HashMap
    evaluator.updateNodeEvalAndPrintStartingEV(iTable, false);
    iTable.printStrategy();

    std::cout << '\n' << "Final Information-Set EVs:" << '\n';
    evaluator.printEVs();

    return 0;
}