//Edited by Kian Choi 9/28/26

#include <iostream>
#include <algorithm>
#include "Evaluator.hpp"

void Evaluator::updateNodeEvalAndPrintStartingEV(InfoSetTable& iGlobal, bool printStartingEV) {

    GameState g1(GameState::Card::J, GameState::Card::Q, GameState::ActionHistory::Start);
    GameState g2(GameState::Card::J, GameState::Card::K, GameState::ActionHistory::Start);
    GameState g3(GameState::Card::Q, GameState::Card::J, GameState::ActionHistory::Start);
    GameState g4(GameState::Card::Q, GameState::Card::K, GameState::ActionHistory::Start);
    GameState g5(GameState::Card::K, GameState::Card::J, GameState::ActionHistory::Start);
    GameState g6(GameState::Card::K, GameState::Card::Q, GameState::ActionHistory::Start);
    Solve::reachProb r1;
    Solve::reachProb r2;
    Solve::reachProb r3;
    Solve::reachProb r4;
    Solve::reachProb r5;
    Solve::reachProb r6;
    double e1 = populateNodeEval(iGlobal, g1, r1, true);
    double e2 = populateNodeEval(iGlobal, g2, r2, true);
    double e3 = populateNodeEval(iGlobal, g3, r3, true);
    double e4 = populateNodeEval(iGlobal, g4, r4, true);
    double e5 = populateNodeEval(iGlobal, g5, r5, true);
    double e6 = populateNodeEval(iGlobal, g6, r6, true);
    if (printStartingEV) {
        double startingEV = (e1 + e2 + e3 + e4 + e5 + e6) / 6;
        std::cout << "Starting EV: " << startingEV << '\n';
    }
}

double Evaluator::populateNodeEval(InfoSetTable& iGlobal, GameState g, Solve::reachProb r, bool isP1Turn) {

    GameState::Card c;

    if (isP1Turn) {
        c = g.getCard1();
    }
    else {
        c = g.getCard2();
    }
        
    //Base case: terminal state returns payoff
    if (g.isStateTerminal()) {
        return g.p1PayoffIfTerminal();
    }

    //normalize the trained average strategy at the acting player's information set    
    
    double checkSum = 0.0;
    double betSum = 0.0;
    double foldSum = 0.0;
    double callSum = 0.0;

    if (g.isAbleToBet()) {
        checkSum = iGlobal.getStrategySum(g.getActionHistory(), c, GameState::Action::Check);
        betSum = iGlobal.getStrategySum(g.getActionHistory(), c, GameState::Action::Bet);
        iGlobal.normalizeSums(checkSum, betSum);
    }
    else {
        foldSum = iGlobal.getStrategySum(g.getActionHistory(), c, GameState::Action::Fold);
        callSum = iGlobal.getStrategySum(g.getActionHistory(), c, GameState::Action::Call);
        iGlobal.normalizeSums(foldSum, callSum);
    }

    Solve::strategy s(checkSum, betSum, foldSum, callSum);

    //recursively evaluate each child node
    Solve::EV cEV = Evaluator::evaluateChildren(iGlobal, g, r, s, isP1Turn);

    //calculate this node's EV under the trained average strategy
    if (g.isAbleToBet()) {
        cEV.stratVal = cEV.checkEV * s.checkFreq + cEV.betEV * s.betFreq;
    }
    else {
        cEV.stratVal = cEV.foldEV * s.foldFreq + cEV.callEV * s.callFreq;
    }

    //store the node EV and the acting player's opponent reach
    if (isP1Turn) {
        nodeEval[lookupID(g)].opponentReach = r.p2;
    }
    else {
        nodeEval[lookupID(g)].opponentReach = r.p1;
    }
    nodeEval[lookupID(g)].nodeEV = cEV.stratVal;

    return cEV.stratVal;
}

Solve::EV Evaluator::evaluateChildren(InfoSetTable& iGlobal, GameState g, Solve::reachProb r, Solve::strategy s, bool isP1Turn) {
    
    double checkEV = 0.0;
    double betEV = 0.0;
    double foldEV = 0.0;
    double callEV = 0.0;

    //finds expected value of checking or betting
    if (g.isAbleToBet()) {

        //if we can only bet/check, create new gamestates for recursion and update reach probabilities based on who's turn it is
        GameState gCheck(g.getCard1(), g.getCard2(), g.resultAction(GameState::Action::Check));
        GameState gBet(g.getCard1(), g.getCard2(), g.resultAction(GameState::Action::Bet));
        Solve::reachProb rCheck = r;
        Solve::reachProb rBet = r;

        if (isP1Turn) {
            rCheck.p1 *= s.checkFreq;
            rBet.p1 *= s.betFreq;
        }
        else {
            rCheck.p2 *= s.checkFreq;
            rBet.p2 *= s.betFreq;
        }
        
        //recursively evaluate both child nodes
        checkEV = populateNodeEval(iGlobal, gCheck, rCheck, !isP1Turn);
        betEV = populateNodeEval(iGlobal, gBet, rBet, !isP1Turn);
    }

    //finds expected value of folding or calling
    else {

        //if we can only fold/call, create new gamestates for recursion and update reach probabilities based on whos turn it is
        GameState gFold(g.getCard1(), g.getCard2(), g.resultAction(GameState::Action::Fold));
        GameState gCall(g.getCard1(), g.getCard2(), g.resultAction(GameState::Action::Call));
        Solve::reachProb rFold = r;
        Solve::reachProb rCall = r;

        if (isP1Turn) {
            rFold.p1 *= s.foldFreq;
            rCall.p1 *= s.callFreq;
        }
        else {
            rFold.p2 *= s.foldFreq;
            rCall.p2 *= s.callFreq;
        }
        
        //recursively evaluate both child nodes
        foldEV = populateNodeEval(iGlobal, gFold, rFold, !isP1Turn);
        callEV = populateNodeEval(iGlobal, gCall, rCall, !isP1Turn);

    }
    Solve::EV cEV(0.0, checkEV, betEV, foldEV, callEV);
    return cEV;
}

int Evaluator::lookupID(GameState g) {
    int a = static_cast<int>(g.getActionHistory());
    int card1 = static_cast<int>(g.getCard1());
    int card2 = static_cast<int>(g.getCard2());
    if ((a < 0) || (a > 8)) {
        throw std::runtime_error("Invalid action history in lookupID.");
    }
    if ((card1 < 0) || (card1 > 2)) {
        throw std::runtime_error("Invalid P1 card in lookupID.");
    }
    if ((card2 < 0) || (card2 > 2)) {
        throw std::runtime_error("Invalid P2 card in lookupID.");
    }
    return (a << 4) + (card1 << 2) + card2;
}

double Evaluator::getInfoSetEV(GameState g) {
    if (g.isStateTerminal()) {
        return g.p1PayoffIfTerminal();
    }

    addTerminalNodesToMap();

    bool isP1Turn = g.isP1Turn();

    //construct the two hidden nodes consistent with the acting player's private card
    int ID1;
    int ID2;
    InfoSetTable i;

    GameState::Card c1 = static_cast<GameState::Card>((static_cast<int>(g.getCard1()) + 1) % 3);
    GameState::Card c2 = static_cast<GameState::Card>(3 - static_cast<int>(g.getCard1()) - static_cast<int>(c1));

    if (isP1Turn) {
        //gets other possible gameState by replacing P2's card with other possible card
        //Card1 in this case means the acting player's card
        GameState g1(g.getCard1(), c1, g.getActionHistory());
        GameState g2(g.getCard1(), c2, g.getActionHistory());
        ID1 = Evaluator::lookupID(g1);
        ID2 = Evaluator::lookupID(g2);
    }
    else {
        //gets other possible gameState by replacing P1's card with other possible card
        GameState g1(c1, g.getCard1(), g.getActionHistory());
        GameState g2(c2, g.getCard1(), g.getActionHistory());
        ID1 = Evaluator::lookupID(g1);
        ID2 = Evaluator::lookupID(g2);
    }

    //normalize opponent reaches to obtain the conditional probability of each hidden node
    double reach1 = nodeEval.at(ID1).opponentReach;
    double reach2 = nodeEval.at(ID2).opponentReach;
    i.normalizeSums(reach1, reach2);

    //nodeEV is stored from P1's perspective; negate it when P2 is the acting player.
    if (!isP1Turn) {
        return -1 * (reach1 * nodeEval.at(ID1).nodeEV + reach2 * nodeEval.at(ID2).nodeEV);
    }
    return reach1 * nodeEval.at(ID1).nodeEV + reach2 * nodeEval.at(ID2).nodeEV;

}

double Evaluator::getInfoSetActionEV(GameState g, GameState::Action a) {
    if (g.isStateTerminal()) {
        return g.p1PayoffIfTerminal();
    }

    addTerminalNodesToMap();

    bool isP1Turn = g.isP1Turn();

    //construct the hidden nodes consistent with the acting player's private card
    int ID1;
    int ID2;
    int ID3;
    int ID4;
    InfoSetTable i;

    GameState::Card c1 = static_cast<GameState::Card>((static_cast<int>(g.getCard1()) + 1) % 3);
    GameState::Card c2 = static_cast<GameState::Card>(3 - static_cast<int>(g.getCard1()) - static_cast<int>(c1));

    if (isP1Turn) {
        //gets other possible gameState by replacing P2's card with other possible card
        //Card1 in this case means the acting player's card
        GameState g1(g.getCard1(), c1, g.getActionHistory());
        GameState g2(g.getCard1(), c2, g.getActionHistory());
        GameState g3(g.getCard1(), c1, g.resultAction(a));
        GameState g4(g.getCard1(), c2, g.resultAction(a));
        ID1 = Evaluator::lookupID(g1);
        ID2 = Evaluator::lookupID(g2);
        ID3 = Evaluator::lookupID(g3);
        ID4 = Evaluator::lookupID(g4);
    }
    else {
        //gets other possible gameState by replacing P1's card with other possible card
        GameState g1(c1, g.getCard1(), g.getActionHistory());
        GameState g2(c2, g.getCard1(), g.getActionHistory());
        GameState g3(c1, g.getCard1(), g.resultAction(a));
        GameState g4(c2, g.getCard1(), g.resultAction(a));
        ID1 = Evaluator::lookupID(g1);
        ID2 = Evaluator::lookupID(g2);
        ID3 = Evaluator::lookupID(g3);
        ID4 = Evaluator::lookupID(g4);
    }

    //weight the resulting child-node EVs by the normalized opponent reaches
    double reach1 = nodeEval.at(ID1).opponentReach;
    double reach2 = nodeEval.at(ID2).opponentReach;
    i.normalizeSums(reach1, reach2);

    if (!isP1Turn) {
        return -1 * (reach1 * nodeEval.at(ID3).nodeEV + reach2 * nodeEval.at(ID4).nodeEV);
    }
    return reach1 * nodeEval.at(ID3).nodeEV + reach2 * nodeEval.at(ID4).nodeEV;
}

void Evaluator::printEVs() {
    //nonterminal action histories correspond to enum values 0-3
    int cards = 3;
    int actionHistories = 4;
    InfoSetTable i;

    for (int ah = 0; ah < actionHistories; ah++) {
        for (int c = 0; c < cards; c++) {     
            GameState::ActionHistory history = static_cast<GameState::ActionHistory>(ah);
            GameState::Card card = static_cast<GameState::Card>(c);

            GameState g(card, history);

            std::cout << "Card: " << i.cardToString(card) << ", Action History: " << i.historyToString(history) << ", InfoSetEV (player to act's perspective) = " << getInfoSetEV(g) << '\n';
            
            if (g.isAbleToBet()) {
                std::cout << '\t' << "EV of Check: " << getInfoSetActionEV(g, GameState::Action::Check) << '\n';
                std::cout << '\t' << "EV of Bet: " << getInfoSetActionEV(g, GameState::Action::Bet) << '\n';
            }
            else {
                std::cout << '\t' << "EV of Fold: " << getInfoSetActionEV(g, GameState::Action::Fold) << '\n';
                std::cout << '\t' << "EV of Call: " << getInfoSetActionEV(g, GameState::Action::Call) << '\n';
            }
        }
    }
}

void Evaluator::addTerminalNodesToMap() {
    for (int termState = 4; termState <= 8; termState++) {
        for (int c = 0; c <= 2; c++) {
            for (int c2 = 0; c2 <= 2; c2++) {
                if (c2 != c) {
                    GameState::ActionHistory tState = static_cast<GameState::ActionHistory>(termState);
                    GameState::Card card = static_cast<GameState::Card>(c);
                    GameState::Card card2 = static_cast<GameState::Card>(c2);

                    GameState g(card, card2, tState);

                    nodeEval[lookupID(g)].nodeEV = g.p1PayoffIfTerminal();
                }
            }
        }
    }
}

double Evaluator::getP1BestResponseEV(InfoSetTable iGlobal, GameState g) {
    GameState g2(g.getCard1(), GameState::ActionHistory::CheckBet);
    //P1's best response after checking and facing a bet.
    double BRCheckBet = std::max(getInfoSetActionEV(g2, GameState::Action::Fold), getInfoSetActionEV(g2, GameState::Action::Call));

    GameState::Card c1 = static_cast<GameState::Card>((static_cast<int>(g.getCard1()) + 1) % 3);
    GameState::Card c2 = static_cast<GameState::Card>(3 - static_cast<int>(g.getCard1()) - static_cast<int>(c1));

    double p2CheckC1 = iGlobal.getStrategySum(GameState::ActionHistory::Check, c1, GameState::Action::Check);
    double p2BetC1 = iGlobal.getStrategySum(GameState::ActionHistory::Check, c1, GameState::Action::Bet);
    double p2CheckC2 = iGlobal.getStrategySum(GameState::ActionHistory::Check, c2, GameState::Action::Check);
    double p2BetC2 = iGlobal.getStrategySum(GameState::ActionHistory::Check, c2, GameState::Action::Bet);

    //normalized P2 Check/Bet strategy for each possible P2 card
    iGlobal.normalizeSums(p2CheckC1, p2BetC1);
    iGlobal.normalizeSums(p2CheckC2, p2BetC2);

    //returns -1 if C1/C2 is higher, 1 if C1/C2 is lower
    int showdownValueC1 = (static_cast<int>(g.getCard1()) > static_cast<int>(c1)) * 2 - 1;
    int showdownValueC2 = (static_cast<int>(g.getCard1()) > static_cast<int>(c2)) * 2 - 1;

    //gets check value for each hidden node
    double checkC1 = p2CheckC1 * showdownValueC1 + p2BetC1 * BRCheckBet;
    double checkC2 = p2CheckC2 * showdownValueC2 + p2BetC2 * BRCheckBet;

    //average over the two possible P2 private cards
    double checkVal = 0.5 * checkC1 + 0.5 * checkC2;
    double betVal = getInfoSetActionEV(g, GameState::Action::Bet);

    return std::max(checkVal, betVal);
}

double Evaluator::getP2BestResponseEV(InfoSetTable iGlobal, GameState g) {
    GameState::Card c1 = static_cast<GameState::Card>((static_cast<int>(g.getCard1()) + 1) % 3);
    GameState::Card c2 = static_cast<GameState::Card>(3 - static_cast<int>(g.getCard1()) - static_cast<int>(c1));

    GameState g2(g.getCard1(), GameState::ActionHistory::Check);
    GameState g3(g.getCard1(), GameState::ActionHistory::Bet);

    double BRCheck = std::max(getInfoSetActionEV(g2, GameState::Action::Check), getInfoSetActionEV(g2, GameState::Action::Bet));
    double BRBet = std::max(getInfoSetActionEV(g3, GameState::Action::Fold), getInfoSetActionEV(g3, GameState::Action::Call));

    double p1CheckC1 = iGlobal.getStrategySum(GameState::ActionHistory::Start, c1, GameState::Action::Check);
    double p1BetC1 = iGlobal.getStrategySum(GameState::ActionHistory::Start, c1, GameState::Action::Bet);
    double p1CheckC2 = iGlobal.getStrategySum(GameState::ActionHistory::Start, c2, GameState::Action::Check);
    double p1BetC2 = iGlobal.getStrategySum(GameState::ActionHistory::Start, c2, GameState::Action::Bet);

    //normalized P1 Check/Bet strategy for each possible P1 card
    iGlobal.normalizeSums(p1CheckC1, p1BetC1);
    iGlobal.normalizeSums(p1CheckC2, p1BetC2);

    //average over the two possible P1 private cards

    double c1Val = BRCheck * p1CheckC1 + BRBet * p1BetC1;
    double c2Val = BRCheck * p1CheckC2 + BRBet * p1BetC2;

    double totVal = (c1Val + c2Val) / 2;

    return totVal;

}

void Evaluator::printBestResponses(InfoSetTable iGlobal) {
    GameState g1(GameState::Card::J, GameState::ActionHistory::Start);
    GameState g2(GameState::Card::Q, GameState::ActionHistory::Start);
    GameState g3(GameState::Card::K, GameState::ActionHistory::Start);
    double BRP1 = (getP1BestResponseEV(iGlobal, g1) + getP1BestResponseEV(iGlobal, g2) + getP1BestResponseEV(iGlobal, g3)) / 3;
    double BRP2 = (getP2BestResponseEV(iGlobal, g1) + getP2BestResponseEV(iGlobal, g2) + getP2BestResponseEV(iGlobal, g3)) / 3;
    std::cout << "P1's Best Response: " << BRP1 << '\n';
    std::cout << "P2's Best Response: " << BRP2 << '\n';
}

double Evaluator::getNashConv(InfoSetTable iGlobal) {
    GameState g1(GameState::Card::J, GameState::ActionHistory::Start);
    GameState g2(GameState::Card::Q, GameState::ActionHistory::Start);
    GameState g3(GameState::Card::K, GameState::ActionHistory::Start);
    double BRP1 = (getP1BestResponseEV(iGlobal, g1) + getP1BestResponseEV(iGlobal, g2) + getP1BestResponseEV(iGlobal, g3)) / 3;
    double BRP2 = (getP2BestResponseEV(iGlobal, g1) + getP2BestResponseEV(iGlobal, g2) + getP2BestResponseEV(iGlobal, g3)) / 3;

    return BRP1 + BRP2;
}

double Evaluator::getExploitability(InfoSetTable iGlobal) {
    return getNashConv(iGlobal) / 2;
}

void Evaluator::printConvergenceMetrics(InfoSetTable iGlobal) {
    std::cout << "Nash Convergence: " << getNashConv(iGlobal) << ", Exploitability: " << getExploitability(iGlobal) << '\n';
}