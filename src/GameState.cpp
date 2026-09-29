//Edited by Kian Choi 9/28/26

#include <stdexcept>
#include "GameState.hpp"

GameState::GameState(Card p1, Card p2, ActionHistory prev) {
    if (p1 == p2) { //doesn't let cards be the same
        throw std::runtime_error("Cards cannot be same value.");
    }

    p1Card = p1;
    p2Card = p2;
    prevActions = prev;
}    

GameState::GameState(Card p1, ActionHistory prev) {
    p1Card = p1;
    p2Card = static_cast<Card>((static_cast<int>(p1) + 1) % 3);
    prevActions = prev;
}

GameState::ActionHistory GameState::getActionHistory() {
    return prevActions;
}

GameState::Card GameState::getCard1() {
    return p1Card;
}

GameState::Card GameState::getCard2() {
    return p2Card;
}

bool GameState::isStateTerminal() {
    //ActionHistory values 4 and above represent terminal states
    return static_cast<int>(prevActions) >= 4; 
}

bool GameState::isP1Turn() { 
    switch(prevActions) {
        case ActionHistory::Start:
            return true;
        case ActionHistory::Check:
            return false;
        case ActionHistory::Bet:
            return false;
        case ActionHistory::CheckBet:
            return true;
        default: 
            throw std::runtime_error("Terminal state in isP1Turn method.");
    }
}

bool GameState::isAbleToBet() { 
//a player can only bet/check unless facing a bet, in which case the only legal actions are call/fold
    switch(prevActions) {
        case ActionHistory::Start:
            return true;
        case ActionHistory::Check:
            return true;
        case ActionHistory::Bet:
            return false;
        case ActionHistory::CheckBet:
            return false;
        default:
            throw std::runtime_error("Terminal state in isAbleToBet method.");
    }
}

GameState::ActionHistory GameState::resultAction(Action a) {
    if(isStateTerminal()) {
        throw std::runtime_error("Cannot take an action from a terminal state."); 
    }
    switch(prevActions) {
        case ActionHistory::Start:
            if (a == Action::Check) {
                return ActionHistory::Check;
            }
            if (a == Action::Bet) {
                return ActionHistory::Bet;
            } 
            throw std::runtime_error("Invalid action for current game state.");

        case ActionHistory::Check:
            if (a == Action::Check) {
                return ActionHistory::CheckCheck;
            }
            if (a == Action::Bet) {
                return ActionHistory::CheckBet;
            }
            throw std::runtime_error("Invalid action for current game state.");

        case ActionHistory::Bet:
            if (a == Action::Fold) {
                return ActionHistory::BetFold;
            }
            if (a == Action::Call) {
                return ActionHistory::BetCall;
            }
            throw std::runtime_error("Invalid action for current game state.");

        case ActionHistory::CheckBet:
            if (a == Action::Fold) {
                return ActionHistory::CheckBetFold;
            }
            if (a == Action::Call) {
                return ActionHistory::CheckBetCall;
            }
            throw std::runtime_error("Invalid action for current game state.");

        default:
            throw std::runtime_error("Invalid action history.");
    }
}

int GameState::p1PayoffIfTerminal() {
    if(!isStateTerminal()) {
        throw std::runtime_error("Not a terminal state.");
    }
    switch(prevActions) {
        case ActionHistory::CheckCheck:
            return (p1Card > p2Card) * 2 - 1; //evaluates to +1 if p1Card is higher, -1 if p1Card is lower
            
        case ActionHistory::BetCall:
            return (p1Card > p2Card) * 4 - 2; //evaluates to +2 if p1Card is higher, -2 if p1Card is lower
                
        case ActionHistory::BetFold: 
            return 1; //p2 folds, p1 wins +1

        case ActionHistory::CheckBetCall:
            return (p1Card > p2Card) * 4 - 2; //evaluates to +2 if p1Card is higher, -2 if p1Card is lower

        case ActionHistory::CheckBetFold:
            return -1; //p1 folds, p2 wins

        default:
            throw std::runtime_error("Invalid action history.");
    }
}

