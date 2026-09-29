//Edited by Kian Choi 9/28/26

#ifndef INFOSETTESTS_HPP
#define INFOSETTESTS_HPP

#include "InfoSetTable.hpp"
#include "GameState.hpp"

class InfoSetTests {
public: 
    //tests valid information-set queries
    void testLegalQueries();

    //for comparing doubles safely
    bool approximatelyEqual(double a, double b, double epsilon);

    //helper for test output
    void check(bool condition, const char* testName, int& passed, int& failed);

    //tests regret and strategy-sum storage and updates
    void testInfoSetTable();

    //tests invalid information-set operations that should throw exceptions
    void testInfoSetExceptions();

    //tests additional invalid inputs and edge cases
    void testAdditionalInfoSetExceptions();

};

#endif