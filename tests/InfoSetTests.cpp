//Edited by Kian Choi 9/28/26

#include <iostream>
#include <exception>
#include <cstdio>
#include <cmath>
#include "InfoSetTests.hpp"

void InfoSetTests::testLegalQueries () {
    //basic add to regrets and retrieve regret
    GameState state(
        GameState::Card::Q,
        GameState::Card::J,
        GameState::ActionHistory::Start
    );

    InfoSetTable iTable;

    iTable.addWeightedRegret(state.getActionHistory(), state.getCard1(), GameState::Action::Bet, 1.0);
    double getRegret = iTable.getCumulativeRegret(state.getActionHistory(), state.getCard1(), GameState::Action::Bet);

    if (getRegret == 1.0) {
        std::cout << "PASS: Cumulative regret added successfully" << '\n';
    }
    else {
        std::cout << "FAIL: Cumulative regret did not add correctly" << '\n';
    }

    //basic add to strategy and retrieve strategy
    GameState state2(
        GameState::Card::K,
        GameState::Card::Q,
        GameState::ActionHistory::Check
    );

    iTable.addWeightedStrategy(state2.getActionHistory(), state2.getCard1(), GameState::Action::Bet, 1.0);
    double getBetStrategy = iTable.getStrategySum(state2.getActionHistory(), state2.getCard1(), GameState::Action::Bet);

    if (getBetStrategy == 1.0) {
        std::cout << "PASS: Strategy added successfully" << '\n';
    }
    else {
        std::cout << "FAIL: Strategy was not 1.0 for bet" << '\n';
    }
    
    //get cumulative regret with nothing in table
    GameState state3(
        GameState::Card::Q,
        GameState::Card::J,
        GameState::ActionHistory::Start
    );

    double getRegret2 = iTable.getStrategySum(state3.getActionHistory(), state3.getCard2(), GameState::Action::Bet);

    if (getRegret2 == 0.0) {
        std::cout << "PASS: Regrets initialized at zero" << '\n';
    }
    else {
        std::cout << "FAIL: Regrets not initialized at zero" << '\n';
    }

    //get strategy sum with nothing in table
    double getBetStrategy2 = iTable.getStrategySum(state3.getActionHistory(), state3.getCard2(), GameState::Action::Bet);

    if (getBetStrategy2 == 0.0) {
        std::cout << "PASS: Strategy initialized at zero" << '\n';
    }
    else {
        std::cout << "FAIL: Strategy not initialized at zero" << '\n';
    }

}

using Card = GameState::Card;
using Action = GameState::Action;
using ActionHistory = GameState::ActionHistory;


//for comparing doubles safely
bool InfoSetTests::approximatelyEqual(double a, double b, double epsilon = 1e-9) {
    return std::fabs(a - b) < epsilon;
}


//helper for test output
void InfoSetTests::check(bool condition, const char* testName, int& passed, int& failed) {
    if (condition) {
        std::cout << "PASS: " << testName << '\n';
        passed++;
    } else {
        std::cout << "FAIL: " << testName << '\n';
        failed++;
    }
}

void InfoSetTests::testInfoSetTable() {

    int passed = 0;
    int failed = 0;

    //Test 1: 
    //New information sets should begin at zero

    {
        InfoSetTable table;

        check(
            approximatelyEqual(
                table.getCumulativeRegret(
                    ActionHistory::Start,
                    Card::J,
                    Action::Check),
                0.0),
            "New info set regret initializes to 0",
            passed,
            failed
        );

        check(
            approximatelyEqual(
                table.getStrategySum(
                    ActionHistory::Start,
                    Card::J,
                    Action::Check),
                0.0),
            "New info set strategy sum initializes to 0",
            passed,
            failed
        );
    }

    //Test 2: Single regret update

    {
        InfoSetTable table;

        table.addWeightedRegret(
            ActionHistory::Start,
            Card::J,
            Action::Check,
            2.5
        );

        check(
            approximatelyEqual(
                table.getCumulativeRegret(
                    ActionHistory::Start,
                    Card::J,
                    Action::Check),
                2.5),
            "Single regret update stored correctly",
            passed,
            failed
        );
    }

    //Test 3:
    //Regret values should accumulate

    {
        InfoSetTable table;

        table.addWeightedRegret(
            ActionHistory::Start,
            Card::Q,
            Action::Bet,
            1.5
        );

        table.addWeightedRegret(
            ActionHistory::Start,
            Card::Q,
            Action::Bet,
            2.0
        );

        table.addWeightedRegret(
            ActionHistory::Start,
            Card::Q,
            Action::Bet,
            0.75
        );

        check(
            approximatelyEqual(
                table.getCumulativeRegret(
                    ActionHistory::Start,
                    Card::Q,
                    Action::Bet),
                4.25),
            "Repeated regret updates accumulate",
            passed,
            failed
        );
    }


    //Test 4: 
    //Negative regrets should accumulate

    {
        InfoSetTable table;

        table.addWeightedRegret(
            ActionHistory::Start,
            Card::K,
            Action::Check,
            -3.0
        );

        check(
            approximatelyEqual(
                table.getCumulativeRegret(
                    ActionHistory::Start,
                    Card::K,
                    Action::Check),
                -3.0),
            "Negative regret stored correctly",
            passed,
            failed
        );

        table.addWeightedRegret(
            ActionHistory::Start,
            Card::K,
            Action::Check,
            1.25
        );

        check(
            approximatelyEqual(
                table.getCumulativeRegret(
                    ActionHistory::Start,
                    Card::K,
                    Action::Check),
                -1.75),
            "Positive and negative regrets accumulate correctly",
            passed,
            failed
        );
    }


    //Test 5: Zero regret update

    {
        InfoSetTable table;

        table.addWeightedRegret(
            ActionHistory::Bet,
            Card::Q,
            Action::Fold,
            0.0
        );

        check(
            approximatelyEqual(
                table.getCumulativeRegret(
                    ActionHistory::Bet,
                    Card::Q,
                    Action::Fold),
                0.0),
            "Zero regret update leaves value at 0",
            passed,
            failed
        );
    }

    //Test 6:
    //Actions within same information set are independent

    {
        InfoSetTable table;

        table.addWeightedRegret(
            ActionHistory::Start,
            Card::J,
            Action::Check,
            1.25
        );

        table.addWeightedRegret(
            ActionHistory::Start,
            Card::J,
            Action::Bet,
            -2.75
        );

        check(
            approximatelyEqual(
                table.getCumulativeRegret(
                    ActionHistory::Start,
                    Card::J,
                    Action::Check),
                1.25),
            "Check regret independent from Bet regret",
            passed,
            failed
        );

        check(
            approximatelyEqual(
                table.getCumulativeRegret(
                    ActionHistory::Start,
                    Card::J,
                    Action::Bet),
                -2.75),
            "Bet regret independent from Check regret",
            passed,
            failed
        );
    }

    //Test 7:
    //Different cards create different information sets

    {
        InfoSetTable table;

        table.addWeightedRegret(
            ActionHistory::Start,
            Card::J,
            Action::Bet,
            1.0
        );

        table.addWeightedRegret(
            ActionHistory::Start,
            Card::Q,
            Action::Bet,
            2.0
        );

        table.addWeightedRegret(
            ActionHistory::Start,
            Card::K,
            Action::Bet,
            3.0
        );

        check(
            approximatelyEqual(
                table.getCumulativeRegret(
                    ActionHistory::Start,
                    Card::J,
                    Action::Bet),
                1.0),
            "J information set stored independently",
            passed,
            failed
        );

        check(
            approximatelyEqual(
                table.getCumulativeRegret(
                    ActionHistory::Start,
                    Card::Q,
                    Action::Bet),
                2.0),
            "Q information set stored independently",
            passed,
            failed
        );

        check(
            approximatelyEqual(
                table.getCumulativeRegret(
                    ActionHistory::Start,
                    Card::K,
                    Action::Bet),
                3.0),
            "K information set stored independently",
            passed,
            failed
        );
    }

    //Test 8:
    //Different action histories create different info sets

    {
        InfoSetTable table;

        table.addWeightedRegret(
            ActionHistory::Start,
            Card::Q,
            Action::Bet,
            1.0
        );

        table.addWeightedRegret(
            ActionHistory::Check,
            Card::Q,
            Action::Bet,
            2.0
        );

        table.addWeightedRegret(
            ActionHistory::Bet,
            Card::Q,
            Action::Call,
            3.0
        );

        table.addWeightedRegret(
            ActionHistory::CheckBet,
            Card::Q,
            Action::Call,
            4.0
        );

        check(
            approximatelyEqual(
                table.getCumulativeRegret(
                    ActionHistory::Start,
                    Card::Q,
                    Action::Bet),
                1.0),
            "Start history maps to independent info set",
            passed,
            failed
        );

        check(
            approximatelyEqual(
                table.getCumulativeRegret(
                    ActionHistory::Check,
                    Card::Q,
                    Action::Bet),
                2.0),
            "Check history maps to independent info set",
            passed,
            failed
        );

        check(
            approximatelyEqual(
                table.getCumulativeRegret(
                    ActionHistory::Bet,
                    Card::Q,
                    Action::Call),
                3.0),
            "Bet history maps to independent info set",
            passed,
            failed
        );

        check(
            approximatelyEqual(
                table.getCumulativeRegret(
                    ActionHistory::CheckBet,
                    Card::Q,
                    Action::Call),
                4.0),
            "CheckBet history maps to independent info set",
            passed,
            failed
        );
    }

    //Test 9:
    //Single strategy sum update

    {
        InfoSetTable table;

        table.addWeightedStrategy(
            ActionHistory::Start,
            Card::J,
            Action::Bet,
            0.35
        );

        check(
            approximatelyEqual(
                table.getStrategySum(
                    ActionHistory::Start,
                    Card::J,
                    Action::Bet),
                0.35),
            "Single strategy update stored correctly",
            passed,
            failed
        );
    }

    //Test 10:
    //Strategy sum updates should accumulate

    {
        InfoSetTable table;

        table.addWeightedStrategy(
            ActionHistory::Start,
            Card::K,
            Action::Bet,
            0.25
        );

        table.addWeightedStrategy(
            ActionHistory::Start,
            Card::K,
            Action::Bet,
            0.50
        );

        table.addWeightedStrategy(
            ActionHistory::Start,
            Card::K,
            Action::Bet,
            0.10
        );

        check(
            approximatelyEqual(
                table.getStrategySum(
                    ActionHistory::Start,
                    Card::K,
                    Action::Bet),
                0.85),
            "Repeated strategy updates accumulate",
            passed,
            failed
        );
    }

    //Test 11:
    //Strategy sum actions are independent

    {
        InfoSetTable table;

        table.addWeightedStrategy(
            ActionHistory::Bet,
            Card::J,
            Action::Fold,
            0.75
        );

        table.addWeightedStrategy(
            ActionHistory::Bet,
            Card::J,
            Action::Call,
            0.25
        );

        check(
            approximatelyEqual(
                table.getStrategySum(
                    ActionHistory::Bet,
                    Card::J,
                    Action::Fold),
                0.75),
            "Fold strategy sum independent from Call",
            passed,
            failed
        );

        check(
            approximatelyEqual(
                table.getStrategySum(
                    ActionHistory::Bet,
                    Card::J,
                    Action::Call),
                0.25),
            "Call strategy sum independent from Fold",
            passed,
            failed
        );
    }

    //Test 12:
    //Strategy sums are independent across cards

    {
        InfoSetTable table;

        table.addWeightedStrategy(
            ActionHistory::Check,
            Card::J,
            Action::Bet,
            0.1
        );

        table.addWeightedStrategy(
            ActionHistory::Check,
            Card::Q,
            Action::Bet,
            0.5
        );

        table.addWeightedStrategy(
            ActionHistory::Check,
            Card::K,
            Action::Bet,
            0.9
        );

        check(
            approximatelyEqual(
                table.getStrategySum(
                    ActionHistory::Check,
                    Card::J,
                    Action::Bet),
                0.1),
            "J strategy sum stored independently",
            passed,
            failed
        );

        check(
            approximatelyEqual(
                table.getStrategySum(
                    ActionHistory::Check,
                    Card::Q,
                    Action::Bet),
                0.5),
            "Q strategy sum stored independently",
            passed,
            failed
        );

        check(
            approximatelyEqual(
                table.getStrategySum(
                    ActionHistory::Check,
                    Card::K,
                    Action::Bet),
                0.9),
            "K strategy sum stored independently",
            passed,
            failed
        );
    }

    //Test 13:
    //Strategy sums are independent across histories

    {
        InfoSetTable table;

        table.addWeightedStrategy(
            ActionHistory::Bet,
            Card::K,
            Action::Call,
            0.3
        );

        table.addWeightedStrategy(
            ActionHistory::CheckBet,
            Card::K,
            Action::Call,
            0.7
        );

        check(
            approximatelyEqual(
                table.getStrategySum(
                    ActionHistory::Bet,
                    Card::K,
                    Action::Call),
                0.3),
            "Bet strategy info set independent from CheckBet",
            passed,
            failed
        );

        check(
            approximatelyEqual(
                table.getStrategySum(
                    ActionHistory::CheckBet,
                    Card::K,
                    Action::Call),
                0.7),
            "CheckBet strategy info set independent from Bet",
            passed,
            failed
        );
    }

    //Test 14:
    //Regret and strategy sums must be independent

    {
        InfoSetTable table;

        table.addWeightedRegret(
            ActionHistory::Start,
            Card::Q,
            Action::Bet,
            5.5
        );

        check(
            approximatelyEqual(
                table.getStrategySum(
                    ActionHistory::Start,
                    Card::Q,
                    Action::Bet),
                0.0),
            "Updating regret does not modify strategy sum",
            passed,
            failed
        );

        table.addWeightedStrategy(
            ActionHistory::Start,
            Card::Q,
            Action::Bet,
            0.65
        );

        check(
            approximatelyEqual(
                table.getCumulativeRegret(
                    ActionHistory::Start,
                    Card::Q,
                    Action::Bet),
                5.5),
            "Updating strategy sum does not modify regret",
            passed,
            failed
        );

        check(
            approximatelyEqual(
                table.getStrategySum(
                    ActionHistory::Start,
                    Card::Q,
                    Action::Bet),
                0.65),
            "Regret and strategy can coexist in same action",
            passed,
            failed
        );
    }

    //Test 15: 
    //All four action fields stored correctly

    {
        InfoSetTable table;

        //these histories correspond to places where each action type
        //is actually legal in Kuhn poker.

        table.addWeightedRegret(
            ActionHistory::Start,
            Card::J,
            Action::Check,
            1.1
        );

        table.addWeightedRegret(
            ActionHistory::Start,
            Card::J,
            Action::Bet,
            2.2
        );

        table.addWeightedRegret(
            ActionHistory::Bet,
            Card::J,
            Action::Fold,
            3.3
        );

        table.addWeightedRegret(
            ActionHistory::Bet,
            Card::J,
            Action::Call,
            4.4
        );

        check(
            approximatelyEqual(
                table.getCumulativeRegret(
                    ActionHistory::Start,
                    Card::J,
                    Action::Check),
                1.1),
            "checkReg field works",
            passed,
            failed
        );

        check(
            approximatelyEqual(
                table.getCumulativeRegret(
                    ActionHistory::Start,
                    Card::J,
                    Action::Bet),
                2.2),
            "betReg field works",
            passed,
            failed
        );

        check(
            approximatelyEqual(
                table.getCumulativeRegret(
                    ActionHistory::Bet,
                    Card::J,
                    Action::Fold),
                3.3),
            "foldReg field works",
            passed,
            failed
        );

        check(
            approximatelyEqual(
                table.getCumulativeRegret(
                    ActionHistory::Bet,
                    Card::J,
                    Action::Call),
                4.4),
            "callReg field works",
            passed,
            failed
        );
    }

    //Test 16:
    //Many updates should not interfere with each other

    {
        InfoSetTable table;

        table.addWeightedRegret(
            ActionHistory::Start, Card::J, Action::Check, 1.0);

        table.addWeightedRegret(
            ActionHistory::Start, Card::Q, Action::Check, 2.0);

        table.addWeightedRegret(
            ActionHistory::Start, Card::K, Action::Check, 3.0);

        table.addWeightedRegret(
            ActionHistory::Check, Card::J, Action::Bet, 4.0);

        table.addWeightedRegret(
            ActionHistory::Check, Card::Q, Action::Bet, 5.0);

        table.addWeightedRegret(
            ActionHistory::Check, Card::K, Action::Bet, 6.0);


        check(
            approximatelyEqual(
                table.getCumulativeRegret(
                    ActionHistory::Start,
                    Card::J,
                    Action::Check),
                1.0)
            &&
            approximatelyEqual(
                table.getCumulativeRegret(
                    ActionHistory::Start,
                    Card::Q,
                    Action::Check),
                2.0)
            &&
            approximatelyEqual(
                table.getCumulativeRegret(
                    ActionHistory::Start,
                    Card::K,
                    Action::Check),
                3.0)
            &&
            approximatelyEqual(
                table.getCumulativeRegret(
                    ActionHistory::Check,
                    Card::J,
                    Action::Bet),
                4.0)
            &&
            approximatelyEqual(
                table.getCumulativeRegret(
                    ActionHistory::Check,
                    Card::Q,
                    Action::Bet),
                5.0)
            &&
            approximatelyEqual(
                table.getCumulativeRegret(
                    ActionHistory::Check,
                    Card::K,
                    Action::Bet),
                6.0),
            "Multiple information sets retain correct values",
            passed,
            failed
        );
    }


    //Summary:
    std::cout << "\nInfoSetTable tests: "
              << passed << " passed, "
              << failed << " failed.\n";
}

void InfoSetTests::testInfoSetExceptions() {
    int passed = 0;
    int failed = 0;

    //Test 1: 
    //Negative weighted strategy

    {
        InfoSetTable table;

        try {
            table.addWeightedStrategy(
                ActionHistory::Start,
                Card::J,
                Action::Check,
                -0.5
            );

            std::cout << "FAIL: Negative weighted strategy did not throw\n";
            failed++;
        }
        catch (const std::exception&) {
            std::cout << "PASS: Negative weighted strategy throws exception\n";
            passed++;
        }
    }


    // Test 2:
    // Invalid action history enum values
    // Valid range: 0-8
    // Test immediately below and above valid range

    {
        InfoSetTable table;

        try {
            table.getCumulativeRegret(
                static_cast<ActionHistory>(-1),
                Card::J,
                Action::Check
            );

            std::cout << "FAIL: ActionHistory -1 did not throw\n";
            failed++;
        }
        catch (const std::exception&) {
            std::cout << "PASS: ActionHistory -1 throws exception\n";
            passed++;
        }


        try {
            table.getCumulativeRegret(
                static_cast<ActionHistory>(9),
                Card::J,
                Action::Check
            );

            std::cout << "FAIL: ActionHistory 9 did not throw\n";
            failed++;
        }
        catch (const std::exception&) {
            std::cout << "PASS: ActionHistory 9 throws exception\n";
            passed++;
        }
    }

    //Test 3: 
    //Invalid action enum values
    //Valid range: 0-3

    {
        InfoSetTable table;

        try {
            table.getCumulativeRegret(
                ActionHistory::Start,
                Card::J,
                static_cast<Action>(-1)
            );

            std::cout << "FAIL: Action -1 did not throw\n";
            failed++;
        }
        catch (const std::exception&) {
            std::cout << "PASS: Action -1 throws exception\n";
            passed++;
        }


        try {
            table.getCumulativeRegret(
                ActionHistory::Start,
                Card::J,
                static_cast<Action>(4)
            );

            std::cout << "FAIL: Action 4 did not throw\n";
            failed++;
        }
        catch (const std::exception&) {
            std::cout << "PASS: Action 4 throws exception\n";
            passed++;
        }
    }

    //Test 4:
    //Invalid card enum values
    //Valid range: 0-2

    {
        InfoSetTable table;

        try {
            table.getCumulativeRegret(
                ActionHistory::Start,
                static_cast<Card>(-1),
                Action::Check
            );

            std::cout << "FAIL: Card -1 did not throw\n";
            failed++;
        }
        catch (const std::exception&) {
            std::cout << "PASS: Card -1 throws exception\n";
            passed++;
        }


        try {
            table.getCumulativeRegret(
                ActionHistory::Start,
                static_cast<Card>(3),
                Action::Check
            );

            std::cout << "FAIL: Card 3 did not throw\n";
            failed++;
        }
        catch (const std::exception&) {
            std::cout << "PASS: Card 3 throws exception\n";
            passed++;
        }
    }


    std::cout << "\nInfoSetTable exception tests: "
              << passed << " passed, "
              << failed << " failed.\n";

}

void InfoSetTests::testAdditionalInfoSetExceptions() {
    int passed = 0;
    int failed = 0;

    //helper:

    auto checkThrows = [&](auto testFunction, const char* testName) {
        try {
            testFunction();

            std::cout << "FAIL: " << testName << '\n';
            failed++;
        }
        catch (const std::exception&) {
            std::cout << "PASS: " << testName << '\n';
            passed++;
        }
    };

    //Test 1:
    //Invalid actions at start
    //At Start, P1 can only Check or Bet.
    //Fold and Call should therefore be rejected.

    {
        InfoSetTable table;

        checkThrows(
            [&]() {
                table.addWeightedRegret(
                    ActionHistory::Start,
                    Card::J,
                    Action::Fold,
                    1.0
                );
            },
            "Start -> Fold rejected"
        );

        checkThrows(
            [&]() {
                table.addWeightedRegret(
                    ActionHistory::Start,
                    Card::J,
                    Action::Call,
                    1.0
                );
            },
            "Start -> Call rejected"
        );
    }

    //Test 2:
    //Invalid actions after check
    //After P1 checks, P2 can Check or Bet.
    //Fold and Call should be rejected.

    {
        InfoSetTable table;

        checkThrows(
            [&]() {
                table.addWeightedRegret(
                    ActionHistory::Check,
                    Card::Q,
                    Action::Fold,
                    1.0
                );
            },
            "Check -> Fold rejected"
        );

        checkThrows(
            [&]() {
                table.addWeightedRegret(
                    ActionHistory::Check,
                    Card::Q,
                    Action::Call,
                    1.0
                );
            },
            "Check -> Call rejected"
        );
    }


    //Test 3:
    //Invalid actions after bet
    //After P1 bets, P2 can Fold or Call.
    //Check and Bet should be rejected.

    {
        InfoSetTable table;

        checkThrows(
            [&]() {
                table.addWeightedRegret(
                    ActionHistory::Bet,
                    Card::K,
                    Action::Check,
                    1.0
                );
            },
            "Bet -> Check rejected"
        );

        checkThrows(
            [&]() {
                table.addWeightedRegret(
                    ActionHistory::Bet,
                    Card::K,
                    Action::Bet,
                    1.0
                );
            },
            "Bet -> Bet rejected"
        );
    }

    //Test 4:
    //Invalid actions after check-bet
    //After P1 checks and P2 bets, P1 can Fold or Call.
    //Check and Bet should be rejected.

    {
        InfoSetTable table;

        checkThrows(
            [&]() {
                table.addWeightedRegret(
                    ActionHistory::CheckBet,
                    Card::J,
                    Action::Check,
                    1.0
                );
            },
            "CheckBet -> Check rejected"
        );

        checkThrows(
            [&]() {
                table.addWeightedRegret(
                    ActionHistory::CheckBet,
                    Card::J,
                    Action::Bet,
                    1.0
                );
            },
            "CheckBet -> Bet rejected"
        );
    }


    //Test 5:
    //Strategy updates should also reject illegal actions
    //Verifies validation isn't implemented only in
    //addWeightedRegret().

    {
        InfoSetTable table;

        checkThrows(
            [&]() {
                table.addWeightedStrategy(
                    ActionHistory::Start,
                    Card::J,
                    Action::Call,
                    0.5
                );
            },
            "Strategy update rejects illegal action"
        );
    }


    //Test 6:
    //Getters should also reject illegal actions
    //Prevents callers from querying fields that don't belong
    //to the information set.

    {
        InfoSetTable table;

        checkThrows(
            [&]() {
                table.getCumulativeRegret(
                    ActionHistory::Bet,
                    Card::Q,
                    Action::Check
                );
            },
            "getCumulativeRegret rejects illegal action"
        );

        checkThrows(
            [&]() {
                table.getStrategySum(
                    ActionHistory::CheckBet,
                    Card::Q,
                    Action::Bet
                );
            },
            "getStrategySum rejects illegal action"
        );
    }

    //Test 7:
    //Terminal history: check-check

    {
        InfoSetTable table;

        checkThrows(
            [&]() {
                table.addWeightedRegret(
                    ActionHistory::CheckCheck,
                    Card::J,
                    Action::Check,
                    1.0
                );
            },
            "CheckCheck terminal history rejected"
        );
    }


    //Test 8:
    //Terminal history: bet-call

    {
        InfoSetTable table;

        checkThrows(
            [&]() {
                table.addWeightedRegret(
                    ActionHistory::BetCall,
                    Card::Q,
                    Action::Call,
                    1.0
                );
            },
            "BetCall terminal history rejected"
        );
    }

    //Test 9:
    //Terminal history: bet-fold

    {
        InfoSetTable table;

        checkThrows(
            [&]() {
                table.addWeightedRegret(
                    ActionHistory::BetFold,
                    Card::K,
                    Action::Fold,
                    1.0
                );
            },
            "BetFold terminal history rejected"
        );
    }

    //Test 10:
    //Terminal history: check-bet-call

    {
        InfoSetTable table;

        checkThrows(
            [&]() {
                table.addWeightedRegret(
                    ActionHistory::CheckBetCall,
                    Card::J,
                    Action::Call,
                    1.0
                );
            },
            "CheckBetCall terminal history rejected"
        );
    }

    //Test 11:
    //Terminal history: check-bet-fold

    {
        InfoSetTable table;

        checkThrows(
            [&]() {
                table.addWeightedRegret(
                    ActionHistory::CheckBetFold,
                    Card::Q,
                    Action::Fold,
                    1.0
                );
            },
            "CheckBetFold terminal history rejected"
        );
    }

    //Test 12:
    //Getters should reject terminal histories too

    {
        InfoSetTable table;

        checkThrows(
            [&]() {
                table.getCumulativeRegret(
                    ActionHistory::CheckCheck,
                    Card::K,
                    Action::Check
                );
            },
            "getCumulativeRegret rejects terminal history"
        );

        checkThrows(
            [&]() {
                table.getStrategySum(
                    ActionHistory::BetCall,
                    Card::K,
                    Action::Call
                );
            },
            "getStrategySum rejects terminal history"
        );
    }

    //Test 13:
    //NaN weighted regret

    {
        InfoSetTable table;

        double nan =
            std::numeric_limits<double>::quiet_NaN();

        checkThrows(
            [&]() {
                table.addWeightedRegret(
                    ActionHistory::Start,
                    Card::J,
                    Action::Check,
                    nan
                );
            },
            "NaN weighted regret rejected"
        );
    }

    //Test 14:
    //Positive infinity weighted regret

    {
        InfoSetTable table;

        double infinity =
            std::numeric_limits<double>::infinity();

        checkThrows(
            [&]() {
                table.addWeightedRegret(
                    ActionHistory::Start,
                    Card::J,
                    Action::Check,
                    infinity
                );
            },
            "Positive infinity weighted regret rejected"
        );
    }

    //Test 15: 
    //Negative infinity weighted regret

    {
        InfoSetTable table;

        double negativeInfinity =
            -std::numeric_limits<double>::infinity();

        checkThrows(
            [&]() {
                table.addWeightedRegret(
                    ActionHistory::Start,
                    Card::J,
                    Action::Check,
                    negativeInfinity
                );
            },
            "Negative infinity weighted regret rejected"
        );
    }

    //Test 16:
    //NaN weighted strategy

    {
        InfoSetTable table;

        double nan =
            std::numeric_limits<double>::quiet_NaN();

        checkThrows(
            [&]() {
                table.addWeightedStrategy(
                    ActionHistory::Start,
                    Card::J,
                    Action::Check,
                    nan
                );
            },
            "NaN weighted strategy rejected"
        );
    }

    //Test 17:
    //Positive infinity weighted strategy

    {
        InfoSetTable table;

        double infinity =
            std::numeric_limits<double>::infinity();

        checkThrows(
            [&]() {
                table.addWeightedStrategy(
                    ActionHistory::Start,
                    Card::J,
                    Action::Check,
                    infinity
                );
            },
            "Positive infinity weighted strategy rejected"
        );
    }

    //Test 18:
    //Negative infinity weighted strategy

    {
        InfoSetTable table;

        double negativeInfinity =
            -std::numeric_limits<double>::infinity();

        checkThrows(
            [&]() {
                table.addWeightedStrategy(
                    ActionHistory::Start,
                    Card::J,
                    Action::Check,
                    negativeInfinity
                );
            },
            "Negative infinity weighted strategy rejected"
        );
    }

    //Summary:
    
    std::cout << "\nAdditional InfoSetTable exception tests: "
              << passed << " passed, "
              << failed << " failed.\n";
}