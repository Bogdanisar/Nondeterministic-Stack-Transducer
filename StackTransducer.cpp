#include "StackTransducer.h"

bool operator<(StackTransducer::TransitionCondition a, StackTransducer::TransitionCondition b) {
    auto t1 = std::tie(a.state, a.inputChar, a.stackChar);
    auto t2 = std::tie(b.state, b.inputChar, b.stackChar);
    return t1 < t2;
}

std::ostream& operator<< (std::ostream& out, const StackTransducer::TransitionCondition& tc) {
    out << "TC: " << tc.state << ", " << tc.inputChar << ", " << tc.stackChar;
    return out;
}

std::ostream& operator<< (std::ostream& out, const StackTransducer::TransitionEffect& te) {
    out << "TE: " << te.state << ", " << te.stackString << ", " << te.outputString;
    return out;
}

std::ostream& operator<< (std::ostream& out, const StackTransducer::InstantaneousDescription& id) {
    std::string input  = StackTransducer::emptyStringToLambdaSymbol(id.currentInput);
    std::string stack  = StackTransducer::emptyStringToLambdaSymbol(id.currentStack);
    std::string output = StackTransducer::emptyStringToLambdaSymbol(id.currentOutput);
    out << "(" << id.state << ", " << input << ", " << stack << ", " << output << ")";
    return out;
}


std::string StackTransducer::removeSubstringsFromString(std::string str, std::string pattern) {
    using namespace std;

    if (pattern != "") {
        for (string::size_type i = str.find(pattern); i != string::npos; i = str.find(pattern)) {
            str.erase(i, pattern.length());
        }
    }

    return str;
}

std::string StackTransducer::emptyStringToLambdaSymbol(std::string str, char lambdaSymbol) {
    return (str.length() > 0) ? str : std::string(1, lambdaSymbol);
}


StackTransducer::StackTransducer(std::istream& in) {
    using namespace std;

    // read acceptance
    {
        string line;
        getline(in, line);
        char aux;
        sscanf(line.c_str(), "Acceptance (F/S): %c", &aux);

        if (aux == 'F') {
            this->acceptance = Acceptance::FINAL_STATE;
        }
        else if (aux == 'S') {
            this->acceptance = Acceptance::EMPTY_STACK;
        }
        else {
            throw TransducerException("Acceptance must be F or S in the input file. Input Format: Acceptance (F/S): %c");
        }
    }

    // read transition function
    {
        int N;
        in >> N;

        while (N--) {
            TransitionCondition cond;
            string arrow;
            TransitionEffect effect;
            in >> cond.state >> cond.inputChar >> cond.stackChar;
            in >> arrow;
            in >> effect.state >> effect.stackString >> effect.outputString;

            effect.stackString = StackTransducer::removeSubstringsFromString(effect.stackString, string(1, LAMBDA_SYMBOL));
            effect.outputString = StackTransducer::removeSubstringsFromString(effect.outputString, string(1, LAMBDA_SYMBOL));

            this->delta[cond].push_back(effect);
        }
    }

    // read final states
    if (this->acceptance == Acceptance::FINAL_STATE) {
        int N;
        in >> N;
        while (N--) {
            int state;
            in >> state;
            this->finalStates.insert(state);
        }
    }
}






bool StackTransducer::canAcceptWithFinalState(const InstantaneousDescription& id) {
    return
        id.currentInput.length() == 0               &&
        this->acceptance == Acceptance::FINAL_STATE &&
        this->finalStates.count(id.state) == 1;
}

bool StackTransducer::canAcceptWithEmptyStack(const InstantaneousDescription& id) {
    return
        id.currentInput.length() == 0               &&
        this->acceptance == Acceptance::EMPTY_STACK &&
        id.currentStack.length() == 0;
}


bool StackTransducer::canAcceptState(const InstantaneousDescription& id) {
    return this->canAcceptWithFinalState(id) || this->canAcceptWithEmptyStack(id);
}

// this method performs all available one-transition steps from the current instantaneous description of the queue element
// and pushes the new resulting instantaneous descriptions to the queue (as queue elements), returning the number of new states;
int StackTransducer::expandCurrentQueueElement(const QueueElement& currentElement, std::queue<QueueElement>& Q, bool byLambda, bool verbose) {
    using namespace std;

    InstantaneousDescription currId = currentElement.id;

    bool cantProgressDueToEmptyStack = currId.currentStack.length() == 0;
    bool noMoreInputCharacters = currId.currentInput.length() == 0 && byLambda == false;
    if (cantProgressDueToEmptyStack || noMoreInputCharacters) {
        return 0;
    }

    TransitionCondition tc;
    tc.state = currId.state;
    tc.inputChar = (byLambda) ? LAMBDA_SYMBOL : currId.currentInput[0];
    tc.stackChar = currId.currentStack.back();

    int numNewElements = 0;
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
        numNewElements += 1;
    }

    return numNewElements;
}


void StackTransducer::writeResult(std::string input, QueueElement qe, int transitionsTaken, bool verbose, std::ostream& out) {
    out << "Translation with " << transitionsTaken << " transitions:\n";

    input = StackTransducer::emptyStringToLambdaSymbol(input);
    out << input << '\n';

    std::string output = qe.id.currentOutput;
    output = StackTransducer::emptyStringToLambdaSymbol(output);
    out << output << '\n';

    if (verbose) {
        out << "Instantaneous Descriptions:\n";
        for (auto id : qe.previousStates) {
            out << id << '\n';
        }
        out << qe.id << '\n';
    }

    out << '\n';
    out.flush();
}

void StackTransducer::runInput(std::string input, bool verbose, std::ostream& out, int maxResults) {
    using namespace std;

    input = StackTransducer::removeSubstringsFromString(input, string(1, LAMBDA_SYMBOL));

    queue<QueueElement> Q;
    string initialStack = string(1, StackTransducer::STACK_BASE_SYMBOL);
    InstantaneousDescription id = {0, input, initialStack, ""};
    Q.push({id, {}});

    int numTrasitionsSoFar = 0;
    int currStatesWithNumTransitions = 1;
    int currStatesWithNextNumTransitions = 0;
    int numResults = 0;

    while (Q.size()) {
        QueueElement currentElement = Q.front();
        Q.pop();
        InstantaneousDescription currId = currentElement.id;

        if (this->canAcceptState(currId)) {
            this->writeResult(input, currentElement, numTrasitionsSoFar, verbose, out);

            numResults += 1;
            if (numResults == maxResults) {
                out << "The transducer stopped after printing " << maxResults << " results\n";
                out.flush();
                return;
            }
        }

        currStatesWithNextNumTransitions += expandCurrentQueueElement(currentElement, Q, false, verbose);
        currStatesWithNextNumTransitions += expandCurrentQueueElement(currentElement, Q, true, verbose);

        currStatesWithNumTransitions -= 1;
        if (currStatesWithNumTransitions == 0) {
            // we have finished a level of the BFS

            // increase the number of steps taken for all states in the current level
            numTrasitionsSoFar += 1;

            // and make the number of elements on the current level be equal to the one on the next level
            currStatesWithNumTransitions = currStatesWithNextNumTransitions;
            currStatesWithNextNumTransitions = 0;
        }
    }

    if (numResults == 0) {
        out << "The input string is not accepted by the transducer\n";
        out.flush();
    }
    else {
        out << "The transducer found " << numResults << " results";
        out.flush();
    }
}
