//Edited by Kian Choi 9/28/26

#ifndef EVALUATORTESTS_HPP
#define EVALUATORTESTS_HPP

class EvaluatorTests {
public:

    //tests starting EV against a manually verified uniform strategy
    void testStartingEV();

    //tests information-set EVs against manually calculated values
    void testGetInfoSetEV();

    //tests P1/P2 best responses, NashConv, and exploitability against manually calculated values
    void testBestResponse();

    //integration test between Solve and Evaluator. Checks that CFR approaches a Kuhn equilibrium
    void testConvergence();
};

#endif