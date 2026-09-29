//Edited by Kian Choi 9/28/26

#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include <unordered_map>
#include "InfoSetTable.hpp"
#include "GameState.hpp"
#include "Solve.hpp"

class Evaluator {
private:
    struct NodeEvaluation {
        double opponentReach;
        double nodeEV;
    };

    std::unordered_map<int, NodeEvaluation> nodeEval;

public:
    //populates nodeEval with P1 EV and opponent reach for every hidden node under the trained average strategy
    //prints the starting P1 EV if printStartingEV is true
    //to be used on final infoSetTable after training
    void updateNodeEvalAndPrintStartingEV(InfoSetTable& iGlobal, bool printStartingEV);

    //recursively evaluates a game-tree node and stores its EV and opponent reach in nodeEval
    double populateNodeEval(InfoSetTable& iGlobal, GameState g, Solve::reachProb r, bool isP1Turn);

    //helper for populateNodeEval
    Solve::EV evaluateChildren(InfoSetTable& iGlobal, GameState g, Solve::reachProb r, Solve::strategy s, bool isP1Turn);

    //encodes an action history and both private cards into a unique hidden-node ID
    //each card uses 2 bits and the action history uses 4 bits
    int lookupID(GameState g);

    //returns the opponent-reach-weighted EV of an information set
    double getInfoSetEV(GameState g1);

    //returns the information-set EV after taking the given action
    double getInfoSetActionEV(GameState g, GameState::Action a);

    //prints the EV of every information set
    void printEVs();

    //adds all terminal game-tree nodes to nodeEval
    void addTerminalNodesToMap();

    //returns P1's best-response EV for the given private card against P2's fixed strategy
    double getP1BestResponseEV(InfoSetTable iGlobal, GameState g);

    //returns P2's best-response EV for the given private card against P1's fixed strategy
    double getP2BestResponseEV(InfoSetTable iGlobal, GameState g);

    //prints the average best-response EV for each player across all private cards
    void printBestResponses(InfoSetTable iGlobal);

    //returns the NashConv of the current average strategy
    double getNashConv(InfoSetTable iGlobal);

    //returns the exploitability of the current average strategy
    double getExploitability(InfoSetTable iGlobal);
 
    //prints the NashConv and exploitability of the current average strategy.
    void printConvergenceMetrics(InfoSetTable iGlobal);

};

#endif