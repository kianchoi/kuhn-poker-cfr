//Edited by Kian Choi 9/28/26

#ifndef INFOTABLE_HPP
#define INFOTABLE_HPP
#include <unordered_map>
#include <string>
#include "GameState.hpp"


class InfoSetTable {
private:
    //stores cumulative regret and strategy sums for each action
    struct InfoSetData {
        //cumulative regret values
        double checkReg;
        double betReg;
        double foldReg;
        double callReg;

        //cumulative strategy summation
        double checkSum;
        double betSum;
        double foldSum;
        double callSum;
    };

    // Maps each encoded information-set ID to its cumulative data
    std::unordered_map<int, InfoSetData> infoSet;

    //encodes an action history and private card into a unique information-set ID
    //the card uses the lower 2 bits and the action history begins at bit 2
    int lookupID(GameState::ActionHistory ah, GameState::Card c);

    //initializes an information set with zeroed regret and strategy sums if it does not already exist
    void initializeInfoSetIfMissing(int id);

    //validates that the card, history, and action form a legal nonterminal information set
    //throws an exception if the inputs are invalid
    void validateInputs(GameState::ActionHistory ah, GameState::Card c, GameState::Action a);

public:
    InfoSetTable() = default;

    //adds reach-weighted regret to an action's cumulative regret
    void addWeightedRegret(GameState::ActionHistory ah, GameState::Card c, GameState::Action a, double weightedRegret);

    //adds a reach-weighted strategy contribution to an action's cumulative strategy sum
    void addWeightedStrategy(GameState::ActionHistory ah, GameState::Card c, GameState::Action a, double weightedStrategy);

    //returns the cumulative regret for an action at an information set
    double getCumulativeRegret(GameState::ActionHistory ah, GameState::Card c, GameState::Action a);
    
    //returns the cumulative strategy sum for an action at an information set
    double getStrategySum(GameState::ActionHistory ah, GameState::Card c, GameState::Action a);

    //prints out strategy at every possible information set
    void printStrategy();

    //normalizes two action strategy sums so they sum to 1
    void normalizeSums(double& sum1, double& sum2);

    //converts an action history to a readable string
    std::string historyToString(GameState::ActionHistory ah);

    //converts a card enum to a readable string
    std::string cardToString(GameState::Card c);
};

#endif