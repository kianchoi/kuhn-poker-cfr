//Edited by Kian Choi 9/28/26

#include <cmath>
#include "Solve.hpp"
#include "InfoSetTable.hpp"
#include "GameState.hpp"

double Solve::getInfoSetEV(InfoSetTable& iGlobal, InfoSetTable& iTemp, GameState g, reachProb r, bool isP1Turn) {

    GameState::Card c;

    if (isP1Turn) {
        c = g.getCard1();
    }
    else {
        c = g.getCard2();
    }

    //CFR traversal at the current game-tree node:
    //1. If terminal node, perform calculation and pass up value
    //2. Regret match to find strategy using cumulative regrets of each action in info set table
    //3. Update average strategy values using reach probability * probability of each action
    //4. Recurse down tree to find strategy values of children
    //5. Calculate strategy value with current strategy % * action value coming from children for each action
    //6. Add reach-weighted regrets to each action’s cumulative regret in table
    //7. Pass total strategy value up
        
    //Base case: terminal state returns payoff
    if (g.isStateTerminal()) {
        return g.p1PayoffIfTerminal();
    }

    //regret match to create strategy
    Solve::strategy s = regretMatch(iGlobal, g, c);

    //updates strategy sum in temporary InfoSetTable
    if (isP1Turn) {
        updateStrategySum(iTemp, g, c, r.p1, s);
    }
    else {
        updateStrategySum(iTemp, g, c, r.p2, s);
    }

    //find strategy values of children using recursion
    EV cEV = getValuesFromChildren(iGlobal, iTemp, g, r, s, isP1Turn);

    //calculates strategy value at this information set
    if (g.isAbleToBet()) {
        cEV.stratVal = cEV.checkEV * s.checkFreq + cEV.betEV * s.betFreq;
    }
    else {
        cEV.stratVal = cEV.foldEV * s.foldFreq + cEV.callEV * s.callFreq;
    }

    //add reach-weighted regrets to each action's cumulative regret in table
    addRegretsToTable(cEV, iTemp, g, c, r, isP1Turn);

    return cEV.stratVal;
}

void Solve::performOneIteration(InfoSetTable& iGlobal) {
    InfoSetTable iTemp;
    GameState g1(GameState::Card::J, GameState::Card::Q, GameState::ActionHistory::Start);
    GameState g2(GameState::Card::J, GameState::Card::K, GameState::ActionHistory::Start);
    GameState g3(GameState::Card::Q, GameState::Card::J, GameState::ActionHistory::Start);
    GameState g4(GameState::Card::Q, GameState::Card::K, GameState::ActionHistory::Start);
    GameState g5(GameState::Card::K, GameState::Card::J, GameState::ActionHistory::Start);
    GameState g6(GameState::Card::K, GameState::Card::Q, GameState::ActionHistory::Start);
    reachProb r1;
    reachProb r2;
    reachProb r3;
    reachProb r4;
    reachProb r5;
    reachProb r6;
    getInfoSetEV(iGlobal, iTemp, g1, r1, true);
    getInfoSetEV(iGlobal, iTemp, g2, r2, true);
    getInfoSetEV(iGlobal, iTemp, g3, r3, true);
    getInfoSetEV(iGlobal, iTemp, g4, r4, true);
    getInfoSetEV(iGlobal, iTemp, g5, r5, true);
    getInfoSetEV(iGlobal, iTemp, g6, r6, true);

    //Now we have to transfer all of the reach-weighted strategy and regret values in iTemp to iGlobal

    //amount of cards / actionhistories / actions in their respective enums
    int cards = 3;
    int actionHistories = 4;
    int actions = 4;

    for (int i = 0; i < cards; i++) {
        for (int j = 0; j < actionHistories; j++) {
            for (int k = 0; k < actions; k++) {
                //convert iterators to card and action history enums
                GameState::Card c = static_cast<GameState::Card>(i);
                GameState::ActionHistory ah = static_cast<GameState::ActionHistory>(j);
                GameState::Action a = static_cast<GameState::Action>(k);
                
                GameState g(c, ah);
    
                //these if statements ensure that only the correct action regret/strategy values are passed into the table
                //check or bet if the player is able to bet, otherwise call or fold
                //invalid actions do nothing
                if ((g.isAbleToBet()) && ((k == 0) || (k == 1))) {
                    double tempReg = iTemp.getCumulativeRegret(ah, c, a);
                    iGlobal.addWeightedRegret(ah, c, a, tempReg);
                    double tempStrat = iTemp.getStrategySum(ah, c, a);
                    iGlobal.addWeightedStrategy(ah, c, a, tempStrat);
                }
                else if ((!g.isAbleToBet()) && ((k == 2) || (k == 3))) {
                    double tempReg = iTemp.getCumulativeRegret(ah, c, a);
                    iGlobal.addWeightedRegret(ah, c, a, tempReg);
                    double tempStrat = iTemp.getStrategySum(ah, c, a);
                    iGlobal.addWeightedStrategy(ah, c, a, tempStrat);
                }

            }

        }
    }
}

InfoSetTable Solve::train(InfoSetTable& i, int iterations) {
    for (int j = 0; j < iterations; j++) {
            performOneIteration(i);
    }
    return i;
}

Solve::strategy Solve::regretMatch(InfoSetTable& iGlobal, GameState g, GameState::Card c) {
        
    constexpr double EPSILON = 1e-9;

    //for our purposes (regret matching), we want actions with negative cumulative regret to not be present in strategy.
    //total cumulative regret = sum of valid actions' cumulative regrets
    //we then divide each regret by totalReg to get individual strategies
    double checkReg = 0.0;
    double betReg = 0.0;
    double foldReg = 0.0;
    double callReg = 0.0;
    double totalReg = 0.0;
    Solve::strategy s;


    if(g.isAbleToBet()) {
        checkReg = std::max(0.0, iGlobal.getCumulativeRegret(g.getActionHistory(), c, GameState::Action::Check));
        betReg = std::max(0.0, iGlobal.getCumulativeRegret(g.getActionHistory(), c, GameState::Action::Bet));

        //in case of checkReg and betReg == 0.0, return uniform strategy
        if ((std::abs(checkReg) < EPSILON) && (std::abs(betReg) < EPSILON)) {
            s.checkFreq = 0.5;
            s.betFreq = 0.5;
        }
        //otherwise regret match
        else {
            totalReg = checkReg + betReg;
            s.checkFreq = checkReg / totalReg; //percent of the time check is taken in strategy
            s.betFreq = betReg / totalReg; //percent of the time bet is taken in strategy
        }

        if ((std::abs(s.callFreq) > EPSILON) || (std::abs(s.foldFreq) > EPSILON)) {
            throw std::runtime_error("Invalid frequencies (call, fold) are not 0 in regret matching.");
        }
    }
    else {
        callReg = std::max(0.0, iGlobal.getCumulativeRegret(g.getActionHistory(), c, GameState::Action::Call));
        foldReg = std::max(0.0, iGlobal.getCumulativeRegret(g.getActionHistory(), c, GameState::Action::Fold));
        
        //in case of callReg and foldReg == 0.0, return uniform strategy
        if ((std::abs(callReg) < EPSILON) && (std::abs(foldReg) < EPSILON)) {
            s.callFreq = 0.5;
            s.foldFreq = 0.5;
        }
        else {
            totalReg = callReg + foldReg;
            s.callFreq = callReg / totalReg; //percent of the time call is taken in strategy
            s.foldFreq = foldReg / totalReg; //percent of the time fold is taken in strategy
        }

        if ((std::abs(s.checkFreq) > EPSILON) || (std::abs(s.betFreq) > EPSILON)) {
            throw std::runtime_error("Invalid frequencies (check, bet) are not 0 in regret matching.");
        }
    }

    //now strategy inputs should add to 1, throw exception if they don't
    double totalStrategy = s.checkFreq + s.betFreq + s.callFreq + s.foldFreq;

    if (std::abs(totalStrategy - 1.0) > EPSILON) {
        throw std::runtime_error("Strategies don't add up to 1 in regret matching method.");
    }

    //now that strategy inputs add up to 1, return strategy
    return s;
}

void Solve::updateStrategySum(InfoSetTable& iTemp, GameState g, GameState::Card c, double ownReachContribution, Solve::strategy s) {
    //Accumulate each legal action's strategy weighted by the acting player's reach probability.
    if (g.isAbleToBet()) {
        iTemp.addWeightedStrategy(g.getActionHistory(), c, GameState::Action::Check, ownReachContribution * s.checkFreq);
        iTemp.addWeightedStrategy(g.getActionHistory(), c, GameState::Action::Bet, ownReachContribution * s.betFreq);
    }
    else {
        iTemp.addWeightedStrategy(g.getActionHistory(), c, GameState::Action::Fold, ownReachContribution * s.foldFreq);
        iTemp.addWeightedStrategy(g.getActionHistory(), c, GameState::Action::Call, ownReachContribution * s.callFreq);
    }
}

void Solve::addRegretsToTable(Solve::EV cEV, InfoSetTable& i, GameState g, GameState::Card c, reachProb r, bool isP1Turn) {

    //equals -1 if p2 is acting and 1 if p1 is acting
    //we want p2's cumulative regrets to be the opposite of p1 so p2's regret matching over time tries to beat p1
    int playerSign = isP1Turn * 2 - 1;

    if (g.isAbleToBet()) {
        double checkReg = playerSign * (cEV.checkEV - cEV.stratVal);
        double betReg = playerSign * (cEV.betEV - cEV.stratVal);

        //weight regret by all reach probabilities that aren't our own, add to information set table
        if (isP1Turn) {
            i.addWeightedRegret(g.getActionHistory(), c, GameState::Action::Check, (checkReg * r.chance * r.p2));
            i.addWeightedRegret(g.getActionHistory(), c, GameState::Action::Bet, (betReg * r.chance * r.p2));
        }
        else {
            i.addWeightedRegret(g.getActionHistory(), c, GameState::Action::Check, (checkReg * r.chance * r.p1));
            i.addWeightedRegret(g.getActionHistory(), c, GameState::Action::Bet, (betReg * r.chance * r.p1));
        }
    }
    else {
        double foldReg = playerSign * (cEV.foldEV - cEV.stratVal);
        double callReg = playerSign * (cEV.callEV - cEV.stratVal);

        //weight regret by all reach probabilities that aren't our own, add to information set table
        if (isP1Turn) {
            i.addWeightedRegret(g.getActionHistory(), c, GameState::Action::Fold, (foldReg * r.chance * r.p2));
            i.addWeightedRegret(g.getActionHistory(), c, GameState::Action::Call, (callReg * r.chance * r.p2));
        }
        else {
            i.addWeightedRegret(g.getActionHistory(), c, GameState::Action::Fold, (foldReg * r.chance * r.p1));
            i.addWeightedRegret(g.getActionHistory(), c, GameState::Action::Call, (callReg * r.chance * r.p1));
        }
    }
}

Solve::EV Solve::getValuesFromChildren(InfoSetTable& iGlobal, InfoSetTable& iTemp, GameState g, reachProb r, Solve::strategy s, bool isP1Turn) {
    
    double checkEV = 0.0;
    double betEV = 0.0;
    double foldEV = 0.0;
    double callEV = 0.0;

    //finds expected value of checking or betting
    if (g.isAbleToBet()) {

        //if we can only bet/check, create new gamestates for recursion and update reach probabilities based on who's turn it is
        GameState gCheck(g.getCard1(), g.getCard2(), g.resultAction(GameState::Action::Check));
        GameState gBet(g.getCard1(), g.getCard2(), g.resultAction(GameState::Action::Bet));
        reachProb rCheck = r;
        reachProb rBet = r;

        if (isP1Turn) {
            rCheck.p1 *= s.checkFreq;
            rBet.p1 *= s.betFreq;
        }
        else {
            rCheck.p2 *= s.checkFreq;
            rBet.p2 *= s.betFreq;
        }
        
        //recurse getInfoSetEV function until terminal state is reached, then values are passed up until they hit this information set
        checkEV = getInfoSetEV(iGlobal, iTemp, gCheck, rCheck, !isP1Turn);
        betEV = getInfoSetEV(iGlobal, iTemp, gBet, rBet, !isP1Turn);
    }

    //finds expected value of folding or calling
    else {

        //if we can only fold/call, create new gamestates for recursion and update reach probabilities based on whos turn it is
        GameState gFold(g.getCard1(), g.getCard2(), g.resultAction(GameState::Action::Fold));
        GameState gCall(g.getCard1(), g.getCard2(), g.resultAction(GameState::Action::Call));
        reachProb rFold = r;
        reachProb rCall = r;

        if (isP1Turn) {
            rFold.p1 *= s.foldFreq;
            rCall.p1 *= s.callFreq;
        }
        else {
            rFold.p2 *= s.foldFreq;
            rCall.p2 *= s.callFreq;
        }
        
        //recurse getInfoSetEV function until terminal state is reached, then values are passed up until they hit this information set
        foldEV = getInfoSetEV(iGlobal, iTemp, gFold, rFold, !isP1Turn);
        callEV = getInfoSetEV(iGlobal, iTemp, gCall, rCall, !isP1Turn);

    }
    Solve::EV cEV(0.0, checkEV, betEV, foldEV, callEV);
    return cEV;
}
