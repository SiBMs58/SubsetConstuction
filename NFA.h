//
// Created by Siebe Mees on 27/03/2023.
//

#ifndef SUBSETCONSTRUCTION_NFA_H
#define SUBSETCONSTRUCTION_NFA_H

#include <string>
#include <vector>
#include <map>
#include "DFA.h"

using namespace std;


class NFA {
private:
    vector<string> states;
    vector<char> alfabet;
    map<pair<string, char>, vector<string>> transitionFunction;
    string startState;
    vector<string> acceptStates;
public:
    // Default constructor
    NFA();
    // Inputfile constructor
    NFA(const string inputFile);

    DFA toDFA();

    // Setters
    void setStates(const vector<string> &states);
    void setAlfabet(const string &alfabet);
    void setTransitionFunction(const map<pair<string, char>, vector<string>>  &transitionFunction);
    void setStartState(const string &startState);
    void setAcceptStates(const vector<string> &acceptStates);

};


#endif //SUBSETCONSTRUCTION_NFA_H
