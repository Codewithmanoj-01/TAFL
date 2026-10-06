#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <queue>
#include <cstdlib>

using namespace std;


// A DFA contains states, input symbols and one destination
// for every state/input combination.
struct DFAMachine
{
    int numberOfStates;
    int numberOfInputs;

    vector<string> states;
    vector<string> inputs;

    // transition[state][input] gives the destination state.
    // -1 means that the transition is not defined.
    vector<vector<int>> transition;

    // Stores the states that are accepting/final states.
    set<int> finalStates;

    int initialState;
};


// Prints a simple line to make tables easier to read.
void printLine(int length = 70)
{
    for (int i = 0; i < length; i++)
    {
        cout << "-";
    }

    cout << endl;
}


// Finds the index of a state from its name.
// This is useful because transitions are stored using indexes.
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


// Finds the group containing a particular state.
//
// During minimization, states are divided into groups.
// For example:
//
// {q0, q2} -> group 0
// {q1}     -> group 1
//
// If q2 is passed to this function, it returns 0.
int findGroup(
    const vector<set<int>>& partition,
    int state)
{
    for (int i = 0; i < (int)partition.size(); i++)
    {
        if (partition[i].count(state))
        {
            return i;
        }
    }

    return -1;
}


// Converts a group of state indexes into a readable name.
//
// Example:
// {0, 2} becomes {q0,q2}
string groupToString(
    const set<int>& group,
    const vector<string>& states)
{
    if (group.empty())
    {
        return "{}";
    }

    string result = "{";
    bool first = true;

    for (int state : group)
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


// Converts the complete partition into one readable string.
//
// Example:
//
// {q0,q2}   {q1}   {q3}
string partitionToString(
    const vector<set<int>>& partition,
    const vector<string>& states)
{
    string result;

    for (int i = 0; i < (int)partition.size(); i++)
    {
        if (i > 0)
        {
            result += "   ";
        }

        result += groupToString(
            partition[i],
            states
        );
    }

    return result;
}


// Checks whether two partitions contain the same groups
// in the same order.
bool partitionsAreSame(
    const vector<set<int>>& p1,
    const vector<set<int>>& p2)
{
    if (p1.size() != p2.size())
    {
        return false;
    }

    for (int i = 0; i < (int)p1.size(); i++)
    {
        if (p1[i] != p2[i])
        {
            return false;
        }
    }

    return true;
}


// Reads all the information needed to build a DFA.
DFAMachine readDFAMachine()
{
    DFAMachine machine;

    cout << "\n========================================\n";
    cout << "              ENTER DFA\n";
    cout << "========================================\n";


    // Read the number of states.
    cout << "\nEnter number of states: ";

    while (!(cin >> machine.numberOfStates)
           || machine.numberOfStates <= 0)
    {
        cout << "Please enter a valid positive number: ";

        cin.clear();
        cin.ignore(10000, '\n');
    }


    // Read the number of input symbols.
    cout << "Enter number of input symbols: ";

    while (!(cin >> machine.numberOfInputs)
           || machine.numberOfInputs <= 0)
    {
        cout << "Please enter a valid positive number: ";

        cin.clear();
        cin.ignore(10000, '\n');
    }


    // Store the names of the states.
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


    // Store the input symbols.
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


    // Find the initial state and store its index.
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


    // Read the final states.
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


    // Initially every transition is set to -1.
    // The user can enter '-' when a transition does not exist.
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

    cout << "\nUse '-' if there is no transition.\n";


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
                cout << "\nInvalid destination: "
                     << destination
                     << endl;

                exit(0);
            }


            machine.transition[state][input] = index;
        }
    }


    return machine;
}


// Displays the transition table of a DFA.
void displayDFATable(
    const DFAMachine& machine,
    string title = "DFA TABLE")
{
    cout << "\n\n========================================\n";
    cout << "              " << title << "\n";
    cout << "========================================\n\n";


    cout << left
         << setw(18)
         << "State";


    for (const string& input : machine.inputs)
    {
        cout << setw(18)
             << input;
    }


    cout << setw(12)
         << "Final"
         << endl;


    printLine(
        18 +
        machine.numberOfInputs * 18 +
        12
    );


    for (int state = 0;
         state < machine.numberOfStates;
         state++)
    {
        cout << left
             << setw(18)
             << machine.states[state];


        for (int input = 0;
             input < machine.numberOfInputs;
             input++)
        {
            int destination =
                machine.transition[state][input];


            if (destination == -1)
            {
                cout << setw(18) << "-";
            }
            else
            {
                cout << setw(18)
                     << machine.states[destination];
            }
        }


        if (machine.finalStates.count(state))
        {
            cout << setw(12) << "Yes";
        }
        else
        {
            cout << setw(12) << "No";
        }


        cout << endl;
    }
}


// Removes states that can never be reached from the initial state.
//
// These states do not affect the language accepted by the DFA,
// so there is no reason to keep them during minimization.
DFAMachine removeUnreachableStates(
    const DFAMachine& original)
{
    DFAMachine result;


    vector<bool> visited(
        original.numberOfStates,
        false
    );


    queue<int> q;


    // BFS starts from the initial state.
    visited[original.initialState] = true;
    q.push(original.initialState);


    while (!q.empty())
    {
        int current = q.front();
        q.pop();


        for (int input = 0;
             input < original.numberOfInputs;
             input++)
        {
            int next =
                original.transition[current][input];


            if (next == -1)
            {
                continue;
            }


            if (!visited[next])
            {
                visited[next] = true;
                q.push(next);
            }
        }
    }


    // The old state numbers may change after unreachable
    // states are removed, so keep a mapping between them.
    vector<int> oldToNew(
        original.numberOfStates,
        -1
    );


    for (int i = 0;
         i < original.numberOfStates;
         i++)
    {
        if (visited[i])
        {
            oldToNew[i] =
                result.states.size();

            result.states.push_back(
                original.states[i]
            );
        }
    }


    result.numberOfStates =
        result.states.size();

    result.numberOfInputs =
        original.numberOfInputs;

    result.inputs =
        original.inputs;


    result.initialState =
        oldToNew[original.initialState];


    // Rebuild the transition table using the new state numbers.
    result.transition.resize(
        result.numberOfStates,
        vector<int>(
            result.numberOfInputs,
            -1
        )
    );


    for (int oldState = 0;
         oldState < original.numberOfStates;
         oldState++)
    {
        if (!visited[oldState])
        {
            continue;
        }


        int newState =
            oldToNew[oldState];


        for (int input = 0;
             input < original.numberOfInputs;
             input++)
        {
            int oldDestination =
                original.transition[oldState][input];


            if (oldDestination == -1)
            {
                result.transition[newState][input] = -1;
            }
            else
            {
                result.transition[newState][input] =
                    oldToNew[oldDestination];
            }
        }
    }


    // Keep only the final states that are still reachable.
    for (int oldFinal : original.finalStates)
    {
        if (visited[oldFinal])
        {
            result.finalStates.insert(
                oldToNew[oldFinal]
            );
        }
    }


    return result;
}


// Creates the first partition.
//
// At the beginning, final and non-final states must be separated
// because they clearly do not behave the same way.
vector<set<int>> createInitialPartition(
    const DFAMachine& machine)
{
    vector<set<int>> partition;

    set<int> nonFinalStates;
    set<int> finalStates;


    for (int state = 0;
         state < machine.numberOfStates;
         state++)
    {
        if (machine.finalStates.count(state))
        {
            finalStates.insert(state);
        }
        else
        {
            nonFinalStates.insert(state);
        }
    }


    if (!nonFinalStates.empty())
    {
        partition.push_back(nonFinalStates);
    }


    if (!finalStates.empty())
    {
        partition.push_back(finalStates);
    }


    return partition;
}


// Splits the current groups based on where their transitions go.
//
// Two states can stay together only when, for every input,
// they move to the same groups of the current partition.
vector<set<int>> refinePartition(
    const DFAMachine& machine,
    const vector<set<int>>& oldPartition)
{
    vector<set<int>> newPartition;


    for (const set<int>& group : oldPartition)
    {
        /*
            Each state gets a signature.

            For example, with two inputs:

                q0 -> [0, 1]
                q2 -> [0, 1]

            Since both states have the same signature,
            they can remain in the same group.

            But if:

                q0 -> [0, 1]
                q2 -> [1, 0]

            they must be separated.
        */

        map<vector<int>, set<int>> groups;


        for (int state : group)
        {
            vector<int> signature;


            for (int input = 0;
                 input < machine.numberOfInputs;
                 input++)
            {
                int destination =
                    machine.transition[state][input];


                if (destination == -1)
                {
                    signature.push_back(-1);
                }
                else
                {
                    int destinationGroup =
                        findGroup(
                            oldPartition,
                            destination
                        );

                    signature.push_back(
                        destinationGroup
                    );
                }
            }


            groups[signature].insert(state);
        }


        // Every unique signature becomes a separate group.
        for (const auto& entry : groups)
        {
            newPartition.push_back(
                entry.second
            );
        }
    }


    return newPartition;
}


// Prints one step of the partition refinement process.
void displayPartition(
    const vector<set<int>>& partition,
    const DFAMachine& machine,
    int number)
{
    cout << "\n";

    cout << "P"
         << number
         << " = ";


    for (int i = 0;
         i < (int)partition.size();
         i++)
    {
        if (i > 0)
        {
            cout << "   ";
        }


        cout << groupToString(
            partition[i],
            machine.states
        );
    }


    cout << "\n";
}


// Performs the actual DFA minimization.
DFAMachine minimizeDFA(
    const DFAMachine& original)
{
    // First remove states that cannot be reached.
    DFAMachine machine =
        removeUnreachableStates(original);


    if (machine.numberOfStates <
        original.numberOfStates)
    {
        cout << "\n============================================\n";
        cout << "        UNREACHABLE STATES REMOVED\n";
        cout << "============================================\n";

        cout << "Original states  : "
             << original.numberOfStates
             << endl;

        cout << "Reachable states : "
             << machine.numberOfStates
             << endl;
    }


    // Start by separating final and non-final states.
    vector<set<int>> partition =
        createInitialPartition(machine);


    cout << "\n============================================\n";
    cout << "          PARTITION REFINEMENT\n";
    cout << "============================================\n";


    displayPartition(
        partition,
        machine,
        0
    );


    int iteration = 0;


    while (true)
    {
        // Try to split the current groups further.
        vector<set<int>> newPartition =
            refinePartition(
                machine,
                partition
            );


        iteration++;


        displayPartition(
            newPartition,
            machine,
            iteration
        );


        // If the partition did not change, no more states
        // can be separated.
        if (partitionsAreSame(
                partition,
                newPartition))
        {
            cout << "\nPartition is unchanged.\n";
            cout << "DFA minimization completed.\n";

            partition = newPartition;
            break;
        }


        partition = newPartition;
    }


    // Every group in the final partition becomes one state
    // in the minimized DFA.
    DFAMachine minimized;


    minimized.numberOfStates =
        partition.size();

    minimized.numberOfInputs =
        machine.numberOfInputs;

    minimized.inputs =
        machine.inputs;


    // Give each minimized state a readable name.
    for (const set<int>& group : partition)
    {
        minimized.states.push_back(
            groupToString(
                group,
                machine.states
            )
        );
    }


    // Find which new state contains the original initial state.
    minimized.initialState =
        findGroup(
            partition,
            machine.initialState
        );


    // Build the transition table of the minimized DFA.
    minimized.transition.resize(
        minimized.numberOfStates,
        vector<int>(
            minimized.numberOfInputs,
            -1
        )
    );


    for (int groupNumber = 0;
         groupNumber < (int)partition.size();
         groupNumber++)
    {
        /*
            Any state inside this group can be used as a
            representative because all states in a final
            partition group have the same behavior.
        */
        int representative =
            *partition[groupNumber].begin();


        for (int input = 0;
             input < minimized.numberOfInputs;
             input++)
        {
            int destination =
                machine.transition[
                    representative
                ][input];


            if (destination == -1)
            {
                minimized.transition[groupNumber][input] = -1;
                continue;
            }


            // Convert the old destination into the new
            // minimized-state number.
            int destinationGroup =
                findGroup(
                    partition,
                    destination
                );


            minimized.transition[groupNumber][input] =
                destinationGroup;
        }
    }


    // A minimized state is final if it contains at least
    // one final state from the original DFA.
    for (int groupNumber = 0;
         groupNumber < (int)partition.size();
         groupNumber++)
    {
        for (int state : partition[groupNumber])
        {
            if (machine.finalStates.count(state))
            {
                minimized.finalStates.insert(
                    groupNumber
                );

                break;
            }
        }
    }


    return minimized;
}


// Creates a Graphviz .dot file for the DFA.
void generateDFADiagram(
    const DFAMachine& machine,
    const string& filename)
{
    ofstream file(filename);


    if (!file)
    {
        cout << "\nCould not create diagram file.\n";
        return;
    }


    file << "digraph DFA {\n";
    file << "    rankdir=LR;\n";
    file << "    node [shape=circle];\n";


    // Final states are drawn as double circles.
    for (int state : machine.finalStates)
    {
        file << "    \""
             << machine.states[state]
             << "\" [shape=doublecircle];\n";
    }


    // Add an arrow showing the initial state.
    file << "    start [shape=point];\n";

    file << "    start -> \""
         << machine.states[machine.initialState]
         << "\";\n";


    /*
        If two inputs produce the same transition, combine them.

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


    // Write the grouped transitions to the dot file.
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


// ============================================================
// MAIN
// ============================================================

int main()
{
    int choice;


    while (true)
    {
        cout << "\n\n";
        cout << "========================================\n";
        cout << "          DFA MINIMIZATION\n";
        cout << "========================================\n";

        cout << "\n1. Minimize DFA\n";
        cout << "2. Exit\n";


        cout << "\nEnter your choice: ";


        // Handle cases where the user enters something
        // other than a number.
        if (!(cin >> choice))
        {
            cout << "\nInvalid input.\n";

            cin.clear();
            cin.ignore(10000, '\n');

            continue;
        }


        if (choice == 2)
        {
            cout << "\n========================================\n";
            cout << "             PROGRAM ENDED\n";
            cout << "========================================\n";

            break;
        }


        if (choice == 1)
        {
            // Read the DFA from the user.
            DFAMachine original =
                readDFAMachine();


            // Show the DFA before minimization.
            displayDFATable(
                original,
                "ORIGINAL DFA TABLE"
            );


            // Create a diagram of the original DFA.
            generateDFADiagram(
                original,
                "dfa_original.dot"
            );


            int originalPNG =
                system(
                    "dot -Tpng dfa_original.dot "
                    "-o dfa_original.png"
                );


            if (originalPNG == 0)
            {
                cout << "Original DFA PNG created: "
                     << "dfa_original.png"
                     << endl;
            }
            else
            {
                cout << "Graphviz was not found.\n";
                cout << "The .dot file was still created.\n";
            }


            // Perform minimization.
            DFAMachine minimized =
                minimizeDFA(original);


            // Display the resulting minimized DFA.
            displayDFATable(
                minimized,
                "MINIMIZED DFA TABLE"
            );


            cout << "\n========================================\n";
            cout << "                RESULT\n";
            cout << "========================================\n";


            cout << "Original DFA states  : "
                 << original.numberOfStates
                 << endl;


            cout << "Minimized DFA states : "
                 << minimized.numberOfStates
                 << endl;


            if (minimized.numberOfStates <
                original.numberOfStates)
            {
                cout << "\nStates successfully merged.\n";
            }
            else
            {
                cout << "\nNo states could be merged.\n";
            }


            // Create a diagram of the minimized DFA.
            generateDFADiagram(
                minimized,
                "dfa_minimized.dot"
            );


            int minimizedPNG =
                system(
                    "dot -Tpng dfa_minimized.dot "
                    "-o dfa_minimized.png"
                );


            if (minimizedPNG == 0)
            {
                cout << "Minimized DFA PNG created: "
                     << "dfa_minimized.png"
                     << endl;
            }
            else
            {
                cout << "Graphviz was not found.\n";
                cout << "The .dot file was still created.\n";
            }
        }
        else
        {
            cout << "\nInvalid choice!\n";
            cout << "Please enter 1 or 2.\n";
        }
    }


    return 0;
}
