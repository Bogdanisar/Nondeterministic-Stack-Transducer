#include "StackTranslator.h"

bool operator<(StackTranslator::TransitionCondition a, StackTranslator::TransitionCondition b) {
    auto t1 = std::tie(a.state, a.inputChar, a.stackChar);
    auto t2 = std::tie(b.state, b.inputChar, b.stackChar);
    return t1 < t2;
}


std::string StackTranslator::removeSubstringsFromString(std::string str, std::string pattern) {
    using namespace std;

    if (pattern != "") {
        for (string::size_type i = str.find(pattern); i != string::npos; i = str.find(pattern)) {
            str.erase(i, pattern.length());
        }
    }

    return str;
}


StackTranslator::StackTranslator(std::ifstream& in) {
    using namespace std;

    // read acceptance
    {
        string line;
        getline(in, line);
        char aux;
        sscanf(line.c_str(), "Acceptance (F/S): %c", &aux);
        
        if (aux == 'F') {
            this->accept = Acceptance::FINAL_STATE;           
        }
        else if (aux == 'S') {
            this->accept = Acceptance::EMPTY_STACK;
        }
        else {
            throw TranslatorException("Acceptance must be F or S in the input file. Input Format: Acceptance (F/S): %c");
        }
    }

    // read transition function
    {
        int N;
        in >> N;
        string line;
        
        while (N--) {
            getline(in, line);

            TransitionCondition cond;
            string arrow;
            TransitionEffect effect;
            in >> cond.state >> cond.inputChar >> cond.stackChar;
            in >> arrow;
            in >> effect.state >> effect.stackString >> effect.outputString;

            effect.stackString = removeSubstringsFromString(effect.stackString, string(1, STACK_LAMBDA_SYMBOL));
            effect.outputString = removeSubstringsFromString(effect.outputString, string(1, OUTPUT_LAMBDA_SYMBOL));

            this->delta[cond].push_back(effect);

            // pv(cond.state);pv(cond.inputChar);pv(cond.stackChar);pn;
        }
    }

    // read final states
    {
        int N;
        in >> N;
        while (N--) {
            int state;
            in >> state;
            this->finalStates.push_back(state);
        }
    }
}

void StackTranslator::expandCurrentQueueElement(const QueueElement& currentElement, std::queue<QueueElement>& Q, bool byLambda, bool verbose) {
    using namespace std;

    InstantaneousDescription currId = currentElement.id;

    bool cantProgressDueToEmptyStack = currId.currentStack.length() == 0;
    bool noMoreInputCharacters = currId.currentInput.length() == 0 && byLambda == false;
    if (cantProgressDueToEmptyStack || noMoreInputCharacters) {
        return;
    }

    TransitionCondition tc;
    tc.state = currId.state;
    tc.inputChar = (byLambda) ? INPUT_LAMBDA_SYMBOL : currId.currentInput[0];
    tc.stackChar = currId.currentStack.back();

    for (TransitionEffect effect : this->delta[tc]) {
        InstantaneousDescription newId;

        newId.state = effect.state;
        newId.currentInput = (byLambda) ? currId.currentInput : currId.currentInput.substr(1);

        string newStack = currId.currentStack;
        newStack.pop_back();
        newStack += effect.stackString;
        newId.currentStack = newStack;

        newId.currentOutput = currId.currentOutput + effect.outputString;

        QueueElement newElem;
        newElem.id = newId;
        if (verbose) {
            newElem.previousStates = currentElement.previousStates;
            newElem.previousStates.push_back( currId );
        }

        Q.push(newElem);
    }
}


void StackTranslator::runInput(const std::string& input, bool debug, std::ofstream& out) {
    using namespace std;


    queue<QueueElement> Q;
    string initialStack = string(1, StackTranslator::STACK_BASE_SYMBOL );
    InstantaneousDescription id = {0, input, initialStack, ""};
    Q.push({id, {}});

    while (Q.size()) {
        QueueElement currentElement = Q.front();
        Q.pop();
        InstantaneousDescription currId = currentElement.id;

        // check is final

        expandCurrentQueueElement(currentElement, Q, false, debug);
        expandCurrentQueueElement(currentElement, Q, true, debug);
    }
}
