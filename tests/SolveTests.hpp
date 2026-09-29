//Edited by Kian Choi 9/28/26

#ifndef SOLVETESTS_HPP
#define SOLVETESTS_HPP

class SolveTests {
public:
    //tests regret matching: uniform strategy when all cumulative regrets are nonpositive,
    //otherwise proportional to positive cumulative regrets
    void testRegretMatch();

    //tests that strategy sums receive the correct reach-weighted contributions
    void testUpdateStrategySum();

    //tests that cumulative regrets receive the correct reach-weighted updates
    void testAddRegretsToTable();

};

#endif