#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <algorithm>
#include <queue>

using namespace std;


// NFA representation.
//
// An NFA can have more than one destination for the same
// state and input, so each transition stores a set of states.
struct NFAMachine
{
    int numberOfStates;
    int numberOfInputs;

    vector<string> states;
    vector<string> inputs;

    // transition[state][input] = set of destination states
    vector<vector<set<int>>> transition;

    set<int> finalStates;

    int initialState;
};


// DFA representation.
//
// Unlike an NFA, a DFA has at most one destination for
// every state/input combination.
struct DFAMachine
{
    int numberOfStates;
    int numberOfInputs;

    vector<string> states;
    vector<string> inputs;

    // transition[state][input] = destination state
    // -1 means there is no transition.
    vector<vector<int>> transition;

    set<int> finalStates;

    int initialState;
};


// Simple helper used when displaying tables.
void printLine(int length = 70)
{
    for (int i = 0; i < length; i++)
    {
        cout << "-";
    }

    cout << endl;
}


// Returns the index of a state from its name.
// State names are used for input, but indexes make
// the transition tables easier to work with.
int findStateIndex(
    const vector<string>& states,
    const string& name)
{
    for (int i = 0; i < (int)states.size(); i++)
    {
        if (states[i] == name)
        {
            return i;
        }
    }

    return -1;
}


// Removes spaces from an input such as:
// "q0, q1" -> "q0,q1"
string removeSpaces(string text)
{
    text.erase(
        remove(text.begin(), text.end(), ' '),
        text.end()
    );

    return text;
}


// Gives a readable name to a DFA state created from
// a subset of NFA states.
//
// {q0}    -> q0
// {q0,q1} -> q0q1
//
// The empty set is represented by "-".
string subsetToName(
    const set<int>& stateSet,
    const vector<string>& states)
{
    if (stateSet.empty())
    {
        return "-";
    }

    string result;

    for (int state : stateSet)
    {
        result += states[state];
    }

    return result;
}


// Used when we want to display the actual subset.
//
// Example:
// {q0,q1}
string subsetToBracketString(
    const set<int>& stateSet,
    const vector<string>& states)
{
    if (stateSet.empty())
    {
        return "-";
    }

    string result = "{";
    bool first = true;

    for (int state : stateSet)
    {
        if (!first)
        {
            result += ",";
        }

        result += states[state];
        first = false;
    }

    result += "}";

    return result;
}


// Converts input such as:
// q0
// q0,q1
// q0,q1,q2
// -
//
// into a set of state indexes.
set<int> parseNFAStates(
    string text,
    const vector<string>& states)
{
    set<int> result;

    text = removeSpaces(text);

    if (text == "-")
    {
        return result;
    }

    string currentState;

    for (int i = 0;
         i <= (int)text.size();
         i++)
    {
        // The extra iteration at the end makes sure the
        // last state is also processed.
        if (i == (int)text.size() || text[i] == ',')
        {
            if (!currentState.empty())
            {
                int index =
                    findStateIndex(
                        states,
                        currentState
                    );

                if (index == -1)
                {
                    cout << "\nInvalid state: "
                         << currentState
                         << endl;

                    exit(0);
                }

                result.insert(index);
                currentState.clear();
            }
        }
        else
        {
            currentState += text[i];
        }
    }

    return result;
}


// Reads an NFA from the user.
NFAMachine readNFAMachine()
{
    NFAMachine machine;

    cout << "\n========================================\n";
    cout << "              ENTER NFA\n";
    cout << "========================================\n";


    cout << "\nEnter number of states: ";

    while (!(cin >> machine.numberOfStates)
           || machine.numberOfStates <= 0)
    {
        cout << "Please enter a valid positive number: ";

        cin.clear();
        cin.ignore(10000, '\n');
    }


    cout << "Enter number of input symbols: ";

    while (!(cin >> machine.numberOfInputs)
           || machine.numberOfInputs <= 0)
    {
        cout << "Please enter a valid positive number: ";

        cin.clear();
        cin.ignore(10000, '\n');
    }


    // Read state names.
    machine.states.resize(
        machine.numberOfStates
    );

    cout << "\nEnter state names:\n";

    for (int i = 0;
         i < machine.numberOfStates;
         i++)
    {
        cin >> machine.states[i];
    }


    // Read the input alphabet.
    machine.inputs.resize(
        machine.numberOfInputs
    );

    cout << "\nEnter input symbols:\n";

    for (int i = 0;
         i < machine.numberOfInputs;
         i++)
    {
        cin >> machine.inputs[i];
    }


    // Store the initial state as an index.
    string initialState;

    cout << "\nEnter initial state: ";
    cin >> initialState;

    machine.initialState =
        findStateIndex(
            machine.states,
            initialState
        );


    if (machine.initialState == -1)
    {
        cout << "Invalid initial state.\n";
        cout << "Using the first state as the initial state.\n";

        machine.initialState = 0;
    }


    // Read final states.
    int numberOfFinalStates;

    cout << "\nEnter number of final states: ";

    while (!(cin >> numberOfFinalStates)
           || numberOfFinalStates < 0
           || numberOfFinalStates > machine.numberOfStates)
    {
        cout << "Please enter a valid number: ";

        cin.clear();
        cin.ignore(10000, '\n');
    }


    if (numberOfFinalStates > 0)
    {
        cout << "Enter final state names:\n";
    }


    for (int i = 0;
         i < numberOfFinalStates;
         i++)
    {
        string finalState;
        cin >> finalState;


        int index =
            findStateIndex(
                machine.states,
                finalState
            );


        if (index == -1)
        {
            cout << "Invalid final state: "
                 << finalState
                 << endl;

            exit(0);
        }


        machine.finalStates.insert(index);
    }


    // Each NFA transition stores a set because multiple
    // destinations are allowed.
    machine.transition.resize(
        machine.numberOfStates,
        vector<set<int>>(
            machine.numberOfInputs
        )
    );


    cout << "\n========================================\n";
    cout << "          ENTER NFA TRANSITIONS\n";
    cout << "========================================\n";

    cout << "\nFor one destination, enter: q1\n";
    cout << "For multiple destinations, enter: q0,q1\n";
    cout << "For no transition, enter: -\n";


    for (int state = 0;
         state < machine.numberOfStates;
         state++)
    {
        for (int input = 0;
             input < machine.numberOfInputs;
             input++)
        {
            string destination;


            cout << "\n"
                 << machine.states[state]
                 << " --"
                 << machine.inputs[input]
                 << "--> ";

            cin >> destination;


            machine.transition[state][input] =
                parseNFAStates(
                    destination,
                    machine.states
                );
        }
    }


    return machine;
}


// Displays the NFA transition table.
void displayNFATable(
    const NFAMachine& machine)
{
    cout << "\n\n========================================\n";
    cout << "               NFA TABLE\n";
    cout << "========================================\n\n";


    cout << left
         << setw(15)
         << "State";


    for (const string& input : machine.inputs)
    {
        cout << setw(20)
             << input;
    }


    cout << setw(15)
         << "Final"
         << endl;


    printLine(
        15 +
        machine.numberOfInputs * 20 +
        15
    );


    for (int state = 0;
         state < machine.numberOfStates;
         state++)
    {
        cout << left
             << setw(15)
             << machine.states[state];


        for (int input = 0;
             input < machine.numberOfInputs;
             input++)
        {
            if (machine.transition[state][input].empty())
            {
                cout << setw(20)
                     << "-";
            }
            else
            {
                cout << setw(20)
                     << subsetToBracketString(
                            machine.transition[state][input],
                            machine.states
                        );
            }
        }


        if (machine.finalStates.count(state))
        {
            cout << setw(15) << "Yes";
        }
        else
        {
            cout << setw(15) << "No";
        }


        cout << endl;
    }
}


// A DFA subset is final if it contains at least one
// final state of the original NFA.
bool isFinalSet(
    const set<int>& stateSet,
    const set<int>& nfaFinalStates)
{
    for (int state : stateSet)
    {
        if (nfaFinalStates.count(state))
        {
            return true;
        }
    }

    return false;
}


// Finds whether a particular subset has already been
// created as a DFA state.
int findSetIndex(
    const vector<set<int>>& dfaSets,
    const set<int>& target)
{
    for (int i = 0;
         i < (int)dfaSets.size();
         i++)
    {
        if (dfaSets[i] == target)
        {
            return i;
        }
    }

    return -1;
}


// Converts an NFA into a DFA using subset construction.
//
// The main idea is to treat a set of NFA states as one
// DFA state.
//
// For example:
//
//     {q0,q1}
//
// is one state in the resulting DFA.
DFAMachine convertNFAToDFA(
    const NFAMachine& nfa)
{
    DFAMachine dfa;

    dfa.numberOfInputs =
        nfa.numberOfInputs;

    dfa.inputs =
        nfa.inputs;

    dfa.initialState = 0;


    // Every element of dfaSets represents one DFA state.
    vector<set<int>> dfaSets;

    // New subsets are processed using BFS.
    queue<int> unprocessedStates;


    // The first DFA state is the set containing
    // the original NFA initial state.
    set<int> initialSet;

    initialSet.insert(
        nfa.initialState
    );


    dfaSets.push_back(initialSet);
    unprocessedStates.push(0);


    while (!unprocessedStates.empty())
    {
        int current =
            unprocessedStates.front();

        unprocessedStates.pop();


        // Try every input from the current subset.
        for (int input = 0;
             input < nfa.numberOfInputs;
             input++)
        {
            set<int> newSet;


            // Follow the input from every NFA state
            // contained in the current DFA state.
            for (int nfaState : dfaSets[current])
            {
                for (int destination :
                     nfa.transition[nfaState][input])
                {
                    newSet.insert(destination);
                }
            }


            // No destination means there is no transition.
            if (newSet.empty())
            {
                continue;
            }


            // If this subset has not appeared before,
            // it becomes a new DFA state.
            int existing =
                findSetIndex(
                    dfaSets,
                    newSet
                );


            if (existing == -1)
            {
                int newIndex =
                    dfaSets.size();

                dfaSets.push_back(newSet);
                unprocessedStates.push(newIndex);
            }
        }
    }


    dfa.numberOfStates =
        dfaSets.size();


    // Give each subset a readable name.
    for (const set<int>& stateSet : dfaSets)
    {
        dfa.states.push_back(
            subsetToName(
                stateSet,
                nfa.states
            )
        );
    }


    // Build the DFA transition table.
    dfa.transition.resize(
        dfa.numberOfStates,
        vector<int>(
            dfa.numberOfInputs,
            -1
        )
    );


    for (int current = 0;
         current < dfa.numberOfStates;
         current++)
    {
        for (int input = 0;
             input < dfa.numberOfInputs;
             input++)
        {
            set<int> newSet;


            for (int nfaState : dfaSets[current])
            {
                for (int destination :
                     nfa.transition[nfaState][input])
                {
                    newSet.insert(destination);
                }
            }


            if (newSet.empty())
            {
                dfa.transition[current][input] = -1;
                continue;
            }


            dfa.transition[current][input] =
                findSetIndex(
                    dfaSets,
                    newSet
                );
        }
    }


    // A DFA state is final when its subset contains
    // at least one NFA final state.
    for (int i = 0;
         i < dfa.numberOfStates;
         i++)
    {
        if (isFinalSet(
                dfaSets[i],
                nfa.finalStates))
        {
            dfa.finalStates.insert(i);
        }
    }


    // Show how the subsets were converted into DFA states.
    cout << "\n\n========================================\n";
    cout << "          SUBSET CONSTRUCTION\n";
    cout << "========================================\n\n";


    cout << left
         << setw(15)
         << "DFA State"
         << setw(25)
         << "NFA States"
         << setw(15)
         << "Final"
         << endl;


    printLine(55);


    for (int i = 0;
         i < dfa.numberOfStates;
         i++)
    {
        cout << left
             << setw(15)
             << dfa.states[i];


        cout << setw(25)
             << subsetToBracketString(
                    dfaSets[i],
                    nfa.states
                );


        if (dfa.finalStates.count(i))
        {
            cout << setw(15) << "Yes";
        }
        else
        {
            cout << setw(15) << "No";
        }


        cout << endl;
    }


    return dfa;
}


// Displays the DFA transition table.
void displayDFATable(
    const DFAMachine& machine)
{
    cout << "\n\n========================================\n";
    cout << "               DFA TABLE\n";
    cout << "========================================\n\n";


    cout << left
         << setw(15)
         << "State";


    for (const string& input : machine.inputs)
    {
        cout << setw(15)
             << input;
    }


    cout << setw(15)
         << "Final"
         << endl;


    printLine(
        15 +
        machine.numberOfInputs * 15 +
        15
    );


    for (int state = 0;
         state < machine.numberOfStates;
         state++)
    {
        cout << left
             << setw(15)
             << machine.states[state];


        for (int input = 0;
             input < machine.numberOfInputs;
             input++)
        {
            int next =
                machine.transition[state][input];


            if (next == -1)
            {
                cout << setw(15) << "-";
            }
            else
            {
                cout << setw(15)
                     << machine.states[next];
            }
        }


        if (machine.finalStates.count(state))
        {
            cout << setw(15) << "Yes";
        }
        else
        {
            cout << setw(15) << "No";
        }


        cout << endl;
    }
}


// Creates a Graphviz diagram for an NFA.
void generateNFADiagram(
    const NFAMachine& machine,
    const string& filename)
{
    ofstream file(filename);


    if (!file)
    {
        cout << "Could not create "
             << filename
             << endl;

        return;
    }


    file << "digraph NFA {\n";
    file << "    rankdir=LR;\n";
    file << "    node [shape=circle];\n";


    // Final states are drawn using double circles.
    for (int state : machine.finalStates)
    {
        file << "    \""
             << machine.states[state]
             << "\" [shape=doublecircle];\n";
    }


    // Add an arrow pointing to the initial state.
    file << "    start [shape=point];\n";

    file << "    start -> \""
         << machine.states[machine.initialState]
         << "\";\n";


    /*
        If multiple inputs create the same transition,
        combine them into one Graphviz edge.

        For example:

            q0 --0--> q1
            q0 --1--> q1

        becomes:

            q0 --0,1--> q1
    */
    map<pair<int, int>, vector<string>> labels;


    for (int state = 0;
         state < machine.numberOfStates;
         state++)
    {
        for (int input = 0;
             input < machine.numberOfInputs;
             input++)
        {
            for (int destination :
                 machine.transition[state][input])
            {
                labels[
                    {state, destination}
                ].push_back(
                    machine.inputs[input]
                );
            }
        }
    }


    for (const auto& entry : labels)
    {
        int from = entry.first.first;
        int to = entry.first.second;


        string label;


        for (int i = 0;
             i < (int)entry.second.size();
             i++)
        {
            if (i > 0)
            {
                label += ",";
            }

            label += entry.second[i];
        }


        file << "    \""
             << machine.states[from]
             << "\" -> \""
             << machine.states[to]
             << "\" [label=\""
             << label
             << "\"];\n";
    }


    file << "}\n";
    file.close();


    cout << "\nGraphviz file created: "
         << filename
         << endl;
}


// Creates a Graphviz diagram for a DFA.
void generateDFADiagram(
    const DFAMachine& machine,
    const string& filename)
{
    ofstream file(filename);


    if (!file)
    {
        cout << "Could not create "
             << filename
             << endl;

        return;
    }


    file << "digraph DFA {\n";
    file << "    rankdir=LR;\n";
    file << "    node [shape=circle];\n";


    // Mark final states with double circles.
    for (int state : machine.finalStates)
    {
        file << "    \""
             << machine.states[state]
             << "\" [shape=doublecircle];\n";
    }


    file << "    start [shape=point];\n";

    file << "    start -> \""
         << machine.states[machine.initialState]
         << "\";\n";


    // Group transitions that have the same source
    // and destination.
    map<pair<int, int>, vector<string>> labels;


    for (int state = 0;
         state < machine.numberOfStates;
         state++)
    {
        for (int input = 0;
             input < machine.numberOfInputs;
             input++)
        {
            int destination =
                machine.transition[state][input];


            if (destination == -1)
            {
                continue;
            }


            labels[
                {state, destination}
            ].push_back(
                machine.inputs[input]
            );
        }
    }


    for (const auto& entry : labels)
    {
        int from = entry.first.first;
        int to = entry.first.second;


        string label;


        for (int i = 0;
             i < (int)entry.second.size();
             i++)
        {
            if (i > 0)
            {
                label += ",";
            }

            label += entry.second[i];
        }


        file << "    \""
             << machine.states[from]
             << "\" -> \""
             << machine.states[to]
             << "\" [label=\""
             << label
             << "\"];\n";
    }


    file << "}\n";
    file.close();


    cout << "\nGraphviz file created: "
         << filename
         << endl;
}


// Converts a DFA into an NFA.
//
// A DFA is already a valid NFA. The only difference is that
// an NFA transition stores a set, so each DFA destination
// simply becomes a one-element set.
NFAMachine convertDFAToNFA(
    const DFAMachine& dfa)
{
    NFAMachine nfa;


    nfa.numberOfStates =
        dfa.numberOfStates;

    nfa.numberOfInputs =
        dfa.numberOfInputs;

    nfa.states =
        dfa.states;

    nfa.inputs =
        dfa.inputs;

    nfa.initialState =
        dfa.initialState;

    nfa.finalStates =
        dfa.finalStates;


    nfa.transition.resize(
        nfa.numberOfStates,
        vector<set<int>>(
            nfa.numberOfInputs
        )
    );


    for (int state = 0;
         state < dfa.numberOfStates;
         state++)
    {
        for (int input = 0;
             input < dfa.numberOfInputs;
             input++)
        {
            int next =
                dfa.transition[state][input];


            if (next != -1)
            {
                nfa.transition[state][input].insert(next);
            }
        }
    }


    return nfa;
}


// Reads a DFA from the user.
DFAMachine readDFAMachine()
{
    DFAMachine machine;


    cout << "\n========================================\n";
    cout << "              ENTER DFA\n";
    cout << "========================================\n";


    cout << "\nEnter number of states: ";

    while (!(cin >> machine.numberOfStates)
           || machine.numberOfStates <= 0)
    {
        cout << "Please enter a valid positive number: ";

        cin.clear();
        cin.ignore(10000, '\n');
    }


    cout << "Enter number of input symbols: ";

    while (!(cin >> machine.numberOfInputs)
           || machine.numberOfInputs <= 0)
    {
        cout << "Please enter a valid positive number: ";

        cin.clear();
        cin.ignore(10000, '\n');
    }


    // Read state names.
    machine.states.resize(
        machine.numberOfStates
    );

    cout << "\nEnter state names:\n";

    for (int i = 0;
         i < machine.numberOfStates;
         i++)
    {
        cin >> machine.states[i];
    }


    // Read input symbols.
    machine.inputs.resize(
        machine.numberOfInputs
    );

    cout << "\nEnter input symbols:\n";

    for (int i = 0;
         i < machine.numberOfInputs;
         i++)
    {
        cin >> machine.inputs[i];
    }


    // Read the initial state.
    string initialState;

    cout << "\nEnter initial state: ";
    cin >> initialState;


    machine.initialState =
        findStateIndex(
            machine.states,
            initialState
        );


    if (machine.initialState == -1)
    {
        cout << "Invalid initial state.\n";
        cout << "Using the first state as the initial state.\n";

        machine.initialState = 0;
    }


    // Read final states.
    int numberOfFinalStates;

    cout << "\nEnter number of final states: ";

    while (!(cin >> numberOfFinalStates)
           || numberOfFinalStates < 0
           || numberOfFinalStates > machine.numberOfStates)
    {
        cout << "Please enter a valid number: ";

        cin.clear();
        cin.ignore(10000, '\n');
    }


    if (numberOfFinalStates > 0)
    {
        cout << "Enter final state names:\n";
    }


    for (int i = 0;
         i < numberOfFinalStates;
         i++)
    {
        string finalState;

        cin >> finalState;


        int index =
            findStateIndex(
                machine.states,
                finalState
            );


        if (index == -1)
        {
            cout << "Invalid final state.\n";
            exit(0);
        }


        machine.finalStates.insert(index);
    }


    // Create the DFA transition table.
    machine.transition.resize(
        machine.numberOfStates,
        vector<int>(
            machine.numberOfInputs,
            -1
        )
    );


    cout << "\n========================================\n";
    cout << "          ENTER DFA TRANSITIONS\n";
    cout << "========================================\n";


    for (int state = 0;
         state < machine.numberOfStates;
         state++)
    {
        for (int input = 0;
             input < machine.numberOfInputs;
             input++)
        {
            string destination;


            cout << "\n"
                 << machine.states[state]
                 << " --"
                 << machine.inputs[input]
                 << "--> ";

            cin >> destination;


            if (destination == "-")
            {
                machine.transition[state][input] = -1;
                continue;
            }


            int index =
                findStateIndex(
                    machine.states,
                    destination
                );


            if (index == -1)
            {
                cout << "Invalid destination: "
                     << destination
                     << endl;

                exit(0);
            }


            machine.transition[state][input] = index;
        }
    }


    return machine;
}


// ============================================================
// MAIN
// ============================================================

int main()
{
    int choice = 0;


    while (true)
    {
        cout << "\n\n";
        cout << "============================================\n";
        cout << "             NFA <-> DFA CONVERTER\n";
        cout << "============================================\n\n";

        cout << "1. NFA Machine -> DFA Machine\n";
        cout << "2. DFA Machine -> NFA Machine\n";
        cout << "3. Exit\n";


        cout << "\nEnter your choice: ";


        // Handle non-numeric input instead of allowing
        // the program to get stuck in the loop.
        if (!(cin >> choice))
        {
            cout << "\nInvalid input.\n";

            cin.clear();
            cin.ignore(10000, '\n');

            continue;
        }


        // Exit the program.
        if (choice == 3)
        {
            cout << "\n========================================\n";
            cout << "             PROGRAM ENDED\n";
            cout << "========================================\n";

            break;
        }


        // ----------------------------------------------------
        // NFA -> DFA
        // ----------------------------------------------------

        if (choice == 1)
        {
            NFAMachine nfa =
                readNFAMachine();


            displayNFATable(nfa);


            cout << "\n\n========================================\n";
            cout << "           CONVERTING NFA -> DFA\n";
            cout << "========================================\n";


            DFAMachine dfa =
                convertNFAToDFA(nfa);


            cout << "\n\n========================================\n";
            cout << "             CONVERTED DFA\n";
            cout << "========================================\n";


            displayDFATable(dfa);


            // Create diagrams for both machines.
            generateNFADiagram(
                nfa,
                "nfa_original.dot"
            );

            generateDFADiagram(
                dfa,
                "nfa_to_dfa.dot"
            );


            // If Graphviz is installed, convert the dot files
            // into PNG images.
            int result1 =
                system(
                    "dot -Tpng nfa_original.dot "
                    "-o nfa_original.png"
                );


            int result2 =
                system(
                    "dot -Tpng nfa_to_dfa.dot "
                    "-o nfa_to_dfa.png"
                );


            if (result1 == 0)
            {
                cout << "\nNFA diagram created: "
                     << "nfa_original.png"
                     << endl;
            }
            else
            {
                cout << "\nGraphviz was not found.\n";
                cout << "The .dot files were still created.\n";
            }


            if (result2 == 0)
            {
                cout << "DFA diagram created: "
                     << "nfa_to_dfa.png"
                     << endl;
            }
        }


        // ----------------------------------------------------
        // DFA -> NFA
        // ----------------------------------------------------

        else if (choice == 2)
        {
            DFAMachine dfa =
                readDFAMachine();


            displayDFATable(dfa);


            cout << "\n\n========================================\n";
            cout << "           CONVERTING DFA -> NFA\n";
            cout << "========================================\n";


            NFAMachine nfa =
                convertDFAToNFA(dfa);


            cout << "\n\n========================================\n";
            cout << "             CONVERTED NFA\n";
            cout << "========================================\n";


            displayNFATable(nfa);


            generateDFADiagram(
                dfa,
                "dfa_original.dot"
            );

            generateNFADiagram(
                nfa,
                "dfa_to_nfa.dot"
            );


            int result1 =
                system(
                    "dot -Tpng dfa_original.dot "
                    "-o dfa_original.png"
                );


            int result2 =
                system(
                    "dot -Tpng dfa_to_nfa.dot "
                    "-o dfa_to_nfa.png"
                );


            if (result1 == 0)
            {
                cout << "\nDFA diagram created: "
                     << "dfa_original.png"
                     << endl;
            }
            else
            {
                cout << "\nGraphviz was not found.\n";
                cout << "The .dot files were still created.\n";
            }


            if (result2 == 0)
            {
                cout << "NFA diagram created: "
                     << "dfa_to_nfa.png"
                     << endl;
            }
        }


        else
        {
            cout << "\nInvalid choice!\n";
            cout << "Please enter 1, 2 or 3.\n";
        }
    }


    return 0;
}
