//Edited by Kian Choi 9/28/26

#ifndef SOLVE_HPP
#define SOLVE_HPP
#include "GameState.hpp"
#include "InfoSetTable.hpp"

class Solve {
public:
    //stores the reach probabilities of a certain node
    struct reachProb {double chance = 1.0/6.0; double p1 = 1.0; double p2 = 1.0;}; //initialize chance reach to 1/6

    //stores action probabilities for a strategy at an information set.
    struct strategy {double checkFreq = 0.0; double betFreq = 0.0; double foldFreq = 0.0; double callFreq = 0.0;};

    //stores the strategy EV and EV of each available action.
    struct EV {double stratVal = 0.0; double checkEV = 0.0; double betEV = 0.0; double foldEV = 0.0; double callEV = 0.0;};

    //recursively performs CFR and returns P1's expected value at the current node.
    //iGlobal stores cumulative data across iterations, while iTemp collects regret and strategy updates for the current iteration.
    double getInfoSetEV(InfoSetTable& iGlobal, InfoSetTable& iTemp, GameState g, reachProb r, bool isP1Turn);

    //performs one iteration, starting on the chance node and iterating through all 6 starting card combinations
    void performOneIteration(InfoSetTable& iGlobal);

    //trains the algorithm for "iterations" iterations
    InfoSetTable train(InfoSetTable& i, int iterations);

    //performs regret matching and returns a "strategy" struct with values representing frequency of each action in the strategy at this information set
    strategy regretMatch(InfoSetTable& iGlobal, GameState g, GameState::Card c);

    //updates strategy sum for each action in information set table
    void updateStrategySum(InfoSetTable& iTemp, GameState g, GameState::Card c, double ownReachContribution, Solve::strategy s);

    //adds reach-weighted regrets to each action's cumulative regret in table for this iteration
    void addRegretsToTable(Solve::EV cEV, InfoSetTable& i, GameState g, GameState::Card c, reachProb r, bool isP1Turn);

    //helper method to get values from children
    EV getValuesFromChildren(InfoSetTable& iGlobal, InfoSetTable& iTemp, GameState g, reachProb r, strategy s, bool isP1Turn);
};

#endif