//Edited by Kian Choi 9/28/26

#include <iostream>
#include <cmath>
#include <vector>
#include "EvaluatorTests.hpp"
#include "Evaluator.hpp"
#include "Solve.hpp"
#include "InfoSetTable.hpp"
#include "GameState.hpp"

namespace {

constexpr double EPSILON = 1e-9;

bool approximatelyEqual(double actual,
                        double expected,
                        double epsilon = EPSILON)
{
    return std::abs(actual - expected) < epsilon;
}


//Creates a strategy table where every decision is 50/50.
//
//Start:    Check 50%, Bet 50%
//Check:    Check 50%, Bet 50%
//Bet:      Fold 50%, Call 50%
//CheckBet: Fold 50%, Call 50%
InfoSetTable createUniformStrategy() {
    using Card = GameState::Card;
    using Action = GameState::Action;
    using History = GameState::ActionHistory;

    InfoSetTable table;

    Card cards[] = {
        Card::J,
        Card::Q,
        Card::K
    };

    for (Card card : cards) {

        // Start
        table.addWeightedStrategy(
            History::Start, card, Action::Check, 0.5);

        table.addWeightedStrategy(
            History::Start, card, Action::Bet, 0.5);


        // After P1 checks
        table.addWeightedStrategy(
            History::Check, card, Action::Check, 0.5);

        table.addWeightedStrategy(
            History::Check, card, Action::Bet, 0.5);


        // Facing an initial bet
        table.addWeightedStrategy(
            History::Bet, card, Action::Fold, 0.5);

        table.addWeightedStrategy(
            History::Bet, card, Action::Call, 0.5);


        // P1 facing a bet after checking
        table.addWeightedStrategy(
            History::CheckBet, card, Action::Fold, 0.5);

        table.addWeightedStrategy(
            History::CheckBet, card, Action::Call, 0.5);
    }
    return table;
}

} //using namespace

//Test 1: Starting EV

void EvaluatorTests::testStartingEV()
{
    using Card = GameState::Card;
    using History = GameState::ActionHistory;

    int passed = 0;
    int failed = 0;

    InfoSetTable table = createUniformStrategy();
    Evaluator evaluator;

    /*
        Under a completely uniform strategy, the six deals have
        the following P1 EVs:

        JQ = -0.875
        JK = -0.875

        QJ = +1.125
        QK = -0.875

        KJ = +1.125
        KQ = +1.125

        Average:

          (-0.875 - 0.875 + 1.125
           -0.875 + 1.125 + 1.125) / 6

          = 0.125

        So the expected starting EV for P1 is +1/8.
    */

    Solve::reachProb reach;

    double totalEV = 0.0;

    GameState states[] = {
        GameState(Card::J, Card::Q, History::Start),
        GameState(Card::J, Card::K, History::Start),
        GameState(Card::Q, Card::J, History::Start),
        GameState(Card::Q, Card::K, History::Start),
        GameState(Card::K, Card::J, History::Start),
        GameState(Card::K, Card::Q, History::Start)
    };

    for (GameState& state : states) {

        totalEV += evaluator.populateNodeEval(
            table,
            state,
            reach,
            true
        );
    }

    double startingEV = totalEV / 6.0;

    if (approximatelyEqual(startingEV, 0.125)) {

        std::cout
            << "PASS: uniform strategy starting EV = 0.125\n";

        passed++;
    }
    else {

        std::cout
            << "FAIL: uniform strategy starting EV = "
            << startingEV
            << ", expected 0.125\n";

        failed++;
    }

    std::cout << "\nStarting EV tests: "
              << passed << " passed, "
              << failed << " failed.\n";
}

//Test 2:
//Information-set EV

void EvaluatorTests::testGetInfoSetEV()
{
    using Card = GameState::Card;
    using History = GameState::ActionHistory;

    int passed = 0;
    int failed = 0;

    InfoSetTable table = createUniformStrategy();
    Evaluator evaluator;

    Solve::reachProb reach;

    /*
        Populate all six hidden states first.

        This is important because getInfoSetEV() needs the
        different hidden nodes belonging to the same information
        set to already exist in nodeEval.
    */

    GameState states[] = {
        GameState(Card::J, Card::Q, History::Start),
        GameState(Card::J, Card::K, History::Start),
        GameState(Card::Q, Card::J, History::Start),
        GameState(Card::Q, Card::K, History::Start),
        GameState(Card::K, Card::J, History::Start),
        GameState(Card::K, Card::Q, History::Start)
    };

    for (GameState& state : states) {
        evaluator.populateNodeEval(
            table,
            state,
            reach,
            true
        );
    }


    /*
        Under the uniform strategy:

        P1 has J:
            EV(J,Q) = -0.875
            EV(J,K) = -0.875

            Info-set EV = -0.875


        P1 has Q:
            EV(Q,J) = +1.125
            EV(Q,K) = -0.875

            Average = +0.125


        P1 has K:
            EV(K,J) = +1.125
            EV(K,Q) = +1.125

            Info-set EV = +1.125
    */


    // The opponent card here only identifies one node.
    // getInfoSetEV() should combine the hidden nodes belonging
    // to the information set.

    GameState jState(
        Card::J,
        Card::Q,
        History::Start
    );

    GameState qState(
        Card::Q,
        Card::J,
        History::Start
    );

    GameState kState(
        Card::K,
        Card::J,
        History::Start
    );


    double jEV = evaluator.getInfoSetEV(jState);
    double qEV = evaluator.getInfoSetEV(qState);
    double kEV = evaluator.getInfoSetEV(kState);


    if (approximatelyEqual(jEV, -0.875)) {

        std::cout
            << "PASS: J starting information-set EV\n";

        passed++;
    }
    else {

        std::cout
            << "FAIL: J info-set EV = "
            << jEV
            << ", expected -0.875\n";

        failed++;
    }


    if (approximatelyEqual(qEV, 0.125)) {

        std::cout
            << "PASS: Q starting information-set EV\n";

        passed++;
    }
    else {

        std::cout
            << "FAIL: Q info-set EV = "
            << qEV
            << ", expected 0.125\n";

        failed++;
    }


    if (approximatelyEqual(kEV, 1.125)) {

        std::cout
            << "PASS: K starting information-set EV\n";

        passed++;
    }
    else {

        std::cout
            << "FAIL: K info-set EV = "
            << kEV
            << ", expected 1.125\n";

        failed++;
    }


    std::cout << "\nInformation-set EV tests: "
              << passed << " passed, "
              << failed << " failed.\n";
}

// Test 3: 
// Best responses

void EvaluatorTests::testBestResponse()
{
    using Card = GameState::Card;
    using History = GameState::ActionHistory;

    int passed = 0;
    int failed = 0;

    InfoSetTable table = createUniformStrategy();
    Evaluator evaluator;


    Solve::reachProb reach;

    GameState states[] = {
        GameState(Card::J, Card::Q, History::Start),
        GameState(Card::J, Card::K, History::Start),
        GameState(Card::Q, Card::J, History::Start),
        GameState(Card::Q, Card::K, History::Start),
        GameState(Card::K, Card::J, History::Start),
        GameState(Card::K, Card::Q, History::Start)
    };

    for (GameState& state : states) {
        evaluator.populateNodeEval(
            table,
            state,
            reach,
            true
        );
    }

    /*
        Against a completely uniform P2 strategy,
        P1's best-response EVs by private card are:

            J: -0.5
            Q: +0.5
            K: +1.5

        Average:

            (-0.5 + 0.5 + 1.5) / 3
            = 0.5
    */

    GameState jState(Card::J, History::Start);
    GameState qState(Card::Q, History::Start);
    GameState kState(Card::K, History::Start);

    double p1JBR = evaluator.getP1BestResponseEV(table, jState);
    double p1QBR = evaluator.getP1BestResponseEV(table, qState);
    double p1KBR = evaluator.getP1BestResponseEV(table, kState);

    double p2JBR = evaluator.getP2BestResponseEV(table, jState);
    double p2QBR = evaluator.getP2BestResponseEV(table, qState);
    double p2KBR = evaluator.getP2BestResponseEV(table, kState);


    if (approximatelyEqual(p1JBR, -0.5)) {
        std::cout << "PASS: P1 J best response\n";
        passed++;
    }
    else {
        std::cout << "FAIL: P1 J best response = "
                  << p1JBR
                  << ", expected -0.5\n";
        failed++;
    }


    if (approximatelyEqual(p1QBR, 0.5)) {
        std::cout << "PASS: P1 Q best response\n";
        passed++;
    }
    else {
        std::cout << "FAIL: P1 Q best response = "
                  << p1QBR
                  << ", expected 0.5\n";
        failed++;
    }


    if (approximatelyEqual(p1KBR, 1.5)) {
        std::cout << "PASS: P1 K best response\n";
        passed++;
    }
    else {
        std::cout << "FAIL: P1 K best response = "
                  << p1KBR
                  << ", expected 1.5\n";
        failed++;
    }


    /*
        Against uniform P1:

        P2 best-response EVs by private card:

            J: -0.75
            Q: +0.25
            K: +1.75

        Average:

            (-0.75 + 0.25 + 1.75) / 3
            = 5/12
    */

    if (approximatelyEqual(p2JBR, -0.75)) {
        std::cout << "PASS: P2 J best response\n";
        passed++;
    }
    else {
        std::cout << "FAIL: P2 J best response = "
                  << p2JBR
                  << ", expected -0.75\n";
        failed++;
    }


    if (approximatelyEqual(p2QBR, 0.25)) {
        std::cout << "PASS: P2 Q best response\n";
        passed++;
    }
    else {
        std::cout << "FAIL: P2 Q best response = "
                  << p2QBR
                  << ", expected 0.25\n";
        failed++;
    }


    if (approximatelyEqual(p2KBR, 1.75)) {
        std::cout << "PASS: P2 K best response\n";
        passed++;
    }
    else {
        std::cout << "FAIL: P2 K best response = "
                  << p2KBR
                  << ", expected 1.75\n";
        failed++;
    }


    /*
        This also gives us independently known metrics:

        P1 BR = 0.5
        P2 BR = 5/12

        NashConv:
            0.5 + 5/12
            = 11/12

        Exploitability:
            NashConv / 2
            = 11/24
    */

    double nashConv =
        evaluator.getNashConv(table);

    double exploitability =
        evaluator.getExploitability(table);


    if (approximatelyEqual(nashConv, 11.0 / 12.0)) {
        std::cout << "PASS: uniform strategy NashConv\n";
        passed++;
    }
    else {
        std::cout << "FAIL: NashConv = "
                  << nashConv
                  << ", expected "
                  << 11.0 / 12.0
                  << '\n';

        failed++;
    }


    if (approximatelyEqual(exploitability, 11.0 / 24.0)) {
        std::cout << "PASS: uniform strategy exploitability\n";
        passed++;
    }
    else {
        std::cout << "FAIL: exploitability = "
                  << exploitability
                  << ", expected "
                  << 11.0 / 24.0
                  << '\n';

        failed++;
    }


    std::cout << "\nBest-response/evaluation tests: "
              << passed << " passed, "
              << failed << " failed.\n";
}

// Test 4:
// CFR + Evaluator integration/convergence

void EvaluatorTests::testConvergence()
{
    using Card = GameState::Card;
    using History = GameState::ActionHistory;

    int passed = 0;
    int failed = 0;

    int iterationCounts[] = {
        1,
        10,
        1000,
        100000,
        1000000
    };


    std::cout << "\nCFR convergence:\n\n";


    for (int iterations : iterationCounts) {

        Solve solver;
        InfoSetTable table;

        table = solver.train(table, iterations);

        Evaluator evaluator;


        // Calculate starting EV by averaging all six deals.
        Solve::reachProb reach;

        GameState states[] = {
            GameState(Card::J, Card::Q, History::Start),
            GameState(Card::J, Card::K, History::Start),
            GameState(Card::Q, Card::J, History::Start),
            GameState(Card::Q, Card::K, History::Start),
            GameState(Card::K, Card::J, History::Start),
            GameState(Card::K, Card::Q, History::Start)
        };

        double totalEV = 0.0;

        for (GameState& state : states) {
            totalEV += evaluator.populateNodeEval(
                table,
                state,
                reach,
                true
            );
        }

        double startingEV = totalEV / 6.0;

        double nashConv =
            evaluator.getNashConv(table);

        double exploitability =
            evaluator.getExploitability(table);


        std::cout
            << "Iterations: " << iterations << '\n'
            << "Starting EV: " << startingEV << '\n'
            << "NashConv: " << nashConv << '\n'
            << "Exploitability: " << exploitability
            << "\n\n";


        /*
            Don't require each successive result to be lower.

            CFR convergence does not have to be perfectly
            monotonic at every sampled iteration count.

            Instead, apply correctness requirements to the
            final high-iteration strategy.
        */

        if (iterations == 1000000) {

            constexpr double EQUILIBRIUM_EV = -1.0 / 18.0;

            // Your previous 1M result was extremely close
            // to -1/18, so 0.001 is a generous tolerance.
            if (approximatelyEqual(
                    startingEV,
                    EQUILIBRIUM_EV,
                    0.001)) {

                std::cout
                    << "PASS: high-iteration EV approaches -1/18\n";

                passed++;
            }
            else {

                std::cout
                    << "FAIL: high-iteration EV = "
                    << startingEV
                    << ", expected approximately "
                    << EQUILIBRIUM_EV
                    << '\n';

                failed++;
            }


            // Based on your previous convergence results,
            // these tolerances leave reasonable room while
            // still detecting a substantial CFR error.

            if (nashConv < 0.004) {

                std::cout
                    << "PASS: high-iteration NashConv is small\n";

                passed++;
            }
            else {

                std::cout
                    << "FAIL: high-iteration NashConv = "
                    << nashConv
                    << '\n';

                failed++;
            }


            if (exploitability < 0.002) {

                std::cout
                    << "PASS: high-iteration exploitability is small\n";

                passed++;
            }
            else {

                std::cout
                    << "FAIL: high-iteration exploitability = "
                    << exploitability
                    << '\n';

                failed++;
            }
        }
    }

    std::cout
        << "\nConvergence tests: "
        << passed << " passed, "
        << failed << " failed.\n";
}