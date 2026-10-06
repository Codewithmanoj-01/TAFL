#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <fstream>
#include <cstdlib>
#include <iomanip>

using namespace std;


// A Moore machine gives the output based on the current state.
// The transition table only stores where the machine moves next.
struct MooreMachine
{
    int numberOfStates;
    int numberOfInputs;

    vector<string> states;
    vector<string> inputs;

    // transition[state][input] = next state
    // -1 means that transition does not exist
    vector<vector<int>> transition;

    // Output associated with each state
    vector<string> output;

    int initialState;
};


// In a Mealy machine, the output belongs to the transition.
// So each transition needs both the destination and output.
struct MealyTransition
{
    int nextState;
    string output;
};


struct MealyMachine
{
    int numberOfStates;
    int numberOfInputs;

    vector<string> states;
    vector<string> inputs;

    // transition[state][input]
    vector<vector<MealyTransition>> transition;

    int initialState;
};


// Used while creating new states during Mealy -> Moore conversion.
// A Moore state is represented using the original state and its output.
struct MooreState
{
    string name;
    string originalState;
    string output;
};


// Prints a simple horizontal line to make the output easier to read.
void printLine(int length = 60)
{
    for (int i = 0; i < length; i++)
    {
        cout << "-";
    }

    cout << endl;
}


// Returns the position of a state in the states vector.
// Returning -1 means that the state was not found.
int findStateIndex(const vector<string>& states, const string& name)
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


// ============================================================
// READ MOORE MACHINE
// ============================================================

MooreMachine readMooreMachine()
{
    MooreMachine machine;

    cout << "\n========================================\n";
    cout << "          ENTER MOORE MACHINE\n";
    cout << "========================================\n";

    cout << "\nEnter number of states: ";
    cin >> machine.numberOfStates;

    cout << "Enter number of input symbols: ";
    cin >> machine.numberOfInputs;


    // Read the names of all states.
    machine.states.resize(machine.numberOfStates);

    cout << "\nEnter state names:\n";

    for (int i = 0; i < machine.numberOfStates; i++)
    {
        cin >> machine.states[i];
    }


    // Read the input symbols used by the machine.
    machine.inputs.resize(machine.numberOfInputs);

    cout << "\nEnter input symbols:\n";

    for (int i = 0; i < machine.numberOfInputs; i++)
    {
        cin >> machine.inputs[i];
    }


    // Store the initial state as its index in the states vector.
    string initialState;

    cout << "\nEnter initial state: ";
    cin >> initialState;

    machine.initialState =
        findStateIndex(machine.states, initialState);

    if (machine.initialState == -1)
    {
        cout << "Invalid initial state.\n";
        cout << "Using the first state as the initial state.\n";

        machine.initialState = 0;
    }


    // In a Moore machine, every state has an output.
    machine.output.resize(machine.numberOfStates);

    cout << "\nEnter output of each state:\n";

    for (int i = 0; i < machine.numberOfStates; i++)
    {
        cout << "Output of "
             << machine.states[i]
             << ": ";

        cin >> machine.output[i];
    }


    // Initially, all transitions are marked as -1.
    // The user can enter '-' when a transition is missing.
    machine.transition.resize(
        machine.numberOfStates,
        vector<int>(machine.numberOfInputs, -1)
    );

    cout << "\nEnter transitions.\n";
    cout << "Use '-' if there is no transition.\n\n";


    for (int i = 0; i < machine.numberOfStates; i++)
    {
        for (int j = 0; j < machine.numberOfInputs; j++)
        {
            string destination;

            cout << machine.states[i]
                 << " --"
                 << machine.inputs[j]
                 << "--> ";

            cin >> destination;


            // No transition for this input.
            if (destination == "-")
            {
                machine.transition[i][j] = -1;
                continue;
            }


            int destinationIndex =
                findStateIndex(machine.states, destination);


            if (destinationIndex == -1)
            {
                cout << "\nInvalid state: "
                     << destination << endl;

                cout << "Please restart the program.\n";
                exit(0);
            }


            machine.transition[i][j] = destinationIndex;
        }
    }

    return machine;
}


// ============================================================
// DISPLAY MOORE TABLE
// ============================================================

void displayMooreTable(const MooreMachine& machine)
{
    cout << "\n\n========================================\n";
    cout << "          MOORE MACHINE TABLE\n";
    cout << "========================================\n\n";


    cout << left << setw(15) << "State";

    for (const string& input : machine.inputs)
    {
        cout << setw(15) << input;
    }

    cout << setw(15) << "Output" << endl;


    printLine(
        15 +
        machine.numberOfInputs * 15 +
        15
    );


    for (int i = 0; i < machine.numberOfStates; i++)
    {
        cout << left
             << setw(15)
             << machine.states[i];


        for (int j = 0; j < machine.numberOfInputs; j++)
        {
            int nextState =
                machine.transition[i][j];


            if (nextState == -1)
            {
                cout << setw(15) << "-";
            }
            else
            {
                cout << setw(15)
                     << machine.states[nextState];
            }
        }


        cout << setw(15)
             << machine.output[i]
             << endl;
    }
}


// ============================================================
// MOORE TO MEALY
// ============================================================

MealyMachine convertMooreToMealy(const MooreMachine& moore)
{
    MealyMachine mealy;

    // Both machines use the same states and input symbols.
    mealy.numberOfStates = moore.numberOfStates;
    mealy.numberOfInputs = moore.numberOfInputs;

    mealy.states = moore.states;
    mealy.inputs = moore.inputs;

    mealy.initialState = moore.initialState;


    mealy.transition.resize(
        mealy.numberOfStates,
        vector<MealyTransition>(mealy.numberOfInputs)
    );


    /*
        The important difference between Moore and Mealy machines
        is where the output is stored.

        Moore:
            q0 --a--> q1
            output(q1) = 1

        Mealy:
            q0 --a/1--> q1

        Therefore, while converting, we take the output of the
        destination state and attach it to the transition.
    */

    for (int state = 0;
         state < moore.numberOfStates;
         state++)
    {
        for (int input = 0;
             input < moore.numberOfInputs;
             input++)
        {
            int nextState =
                moore.transition[state][input];


            // Keep missing transitions as missing.
            if (nextState == -1)
            {
                mealy.transition[state][input].nextState = -1;
                mealy.transition[state][input].output = "-";

                continue;
            }


            mealy.transition[state][input].nextState =
                nextState;

            // In a Moore machine the output belongs to the
            // state we are entering.
            mealy.transition[state][input].output =
                moore.output[nextState];
        }
    }

    return mealy;
}


// ============================================================
// DISPLAY MEALY TABLE
// ============================================================

void displayMealyTable(const MealyMachine& machine)
{
    cout << "\n\n========================================\n";
    cout << "          MEALY MACHINE TABLE\n";
    cout << "========================================\n\n";


    cout << left
         << setw(15)
         << "State";


    for (const string& input : machine.inputs)
    {
        cout << setw(20)
             << input;
    }

    cout << endl;


    printLine(
        15 +
        machine.numberOfInputs * 20
    );


    for (int i = 0;
         i < machine.numberOfStates;
         i++)
    {
        cout << left
             << setw(15)
             << machine.states[i];


        for (int j = 0;
             j < machine.numberOfInputs;
             j++)
        {
            int nextState =
                machine.transition[i][j].nextState;


            if (nextState == -1)
            {
                cout << setw(20) << "-";
                continue;
            }


            // Mealy transitions are displayed as:
            // destination/output
            string value =
                machine.states[nextState] +
                "/" +
                machine.transition[i][j].output;


            cout << setw(20)
                 << value;
        }


        cout << endl;
    }
}


// ============================================================
// CREATE MEALY GRAPHVIZ FILE
// ============================================================

void generateMealyDiagram(
    const MealyMachine& machine,
    const string& filename)
{
    ofstream file(filename);


    if (!file)
    {
        cout << "\nCould not create diagram file.\n";
        return;
    }


    file << "digraph MealyMachine {\n";
    file << "    rankdir=LR;\n";
    file << "    node [shape=circle];\n\n";


    // A small point is used to show where the machine starts.
    file << "    start [shape=point];\n";

    file << "    start -> \""
         << machine.states[machine.initialState]
         << "\";\n\n";


    // Add every defined transition to the Graphviz file.
    for (int state = 0;
         state < machine.numberOfStates;
         state++)
    {
        for (int input = 0;
             input < machine.numberOfInputs;
             input++)
        {
            int nextState =
                machine.transition[state][input].nextState;


            if (nextState == -1)
            {
                continue;
            }


            string label =
                machine.inputs[input] +
                "/" +
                machine.transition[state][input].output;


            file << "    \""
                 << machine.states[state]
                 << "\" -> \""
                 << machine.states[nextState]
                 << "\" [label=\""
                 << label
                 << "\"];\n";
        }
    }


    file << "}\n";
    file.close();


    cout << "\nGraphviz file created: "
         << filename
         << endl;
}


// ============================================================
// READ MEALY MACHINE
// ============================================================

MealyMachine readMealyMachine()
{
    MealyMachine machine;

    cout << "\n========================================\n";
    cout << "          ENTER MEALY MACHINE\n";
    cout << "========================================\n";


    cout << "\nEnter number of states: ";
    cin >> machine.numberOfStates;

    cout << "Enter number of input symbols: ";
    cin >> machine.numberOfInputs;


    // Read state names.
    machine.states.resize(machine.numberOfStates);

    cout << "\nEnter state names:\n";

    for (int i = 0;
         i < machine.numberOfStates;
         i++)
    {
        cin >> machine.states[i];
    }


    // Read input symbols.
    machine.inputs.resize(machine.numberOfInputs);

    cout << "\nEnter input symbols:\n";

    for (int i = 0;
         i < machine.numberOfInputs;
         i++)
    {
        cin >> machine.inputs[i];
    }


    // Find the initial state.
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


    machine.transition.resize(
        machine.numberOfStates,
        vector<MealyTransition>(
            machine.numberOfInputs
        )
    );


    cout << "\n========================================\n";
    cout << "Enter transitions.\n";
    cout << "Format: destination output\n";
    cout << "Example: q1 0\n";
    cout << "Use '- -' if there is no transition.\n";
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
            string output;


            cout << machine.states[state]
                 << " --"
                 << machine.inputs[input]
                 << "--> ";

            cin >> destination;


            // Read the output even when the transition is missing,
            // because the user enters two values for every transition.
            if (destination == "-")
            {
                cin >> output;

                machine.transition[state][input].nextState = -1;
                machine.transition[state][input].output = "-";

                continue;
            }


            cin >> output;


            int destinationIndex =
                findStateIndex(
                    machine.states,
                    destination
                );


            if (destinationIndex == -1)
            {
                cout << "\nInvalid state: "
                     << destination
                     << endl;

                cout << "Please restart the program.\n";
                exit(0);
            }


            machine.transition[state][input].nextState =
                destinationIndex;

            machine.transition[state][input].output =
                output;
        }
    }


    return machine;
}


// ============================================================
// MEALY TO MOORE
// ============================================================

void convertMealyToMoore(const MealyMachine& mealy)
{
    cout << "\n\n========================================\n";
    cout << "       MEALY -> MOORE CONVERSION\n";
    cout << "========================================\n";


    vector<MooreState> newStates;


    /*
        A Mealy output belongs to a transition, while a Moore
        output belongs to a state.

        If the same state can be reached with different outputs,
        we need separate Moore states.

        For example:

            q0 --a/0--> q1
            q2 --b/1--> q1

        q1 needs to become two Moore states:

            q1_0
            q1_1
    */

    for (int destination = 0;
         destination < mealy.numberOfStates;
         destination++)
    {
        set<string> outputs;


        // Find all different outputs that can lead to this state.
        for (int source = 0;
             source < mealy.numberOfStates;
             source++)
        {
            for (int input = 0;
                 input < mealy.numberOfInputs;
                 input++)
            {
                int nextState =
                    mealy.transition[source][input].nextState;


                if (nextState == -1)
                {
                    continue;
                }


                if (nextState == destination)
                {
                    outputs.insert(
                        mealy.transition[source][input].output
                    );
                }
            }
        }


        // Create one Moore state for each distinct output.
        for (const string& output : outputs)
        {
            MooreState state;

            state.originalState =
                mealy.states[destination];

            state.output = output;

            state.name =
                mealy.states[destination] +
                "_" +
                output;

            newStates.push_back(state);
        }
    }


    /*
        The initial Mealy state may not have an incoming
        transition. In that case, it would not have been created
        above, so we create an initial version with no output.
    */

    bool initialExists = false;

    for (const MooreState& state : newStates)
    {
        if (state.originalState ==
            mealy.states[mealy.initialState])
        {
            initialExists = true;
            break;
        }
    }


    if (!initialExists)
    {
        MooreState initialState;

        initialState.originalState =
            mealy.states[mealy.initialState];

        initialState.output = "-";

        initialState.name =
            mealy.states[mealy.initialState] +
            "_-";

        newStates.push_back(initialState);
    }


    cout << "\nCreated Moore states:\n\n";

    for (const MooreState& state : newStates)
    {
        cout << state.name
             << "   Output = "
             << state.output
             << endl;
    }


    // The new transition table uses the newly created Moore states.
    vector<vector<int>> newTransition(
        newStates.size(),
        vector<int>(
            mealy.numberOfInputs,
            -1
        )
    );


    for (int state = 0;
         state < (int)newStates.size();
         state++)
    {
        int originalIndex =
            findStateIndex(
                mealy.states,
                newStates[state].originalState
            );


        if (originalIndex == -1)
        {
            continue;
        }


        for (int input = 0;
             input < mealy.numberOfInputs;
             input++)
        {
            int destination =
                mealy.transition[
                    originalIndex
                ][input].nextState;


            if (destination == -1)
            {
                continue;
            }


            string transitionOutput =
                mealy.transition[
                    originalIndex
                ][input].output;


            /*
                We now need to find the Moore state that represents:

                    destination + transition output

                For example:

                    q1 + 0  -> q1_0
                    q1 + 1  -> q1_1
            */

            for (int k = 0;
                 k < (int)newStates.size();
                 k++)
            {
                if (
                    newStates[k].originalState ==
                        mealy.states[destination]
                    &&
                    newStates[k].output ==
                        transitionOutput
                )
                {
                    newTransition[state][input] = k;
                    break;
                }
            }
        }
    }


    // Display the converted Moore machine.
    cout << "\n\n========================================\n";
    cout << "       CONVERTED MOORE MACHINE\n";
    cout << "========================================\n\n";


    cout << left
         << setw(15)
         << "State";

    cout << setw(15)
         << "Output";


    for (const string& input : mealy.inputs)
    {
        cout << setw(20)
             << input;
    }

    cout << endl;


    printLine(
        30 +
        mealy.numberOfInputs * 20
    );


    for (int i = 0;
         i < (int)newStates.size();
         i++)
    {
        cout << left
             << setw(15)
             << newStates[i].name;

        cout << setw(15)
             << newStates[i].output;


        for (int j = 0;
             j < mealy.numberOfInputs;
             j++)
        {
            int nextState =
                newTransition[i][j];


            if (nextState == -1)
            {
                cout << setw(20) << "-";
            }
            else
            {
                cout << setw(20)
                     << newStates[nextState].name;
            }
        }


        cout << endl;
    }


    // Save the converted machine as a Graphviz file.
    string filename =
        "mealy_to_moore.dot";

    ofstream file(filename);


    if (!file)
    {
        cout << "\nCould not create diagram file.\n";
        return;
    }


    file << "digraph MooreMachine {\n";
    file << "    rankdir=LR;\n";
    file << "    node [shape=circle];\n\n";


    // Find which generated state represents the initial state.
    int initialMooreState = -1;

    for (int i = 0;
         i < (int)newStates.size();
         i++)
    {
        if (
            newStates[i].originalState ==
            mealy.states[mealy.initialState]
        )
        {
            initialMooreState = i;
            break;
        }
    }


    file << "    start [shape=point];\n";


    if (initialMooreState != -1)
    {
        file << "    start -> S"
             << initialMooreState
             << ";\n\n";
    }


    // Give Graphviz short internal names such as S0, S1, etc.
    // The actual Moore state name is shown as the label.
    for (int i = 0;
         i < (int)newStates.size();
         i++)
    {
        file << "    S"
             << i
             << " [label=\""
             << newStates[i].name
             << "/"
             << newStates[i].output
             << "\"];\n";
    }


    file << "\n";


    // Add all defined transitions to the diagram.
    for (int i = 0;
         i < (int)newStates.size();
         i++)
    {
        for (int j = 0;
             j < mealy.numberOfInputs;
             j++)
        {
            int nextState =
                newTransition[i][j];


            if (nextState == -1)
            {
                continue;
            }


            file << "    S"
                 << i
                 << " -> S"
                 << nextState
                 << " [label=\""
                 << mealy.inputs[j]
                 << "\"];\n";
        }
    }


    file << "}\n";
    file.close();


    cout << "\nGraphviz file created: "
         << filename
         << endl;


    // If Graphviz is installed, also create the PNG automatically.
    string command =
        "dot -Tpng " +
        filename +
        " -o mealy_to_moore.png";


    int result =
        system(command.c_str());


    if (result == 0)
    {
        cout << "Diagram created: "
             << "mealy_to_moore.png"
             << endl;
    }
    else
    {
        cout << "\nGraphviz was not found.\n";
        cout << "The .dot file was created successfully.\n";
    }
}


// ============================================================
// MAIN PROGRAM
// ============================================================

int main()
{
    int choice;


    do
    {
        cout << "\n\n";
        cout << "========================================\n";
        cout << "       MOORE <-> MEALY CONVERTER\n";
        cout << "========================================\n";

        cout << "\n1. Moore Machine -> Mealy Machine\n";
        cout << "2. Mealy Machine -> Moore Machine\n";
        cout << "3. Exit\n";


        cout << "\nEnter your choice: ";
        cin >> choice;


        // Moore to Mealy conversion
        if (choice == 1)
        {
            MooreMachine moore =
                readMooreMachine();


            displayMooreTable(moore);


            MealyMachine mealy =
                convertMooreToMealy(moore);


            cout << "\n========================================\n";
            cout << "        CONVERSION COMPLETE\n";
            cout << "========================================\n";


            displayMealyTable(mealy);


            // Create the Graphviz source file.
            generateMealyDiagram(
                mealy,
                "moore_to_mealy.dot"
            );


            // Convert the .dot file into an image if Graphviz exists.
            string command =
                "dot -Tpng moore_to_mealy.dot "
                "-o moore_to_mealy.png";


            int result =
                system(command.c_str());


            if (result == 0)
            {
                cout << "Diagram created: "
                     << "moore_to_mealy.png"
                     << endl;
            }
            else
            {
                cout << "\nGraphviz was not found.\n";
                cout << "The .dot file was created successfully.\n";
            }
        }


        // Mealy to Moore conversion
        else if (choice == 2)
        {
            MealyMachine mealy =
                readMealyMachine();


            displayMealyTable(mealy);


            convertMealyToMoore(mealy);
        }


        // Exit the program.
        else if (choice == 3)
        {
            cout << "\n========================================\n";
            cout << "           PROGRAM ENDED\n";
            cout << "========================================\n";
        }


        else
        {
            cout << "\nInvalid choice!\n";
            cout << "Please enter 1, 2 or 3.\n";
        }


    } while (choice != 3);


    return 0;
}
