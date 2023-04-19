#include "NFA.h"

using namespace std;
int main() {
    /*DFA dfa("../DFA.json");
    cout << boolalpha << dfa.accepts("0010110100") << endl << dfa.accepts("0001") << endl;
    dfa.print();*/
    //NFA nfa("../input-ssc-cursus.json");
    NFA nfa("../input-ssc1.json");
    nfa.toDFA().print();
    return 0;
}