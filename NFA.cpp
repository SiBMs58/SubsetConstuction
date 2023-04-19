//
// Created by Siebe Mees on 27/03/2023.
//

#include "NFA.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <queue>
#include "json.hpp"

using namespace std;

using json = nlohmann::json;

NFA::NFA() {}

NFA::NFA(const string inputFile) {
    // inlezen uit file
    ifstream input(inputFile);

    json j;
    input >> j;

    // Access the elements of the "alfabet" array
    string alfabet;
    for (const auto& letter : j["alphabet"]) {
        alfabet += letter;
    }
    setAlfabet(alfabet);

    // Access the elements of the "states" array
    vector<string> states;
    string startState;
    vector<string> acceptStates;
    for (const auto& state : j["states"]) {
        states.push_back(state["name"]);
        if (state["starting"] == true){
            startState = state["name"];
        }
        if (state["accepting"] == true){
            acceptStates.push_back(state["name"]);
        }
    }
    setStates(states);
    setAcceptStates(acceptStates);
    setStartState(startState);

    // Access the elements of the "transitions" array
    // Create a map to hold the transition function
    map<pair<string, char>, vector<string>> transitionFunction;
    // Access the elements of the "transitions" array
    for (const auto& transition : j["transitions"]) {
        // Get the "from" state
        string fromState = transition["from"];
        // Get the "to" state
        string toState = transition["to"];
        // Get the input character
        string input = transition["input"];
        char inputChar = input[0];
        // Add the transition to the map
        transitionFunction[make_pair(fromState, inputChar)].push_back(toState);
    }
    setTransitionFunction(transitionFunction);
}

bool stringExistsInQueue(queue<string> q, string s) {
    while (!q.empty()) {
        if (q.front() == s) {
            return true;
        }
        q.pop();
    }
    return false;
}

vector<string> getNFAStatesFromString(const string& state_string) {
    vector<string> nfa_states;
    string state_without_braces = state_string.substr(1, state_string.length() - 2);
    string delimiter = ",";
    size_t pos = 0;
    while ((pos = state_without_braces.find(delimiter)) != std::string::npos) {
        string nfa_state = state_without_braces.substr(0, pos);
        nfa_states.push_back(nfa_state);
        state_without_braces.erase(0, pos + delimiter.length());
    }
    nfa_states.push_back(state_without_braces);
    return nfa_states;
}

string getStringFromNFAStates(const vector<string>& states) {
    string state_string = "{";
    for (int i = 0; i < states.size(); ++i) {
        state_string += states[i];
        if (i != states.size() - 1) {
            state_string += ",";
        }
    }
    state_string += "}";
    return state_string;
}

DFA NFA::toDFA() {
    // Creëert een DFA
    DFA dfa;

    // Alfabet
    string alfabetString;
    for (char c : alfabet) {
        alfabetString += c;
    }
    dfa.setAlfabet(alfabetString);

    // Start state
    dfa.setStartState("{"+startState+"}");

    // States & transitionFunction
    // Stap 1: Initialiseer de verzameling verwerkte toestanden
    vector<string> processedStates;
    processedStates.push_back("{"+startState+"}");

    // Stap 2: Initialiseer de wachtrij met alle staten van de NFA
    queue<string> stateQueue;
    stateQueue.push("{"+startState+"}");

    // Stap 3: Verwerk de wachtrij
    while (!stateQueue.empty()) {
        // a. Haal de volgende toestand uit de wachtrij
        vector<string> nfaStates = getNFAStatesFromString(stateQueue.front());
        stateQueue.pop();

        // b. Genereer de overgangen voor elke invoer van het alfabet van de NFA
        for (char c : alfabet) {
            // b' Bereken alle staten die de nfaState kan bereiken
            vector<string> nfaTransitions;
            for (string nfaState : nfaStates) {
                auto it = transitionFunction.find(make_pair(nfaState, c));
                if (it != transitionFunction.end()) {
                    nfaTransitions.insert(nfaTransitions.end(), it->second.begin(), it->second.end());
                }

            }
            sort(nfaTransitions.begin(), nfaTransitions.end());
            nfaTransitions.erase(unique(nfaTransitions.begin(), nfaTransitions.end()), nfaTransitions.end());
            if (!nfaTransitions.empty()) {
                dfa.addTransition(getStringFromNFAStates(nfaStates), c, getStringFromNFAStates(nfaTransitions));
                // b'' Kijkt in de verzameling van states indien de "to" State al verwerkt is TODO: Zoek een manier om in ons geval "Q1" ook toe te voegen
                if (find(processedStates.begin(), processedStates.end(), getStringFromNFAStates(nfaTransitions)) == processedStates.end()) {
                    stateQueue.push(getStringFromNFAStates(nfaTransitions));
                    for (int i = 0; i < nfaTransitions.size(); ++i) {
                        if (!stringExistsInQueue(stateQueue, "{"+nfaTransitions[i]+"}") || (find(processedStates.begin(), processedStates.end(), "{"+nfaTransitions[i]+"}") != processedStates.end())) {
                            stateQueue.push("{"+nfaTransitions[i]+"}");
                        }
                    }
                }
            }
        }
        // c. Markeer de huidige toestand als verwerkt en voeg dazn ook toe aan de staten van de DFA als deze nog niet bestaat natuurlijk
        vector<string> dfaState = dfa.getStates();
        if (find(dfaState.begin(), dfaState.end(), getStringFromNFAStates(nfaStates)) == dfaState.end()) {
            dfa.addState(getStringFromNFAStates(nfaStates));
        }
        processedStates.push_back(getStringFromNFAStates(nfaStates));
    }

    // Stap 4: Bepaal alle accepterende toestanden in voor de DFA
    /*vector<string> nfaStates = getNFAStatesFromString(dfa.);
    for (string nfaState : nfaStates) {
        if (find(acceptStates.begin(), acceptStates.end(), nfaState) != acceptStates.end()) {
            dfa.addAcceptState(dfaState);
            break;
        }
    }*/


    return dfa;
}


void NFA::setStates(const vector<string> &states) {
    NFA::states = states;
}

void NFA::setAlfabet(const string &alfabet) {
    vector<char> chars;
    for (int i = 0; i < alfabet.size(); ++i) {
        chars.push_back(alfabet[i]);
    }
    NFA::alfabet = chars;
}

void NFA::setTransitionFunction(const map<pair<string, char>, vector<string>>  &transitionFunction) {
    NFA::transitionFunction = transitionFunction;
}

void NFA::setStartState(const string &startState) {
    NFA::startState = startState;
}

void NFA::setAcceptStates(const vector<string> &acceptStates) {
    NFA::acceptStates = acceptStates;
}