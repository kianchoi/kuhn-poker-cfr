//Edited by Kian Choi 9/28/26

#include <iostream>
#include <cmath>

#include "SolveTests.hpp"
#include "Solve.hpp"
#include "InfoSetTable.hpp"
#include "GameState.hpp"

namespace {
    constexpr double EPSILON = 1e-9;

    bool approximatelyEqual(double a, double b)
    {
        return std::abs(a - b) < EPSILON;
    }
}

void SolveTests::testRegretMatch()
{
    using Card = GameState::Card;
    using Action = GameState::Action;
    using History = GameState::ActionHistory;

    int passed = 0;
    int failed = 0;

    Solve solver;

    //Test 1:
    //both regrets are 0 -> uniform strategy

    {
        InfoSetTable table;
        GameState state(Card::J, Card::Q, History::Start);

        Solve::strategy s =
            solver.regretMatch(table, state, Card::J);

        if (approximatelyEqual(s.checkFreq, 0.5) &&
            approximatelyEqual(s.betFreq, 0.5)) {

            std::cout << "PASS: zero regrets -> uniform strategy\n";
            passed++;
        }
        else {
            std::cout << "FAIL: zero regrets -> uniform strategy\n";
            failed++;
        }
    }

    //Test 2:
    //both regrets negative -> uniform strategy
    {
        InfoSetTable table;

        table.addWeightedRegret(
            History::Start, Card::J, Action::Check, -10.0);

        table.addWeightedRegret(
            History::Start, Card::J, Action::Bet, -20.0);

        GameState state(Card::J, Card::Q, History::Start);

        Solve::strategy s =
            solver.regretMatch(table, state, Card::J);

        if (approximatelyEqual(s.checkFreq, 0.5) &&
            approximatelyEqual(s.betFreq, 0.5)) {

            std::cout << "PASS: negative regrets -> uniform strategy\n";
            passed++;
        }
        else {
            std::cout << "FAIL: negative regrets -> uniform strategy\n";
            failed++;
        }
    }

    // Test 3:
    // Check regret = 3
    // Bet regret = 1

    // Expected:
    // Check = 3/4
    // Bet = 1/4

    {
        InfoSetTable table;

        table.addWeightedRegret(
            History::Start, Card::J, Action::Check, 3.0);

        table.addWeightedRegret(
            History::Start, Card::J, Action::Bet, 1.0);

        GameState state(Card::J, Card::Q, History::Start);

        Solve::strategy s =
            solver.regretMatch(table, state, Card::J);

        if (approximatelyEqual(s.checkFreq, 0.75) &&
            approximatelyEqual(s.betFreq, 0.25)) {

            std::cout << "PASS: positive regrets normalized correctly\n";
            passed++;
        }
        else {
            std::cout << "FAIL: positive regrets normalized incorrectly\n";
            failed++;
        }
    }


    //Test 4:
    //one positive, one negative.
 
    //regret matching ignores the negative regret.

    // Check = -5
    // Bet = 2

    // Expected:
    // Check = 0
    // Bet = 1
    {
        InfoSetTable table;

        table.addWeightedRegret(
            History::Start, Card::J, Action::Check, -5.0);

        table.addWeightedRegret(
            History::Start, Card::J, Action::Bet, 2.0);

        GameState state(Card::J, Card::Q, History::Start);

        Solve::strategy s =
            solver.regretMatch(table, state, Card::J);

        if (approximatelyEqual(s.checkFreq, 0.0) &&
            approximatelyEqual(s.betFreq, 1.0)) {

            std::cout << "PASS: negative regret ignored\n";
            passed++;
        }
        else {
            std::cout << "FAIL: negative regret not handled correctly\n";
            failed++;
        }
    }


    std::cout << "\nRegret matching tests: "
              << passed << " passed, "
              << failed << " failed.\n";
}

void SolveTests::testUpdateStrategySum()
{
    using Card = GameState::Card;
    using Action = GameState::Action;
    using History = GameState::ActionHistory;

    int passed = 0;
    int failed = 0;

    Solve solver;
    InfoSetTable table;

    GameState state(Card::J, Card::Q, History::Start);

    Solve::strategy s;

    s.checkFreq = 0.75;
    s.betFreq = 0.25;

    //expected additions if P1 reaches this information set with probability 0.4:
    //checkSum += 0.4 * 0.75 = 0.30
    //betSum   += 0.4 * 0.25 = 0.10

    solver.updateStrategySum(
        table,
        state,
        Card::J,
        0.4,
        s
    );

    double checkSum =
        table.getStrategySum(
            History::Start, Card::J, Action::Check);

    double betSum =
        table.getStrategySum(
            History::Start, Card::J, Action::Bet);


    if (approximatelyEqual(checkSum, 0.30)) {
        std::cout << "PASS: check strategy sum weighted correctly\n";
        passed++;
    }
    else {
        std::cout << "FAIL: check strategy sum = "
                  << checkSum
                  << ", expected 0.30\n";
        failed++;
    }


    if (approximatelyEqual(betSum, 0.10)) {
        std::cout << "PASS: bet strategy sum weighted correctly\n";
        passed++;
    }
    else {
        std::cout << "FAIL: bet strategy sum = "
                  << betSum
                  << ", expected 0.10\n";
        failed++;
    }


    //call it again to make sure the values accumulate rather than overwrite the previous values.
    solver.updateStrategySum(
        table,
        state,
        Card::J,
        0.4,
        s
    );

    checkSum =
        table.getStrategySum(
            History::Start, Card::J, Action::Check);

    betSum =
        table.getStrategySum(
            History::Start, Card::J, Action::Bet);


    if (approximatelyEqual(checkSum, 0.60) &&
        approximatelyEqual(betSum, 0.20)) {

        std::cout << "PASS: strategy sums accumulate correctly\n";
        passed++;
    }
    else {
        std::cout << "FAIL: strategy sums do not accumulate correctly\n";
        failed++;
    }


    std::cout << "\nStrategy sum tests: "
              << passed << " passed, "
              << failed << " failed.\n";
}

void SolveTests::testAddRegretsToTable()
{
    using Card = GameState::Card;
    using Action = GameState::Action;
    using History = GameState::ActionHistory;

    int passed = 0;
    int failed = 0;

    Solve solver;
    InfoSetTable table;

    GameState state(Card::J, Card::Q, History::Start);


    /*
    Raw regrets:

        Check: 3 - 1 = +2
        Bet:   -1 - 1 = -2

    P1's own reach is excluded.

    Counterfactual reach:
        chance reach   = 1/6
        opponent reach = 0.25

    Expected weighted regrets:

        Check:
            (1/6)(0.25)(+2) = +1/12

        Bet:
            (1/6)(0.25)(-2) = -1/12
    */

    Solve::EV ev;

    ev.stratVal = 1.0;
    ev.checkEV = 3.0;
    ev.betEV = -1.0;


    Solve::reachProb reach;

    reach.p1 = 1.0;
    reach.p2 = 0.25;


    // Start is P1's turn.
    solver.addRegretsToTable(
        ev,
        table,
        state,
        Card::J,
        reach,
        true
    );


    double checkRegret =
        table.getCumulativeRegret(
            History::Start, Card::J, Action::Check);

    double betRegret =
        table.getCumulativeRegret(
            History::Start, Card::J, Action::Bet);


    if (approximatelyEqual(checkRegret, 1.0 / 12.0)) {
        std::cout << "PASS: check regret weighted correctly\n";
        passed++;
    }
    else {
        std::cout << "FAIL: check regret = "
                  << checkRegret
                  << ", expected 0.5\n";
        failed++;
    }


    if (approximatelyEqual(betRegret, -1.0 / 12.0)) {
        std::cout << "PASS: bet regret weighted correctly\n";
        passed++;
    }
    else {
        std::cout << "FAIL: bet regret = "
                  << betRegret
                  << ", expected -0.5\n";
        failed++;
    }


    std::cout << "\nRegret update tests: "
              << passed << " passed, "
              << failed << " failed.\n";
}