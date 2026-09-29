//Edited by Kian Choi 9/28/26

#ifndef GAMESTATETESTS_HPP
#define GAMESTATETESTS_HPP
#include "GameState.hpp"

class GameStateTests {
public:
    //tests invalid game states and actions that should throw exceptions
    void testExceptions();

    //tests legal action-history transitions and terminal payoffs
    void testLegalTransitionsAndPayoffs();

    //tests all boolean methods in GameState (isStateTerminal, isP1Turn, isAbleToBet) 
    void testStateQueries();

};

#endif