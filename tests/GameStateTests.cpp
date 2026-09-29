//Edited by Kian Choi 9/28/26

#include <iostream>
#include <exception>
#include "GameState.hpp"
#include "GameStateTests.hpp"

void GameStateTests::testExceptions() {
    //Test 1: Same card dealt to both players
    try {
        GameState state(
            GameState::Card::Q,
            GameState::Card::Q,
            GameState::ActionHistory::Start
        );

        std::cout << "FAIL: No exception thrown.\n";
    }
    catch (const std::exception& e) {
        std::cout << "PASS: " << e.what() << '\n';
    }


    //Test 2: Call at start of game
    try {
        GameState state(
            GameState::Card::J,
            GameState::Card::Q,
            GameState::ActionHistory::Start
        );

        state.resultAction(GameState::Action::Call);

        std::cout << "FAIL: No exception thrown.\n";
    }
    catch (const std::exception& e) {
        std::cout << "PASS: " << e.what() << '\n';
    }


    //Test 3: Fold at start of game
    try {
        GameState state(
            GameState::Card::J,
            GameState::Card::K,
            GameState::ActionHistory::Start
        );

        state.resultAction(GameState::Action::Fold);

        std::cout << "FAIL: No exception thrown.\n";
    }
    catch (const std::exception& e) {
        std::cout << "PASS: " << e.what() << '\n';
    }


    //Test 4: Call after P1 checks
    try {
        GameState state(
            GameState::Card::Q,
            GameState::Card::K,
            GameState::ActionHistory::Check
        );

        state.resultAction(GameState::Action::Call);

        std::cout << "FAIL: No exception thrown.\n";
    }
    catch (const std::exception& e) {
        std::cout << "PASS: " << e.what() << '\n';
    }


    //Test 5: Fold after P1 checks
    try {
        GameState state(
            GameState::Card::Q,
            GameState::Card::J,
            GameState::ActionHistory::Check
        );

        state.resultAction(GameState::Action::Fold);

        std::cout << "FAIL: No exception thrown.\n";
    }
    catch (const std::exception& e) {
        std::cout << "PASS: " << e.what() << '\n';
    }


    //Test 6: Bet while already facing a bet
    try {
        GameState state(
            GameState::Card::K,
            GameState::Card::Q,
            GameState::ActionHistory::Bet
        );

        state.resultAction(GameState::Action::Bet);

        std::cout << "FAIL: No exception thrown.\n";
    }
    catch (const std::exception& e) {
        std::cout << "PASS: " << e.what() << '\n';
    }


    //Test 7: Check while already facing a bet
    try {
        GameState state(
            GameState::Card::J,
            GameState::Card::K,
            GameState::ActionHistory::CheckBet
        );

        state.resultAction(GameState::Action::Check);

        std::cout << "FAIL: No exception thrown.\n";
    }
    catch (const std::exception& e) {
        std::cout << "PASS: " << e.what() << '\n';
    }


    //Test 8: Attempt to take an action in terminal state
    try {
        GameState state(
            GameState::Card::K,
            GameState::Card::J,
            GameState::ActionHistory::CheckCheck
        );

        state.resultAction(GameState::Action::Bet);

        std::cout << "FAIL: No exception thrown.\n";
    }
    catch (const std::exception& e) {
        std::cout << "PASS: " << e.what() << '\n';
    }


    //Test 9: Ask for payoff in nonterminal state
    try {
        GameState state(
            GameState::Card::Q,
            GameState::Card::K,
            GameState::ActionHistory::Start
        );

        state.p1PayoffIfTerminal();

        std::cout << "FAIL: No exception thrown.\n";
    }
    catch (const std::exception& e) {
        std::cout << "PASS: " << e.what() << '\n';
    }


    //Test 10: Ask whose turn it is in terminal state
    try {
        GameState state(
            GameState::Card::K,
            GameState::Card::Q,
            GameState::ActionHistory::BetCall
        );

        state.isP1Turn();

        std::cout << "FAIL: No exception thrown.\n";
    }
    catch (const std::exception& e) {
        std::cout << "PASS: " << e.what() << '\n';
    }

}

void GameStateTests::testLegalTransitionsAndPayoffs() {
    using Card = GameState::Card;
    using Action = GameState::Action;
    using History = GameState::ActionHistory;

    int passed = 0;
    int failed = 0;

    //Checks whether an action produces the expected history.
    auto checkTransition = [&](History start, Action action,
                               History expected, const char* name) {
        try {
            GameState state(Card::J, Card::Q, start);

            if (state.resultAction(action) == expected) {
                std::cout << "PASS: " << name << '\n';
                passed++;
            } else {
                std::cout << "FAIL: " << name
                          << " (wrong history)\n";
                failed++;
            }
        }
        catch (const std::exception& e) {
            std::cout << "FAIL: " << name
                      << " (exception: " << e.what() << ")\n";
            failed++;
        }
    };

    //Checks whether a terminal state produces the expected P1 payoff.
    auto checkPayoff = [&](Card p1, Card p2, History history,
                          int expected, const char* name) {
        try {
            GameState state(p1, p2, history);

            if (state.p1PayoffIfTerminal() == expected) {
                std::cout << "PASS: " << name << '\n';
                passed++;
            } else {
                std::cout << "FAIL: " << name
                          << " (wrong payoff)\n";
                failed++;
            }
        }
        catch (const std::exception& e) {
            std::cout << "FAIL: " << name
                      << " (exception: " << e.what() << ")\n";
            failed++;
        }
    };

    // All 8 legal transitions:

    checkTransition(History::Start, Action::Check,
                    History::Check, "Start -> Check");

    checkTransition(History::Start, Action::Bet,
                    History::Bet, "Start -> Bet");

    checkTransition(History::Check, Action::Check,
                    History::CheckCheck, "Check -> CheckCheck");

    checkTransition(History::Check, Action::Bet,
                    History::CheckBet, "Check -> CheckBet");

    checkTransition(History::Bet, Action::Call,
                    History::BetCall, "Bet -> BetCall");

    checkTransition(History::Bet, Action::Fold,
                    History::BetFold, "Bet -> BetFold");

    checkTransition(History::CheckBet, Action::Call,
                    History::CheckBetCall, "CheckBet -> CheckBetCall");

    checkTransition(History::CheckBet, Action::Fold,
                    History::CheckBetFold, "CheckBet -> CheckBetFold");

    // Terminal payoff tests:
    // P1 payoff: +1/-1 for an uncalled pot,
    //            +2/-2 for a called bet.

    checkPayoff(Card::K, Card::Q, History::CheckCheck,
                +1, "CheckCheck: P1 wins");

    checkPayoff(Card::J, Card::K, History::CheckCheck,
                -1, "CheckCheck: P1 loses");

    checkPayoff(Card::K, Card::Q, History::BetCall,
                +2, "BetCall: P1 wins");

    checkPayoff(Card::J, Card::Q, History::BetCall,
                -2, "BetCall: P1 loses");

    checkPayoff(Card::J, Card::K, History::BetFold,
                +1, "BetFold: P2 folds");

    checkPayoff(Card::K, Card::J, History::CheckBetCall,
                +2, "CheckBetCall: P1 wins");

    checkPayoff(Card::J, Card::K, History::CheckBetCall,
                -2, "CheckBetCall: P1 loses");

    checkPayoff(Card::K, Card::J, History::CheckBetFold,
                -1, "CheckBetFold: P1 folds");

    std::cout << "\nLegal transition/payoff tests: "
              << passed << " passed, "
              << failed << " failed.\n";
}

void GameStateTests::testStateQueries() {
    using Card = GameState::Card;
    using History = GameState::ActionHistory;

    int passed = 0;
    int failed = 0;

    auto checkTerminal = [&](History history, bool expected, const char* name) {
        try {
            GameState state(Card::J, Card::Q, history);

            if (state.isStateTerminal() == expected) {
                std::cout << "PASS: " << name << '\n';
                passed++;
            } else {
                std::cout << "FAIL: " << name << " (wrong terminal value)\n";
                failed++;
            }
        }
        catch (const std::exception& e) {
            std::cout << "FAIL: " << name
                      << " (exception: " << e.what() << ")\n";
            failed++;
        }
    };

    auto checkP1Turn = [&](History history, bool expected, const char* name) {
        try {
            GameState state(Card::J, Card::Q, history);

            if (state.isP1Turn() == expected) {
                std::cout << "PASS: " << name << '\n';
                passed++;
            } else {
                std::cout << "FAIL: " << name << " (wrong turn value)\n";
                failed++;
            }
        }
        catch (const std::exception& e) {
            std::cout << "FAIL: " << name
                      << " (exception: " << e.what() << ")\n";
            failed++;
        }
    };

    auto checkAbleToBet = [&](History history, bool expected, const char* name) {
        try {
            GameState state(Card::J, Card::Q, history);

            if (state.isAbleToBet() == expected) {
                std::cout << "PASS: " << name << '\n';
                passed++;
            } else {
                std::cout << "FAIL: " << name << " (wrong betting value)\n";
                failed++;
            }
        }
        catch (const std::exception& e) {
            std::cout << "FAIL: " << name
                      << " (exception: " << e.what() << ")\n";
            failed++;
        }
    };

    // isStateTerminal()
    checkTerminal(History::Start, false, "Start is nonterminal");
    checkTerminal(History::Check, false, "Check is nonterminal");
    checkTerminal(History::Bet, false, "Bet is nonterminal");
    checkTerminal(History::CheckBet, false, "CheckBet is nonterminal");

    checkTerminal(History::CheckCheck, true, "CheckCheck is terminal");
    checkTerminal(History::BetCall, true, "BetCall is terminal");
    checkTerminal(History::BetFold, true, "BetFold is terminal");
    checkTerminal(History::CheckBetCall, true, "CheckBetCall is terminal");
    checkTerminal(History::CheckBetFold, true, "CheckBetFold is terminal");

    // isP1Turn()
    checkP1Turn(History::Start, true, "Start: P1 turn");
    checkP1Turn(History::Check, false, "Check: P2 turn");
    checkP1Turn(History::Bet, false, "Bet: P2 turn");
    checkP1Turn(History::CheckBet, true, "CheckBet: P1 turn");

    // isAbleToBet()
    checkAbleToBet(History::Start, true, "Start: can bet");
    checkAbleToBet(History::Check, true, "Check: can bet");
    checkAbleToBet(History::Bet, false, "Bet: cannot bet");
    checkAbleToBet(History::CheckBet, false, "CheckBet: cannot bet");

    std::cout << "\nState query tests: "
              << passed << " passed, "
              << failed << " failed.\n";

    printf("");
}