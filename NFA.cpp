//
// Created by Siebe Mees on 27/03/2023.
//

#include "NFA.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <set>
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
    dfa.setStartState(startState);

    // States & transitionFunction
    // Stap 1: Initialiseer de vector van DFA-toestanden
    vector<string> dfaStates;
    //= {startState};

    // Stap 2: Initialiseer de verzameling verwerkte toestanden
    set<vector<string>> processedStates = {{startState}};

    // Stap 3: Initialiseer de wachtrij met alle staten van de NFA
    queue<vector<string>> stateQueue;
    for (int i = 0; i < states.size(); ++i) {
        stateQueue.push({states[i]});
    }

    // Stap 4: Verwerk de wachtrij
    while (!stateQueue.empty()) {
        // a. Haal de volgende toestand uit de wachtrij
        vector<string> nfaStates = stateQueue.front();
        stateQueue.pop();
        // b. Genereer de overgangen voor elke invoer van het alfabet van de NFA
        //map<char, vector<string>> dfaTransitions;
        for (char c : alfabet) {
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
            }
        }
        // c. Voeg de nieuwe DFA-toestanden toe aan de vector van DFA-toestanden en de wachtrij als ze nog niet eerder zijn verwerkt
        /*for (const auto& pair : dfaTransitions) {
            const vector<string>& nfaTransitions = pair.second;
            if (processedStates.find(nfaTransitions)  == processedStates.end()) {
                dfaStates.push_back(getStringFromNFAStates(nfaTransitions));
                stateQueue.push(nfaTransitions);
                processedStates.insert(nfaTransitions);
            }
        }
        // d. Voeg de nieuwe overgangen toe aan de overgangsfunctie van de DFA
        string dfaState = getStringFromNFAStates(nfaStates);
        dfaStates.push_back(dfaState);
        for (const auto& pair : dfaTransitions) {
            char inputSymbol = pair.first;
            const vector<string>& nfaTransitions = pair.second;
            string dfaTargetState = getStringFromNFAStates(nfaTransitions);
            dfa.addTransition(dfaState, inputSymbol, dfaTargetState);
        }*/
        // e. Markeer de huidige toestand als verwerkt
        processedStates.insert(nfaStates);
    }

    // Stap 5: Stel de accepterende toestanden in voor de DFA
    for (string dfaState : dfaStates) {
        vector<string> nfaStates = getNFAStatesFromString(dfaState);
        for (string nfaState : nfaStates) {
            if (find(acceptStates.begin(), acceptStates.end(), nfaState) != acceptStates.end()) {
                dfa.addAcceptState(dfaState);
                break;
            }
        }
    }

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