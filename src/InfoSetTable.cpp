//Edited by Kian Choi 9/28/26

#include <unordered_map>
#include <stdexcept>
#include <cmath>
#include <iostream>
#include <string>
#include "InfoSetTable.hpp"
#include "GameState.hpp"

int InfoSetTable::lookupID(GameState::ActionHistory ah, GameState::Card c) {
    int a = static_cast<int>(ah);
    int card = static_cast<int>(c);
    if ((a < 0) || (a > 8)) {
        throw std::runtime_error("Invalid action history in lookupID.");
    }
    if ((card < 0) || (card > 2)) {
        throw std::runtime_error("Invalid card in lookupID.");
    }
    return (a << 2) + card;
}

void InfoSetTable::initializeInfoSetIfMissing(int id) {
    //initializes a new information set with zeroed values if the ID is not already present
    if (!infoSet.contains(id)) {
        InfoSetData iData{};
        bool inserted = infoSet.insert({id, iData}).second;
        if (!inserted) {
            throw std::runtime_error("Struct did not initialize; problem with lookupID?");
        }
    }
}

void InfoSetTable::validateInputs(GameState::ActionHistory ah, GameState::Card c, GameState::Action a) {
    GameState test(c, ah);
    if(test.isStateTerminal()) {
        throw std::runtime_error("Terminal state when validating inputs -> invalid");
    }
    if(test.isAbleToBet()) {
        if ((a == GameState::Action::Fold) || (a == GameState::Action::Call)) {
            throw std::runtime_error("Invalid action, can only bet or check");
        }
    }
    else {
        if ((a == GameState::Action::Check) || (a == GameState::Action::Bet)) {
            throw std::runtime_error("Invalid action, can only call or fold");
        }
    }
    
}

void InfoSetTable::addWeightedRegret(GameState::ActionHistory ah, GameState::Card c, GameState::Action a, double weightedRegret) {
    int id = lookupID(ah, c);
    bool output = false;

    if (!std::isfinite(weightedRegret)) {
        throw std::runtime_error("Run failed: weightedRegret is NaN or infinity.");
    }

    validateInputs(ah, c, a);
    initializeInfoSetIfMissing(id);

    //adds weighted strategy to the cumulative strategy sum for action a
    switch(a) {
        case GameState::Action::Check:
            infoSet[id].checkReg += weightedRegret;
            output = true;
            break;
        case GameState::Action::Bet:
            infoSet[id].betReg += weightedRegret;
            output = true;
            break;
        case GameState::Action::Fold:
            infoSet[id].foldReg += weightedRegret;
            output = true;
            break;
        case GameState::Action::Call:
            infoSet[id].callReg += weightedRegret;
            output = true;
            break;
    }
    if (!output) {
        throw std::runtime_error("Did not find possible action in hashmap; problem with lookupID?");
    }
}
    
void InfoSetTable::addWeightedStrategy(GameState::ActionHistory ah, GameState::Card c, GameState::Action a, double weightedStrategy) {
    int id = lookupID(ah, c);
    bool output = false;

    if (!std::isfinite(weightedStrategy)) {
        throw std::runtime_error("Run failed: weightedStrategy is NaN or infinity.");
    }

    if (weightedStrategy < 0) {
        throw std::runtime_error("Not a valid weightedStrategy (must be nonnegative)");
    }

    validateInputs(ah, c, a);
    initializeInfoSetIfMissing(id);

    //adds weightedStrategy to strategy sum at action a
    switch(a) {
        case GameState::Action::Check:
            infoSet[id].checkSum += weightedStrategy;
            output = true;
            break;
        case GameState::Action::Bet:
            infoSet[id].betSum += weightedStrategy;
            output = true;
            break;
        case GameState::Action::Fold:
            infoSet[id].foldSum += weightedStrategy;
            output = true;
            break;
        case GameState::Action::Call:
            infoSet[id].callSum += weightedStrategy;
            output = true;
            break;
    }
    if (!output) {
        throw std::runtime_error("Did not find possible action in hashmap; problem with lookupID?");
    }
}

double InfoSetTable::getCumulativeRegret(GameState::ActionHistory ah, GameState::Card c, GameState::Action a) {
    int id = lookupID(ah, c);
    
    validateInputs(ah, c, a);
    initializeInfoSetIfMissing(id);

    switch(a) {
        case GameState::Action::Check:
            return infoSet[id].checkReg;
        case GameState::Action::Bet:
            return infoSet[id].betReg;
        case GameState::Action::Fold:
            return infoSet[id].foldReg;
        case GameState::Action::Call:
            return infoSet[id].callReg;
    }

    throw std::runtime_error("Did not find possible action in hashmap; problem with lookupID?");
}
    
double InfoSetTable::getStrategySum(GameState::ActionHistory ah, GameState::Card c, GameState::Action a) {
    int id = lookupID(ah, c);
    
    validateInputs(ah, c, a);
    initializeInfoSetIfMissing(id);

    switch(a) {
        case GameState::Action::Check:
            return infoSet[id].checkSum;
        case GameState::Action::Bet:
            return infoSet[id].betSum;
        case GameState::Action::Fold:
            return infoSet[id].foldSum;
        case GameState::Action::Call:
            return infoSet[id].callSum;
    }

    throw std::runtime_error("Did not find possible action in hashmap; problem with lookupID?");
}

void InfoSetTable::printStrategy() {
    //only 4 action histories where strategy is possible
    //nonterminal action histories correspond to enum values 0-3.
    int cards = 3;
    int actionHistories = 4;

    for (int ah = 0; ah < actionHistories; ah++) {
        for (int c = 0; c < cards; c++) {     
            GameState::ActionHistory history = static_cast<GameState::ActionHistory>(ah);
            GameState::Card card = static_cast<GameState::Card>(c);

            GameState::Action x = GameState::Action::Check;
            GameState::Action b = GameState::Action::Bet;
            GameState::Action f = GameState::Action::Fold;
            GameState::Action ca = GameState::Action::Call;

            GameState g1(card, history);

            if (g1.isAbleToBet()) {
                //can only check or bet
                double checkSum = InfoSetTable::getStrategySum(history, card, x);
                double betSum = InfoSetTable::getStrategySum(history, card, b);
                InfoSetTable::normalizeSums(checkSum, betSum);
                std::cout << "Card: " << InfoSetTable::cardToString(card) << ", Action History: " << InfoSetTable::historyToString(history) << ", Strategy: Bet = " << betSum << ", Check = " << checkSum << '\n';
            }
            else {
                //can only fold or call
                double foldSum = InfoSetTable::getStrategySum(history, card, f);
                double callSum = InfoSetTable::getStrategySum(history, card, ca);
                InfoSetTable::normalizeSums(foldSum, callSum);
                std::cout << "Card: " << InfoSetTable::cardToString(card) << ", Action History: " << InfoSetTable::historyToString(history) << ", Strategy: Fold = " << foldSum << ", Call = " << callSum << '\n';
            }
        }
    }
}

void InfoSetTable::normalizeSums(double& sum1, double& sum2) {
    double totSum = sum1 + sum2;
    sum1 = sum1 / totSum;
    sum2 = sum2 / totSum;
}

std::string InfoSetTable::historyToString(GameState::ActionHistory ah) {
    int history = static_cast<int>(ah);

    if (history == 0) {
        return "Start";
    }
    if (history == 1) {
        return "Check";
    }
    if (history == 2) {
        return "Bet";
    }
    if (history == 3) {
        return "Check->Bet";
    }
    throw std::runtime_error("Invalid action history in historyToString.");
}

std::string InfoSetTable::cardToString(GameState::Card c) {
    int card = static_cast<int>(c);

    if (card == 0) {
        return "J";
    }
    if (card == 1) {
        return "Q";
    }
    if (card == 2) {
        return "K";
    }
    throw std::runtime_error("Invalid card in cardToString.");
}