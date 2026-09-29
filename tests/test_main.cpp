//Edited by Kian Choi 9/28/26

#include "GameStateTests.hpp"
#include "InfoSetTests.hpp"
#include "SolveTests.hpp"
#include "EvaluatorTests.hpp"

int main() {

    //GameState tests
    GameStateTests gTests;
    gTests.testExceptions();
    gTests.testLegalTransitionsAndPayoffs();
    gTests.testStateQueries();

    //InfoSetTable tests
    InfoSetTests iTests;
    iTests.testLegalQueries();
    iTests.testInfoSetTable();
    iTests.testInfoSetExceptions();
    iTests.testAdditionalInfoSetExceptions();

    //Solve tests
    SolveTests sTests;
    sTests.testRegretMatch();
    sTests.testUpdateStrategySum();
    sTests.testAddRegretsToTable();

    //Evaluator tests
    EvaluatorTests eTests;
    eTests.testStartingEV();
    eTests.testGetInfoSetEV();
    eTests.testBestResponse();
    eTests.testConvergence();

}