#include <bits/stdc++.h>

#define pv(x) std::cerr<<#x<<" = "<<(x)<<"; ";std::cerr.flush()
#define pn std::cerr<<std::endl

class StackTransducer {
public:
    enum class Acceptance {
        FINAL_STATE, EMPTY_STACK
    };

    // this represents an element of the domain of the delta function
    struct TransitionCondition {
        int state;
        char inputChar, stackChar;

        friend bool operator<(TransitionCondition a, TransitionCondition b);

        // for debugging purposes
        friend std::ostream& operator<< (std::ostream& out, const TransitionCondition& tc);
    };

    // this is part of the codomain of the delta function
    struct TransitionEffect {
        int state;
        std::string stackString, outputString;

        // for debugging purposes
        friend std::ostream& operator<< (std::ostream& out, const TransitionEffect& te);
    };

    struct InstantaneousDescription {
        int state;
        std::string currentInput, currentStack, currentOutput;

        friend std::ostream& operator<< (std::ostream& out, const InstantaneousDescription& id);
    };

    class TransducerException: public std::exception {
        const char * message;

        public:
        TransducerException(const char * message) {
            this->message = message;
        }

        virtual const char* what() const throw()
        {
            return this->message;
        }
    };


    static const char STACK_BASE_SYMBOL = '/';
    static const char LAMBDA_SYMBOL = '_';

private:
    // used in the runInput() method
    struct QueueElement {
        StackTransducer::InstantaneousDescription id;

        // will only be filled with values if we are running in verbose mode;
        std::vector< StackTransducer::InstantaneousDescription > previousStates;
    };

    Acceptance acceptance;
    std::map< TransitionCondition, std::vector<TransitionEffect> > delta;
    std::unordered_set<int> finalStates;

    int expandCurrentQueueElement(const QueueElement&, std::queue<QueueElement>&, bool, bool);
    void writeResult(std::string input, QueueElement qe, int transitionsTaken, bool verbose, std::ostream& out);
public:
    // create the Transducer from a description read from an input stream;
    // see transducer.in and transducerFormat.txt for the format of the input.
    StackTransducer(std::istream& in);

    // returns true if the Transducer runs in a "final state" mode and
    // it can accept the input at the given Instantaneous Description
    bool canAcceptWithFinalState(const InstantaneousDescription& id);

    // returns true if the Transducer runs in an "empty stack" mode and
    // it can accept the input at the given Instantaneous Description
    bool canAcceptWithEmptyStack(const InstantaneousDescription& id);

    // returns true if the Transducer can accept the input at the given Instantaneous Description
    bool canAcceptState(const InstantaneousDescription& id);

    // runs the Transducer on the input string and prints results to the output stream as soon as they are found
    // if verbose is 'true', then the Transducer will also print all intermediary Instantaneous Descriptions
    void runInput(std::string input, bool verbose, std::ostream& out);

    // removes all appearances of "pattern" from "str"
    static std::string removeSubstringsFromString(std::string str, std::string pattern);

    // If the string is empty (""), this returns a string with a lambda symbol ("_")
    // otherwise, it returns the string unchanged
    static std::string emptyStringToLambdaSymbol(std::string, char lambdaSymbol = StackTransducer::LAMBDA_SYMBOL);
};