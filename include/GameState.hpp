//Edited by Kian Choi 9/28/26

#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

class GameState {
public:
    //possible cards
    enum class Card {J, Q, K};

    //possible player actions
    enum class Action {Check, Bet, Fold, Call};

    //possible action histories
    enum class ActionHistory {Start, Check, Bet, CheckBet, CheckCheck, BetCall, BetFold, CheckBetCall, CheckBetFold};

private:
    Card p1Card;
    Card p2Card;
    ActionHistory prevActions;

public:
    //constructs a game state when both players' private cards are known
    GameState(Card p1, Card p2, ActionHistory prev);

    //constructs a game state when only one player's private card is known
    GameState(Card p1, ActionHistory prev);

    //getters
    ActionHistory getActionHistory();
    Card getCard1();
    Card getCard2();

    bool isStateTerminal();
    bool isP1Turn();
    bool isAbleToBet();

    //returns the action history that results from taking the given action
    ActionHistory resultAction(Action a);

    //returns Player 1's payoff at a terminal state; throws if the state is nonterminal
    int p1PayoffIfTerminal();

};

#endif