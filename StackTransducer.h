#include <bits/stdc++.h>

#define pv(x) std::cerr<<#x<<" = "<<(x)<<"; ";std::cerr.flush()
#define pn std::cerr<<std::endl

class StackTransducer {
public:
    enum class Acceptance {
        FINAL_STATE, EMPTY_STACK
    };

    struct TransitionCondition {
        int state;
        char inputChar, stackChar;

        friend bool operator<(TransitionCondition a, TransitionCondition b);
        // for debugging purposes
        friend std::ostream& operator<< (std::ostream& out, const TransitionCondition& tc);
    };

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
    
    // used in the runInput() method
    struct QueueElement {
        StackTransducer::InstantaneousDescription id;

        // will only be filled with values if we are running in verbose mode;
        std::vector< StackTransducer::InstantaneousDescription > previousStates;
    };
    

    static const char STACK_BASE_SYMBOL = '/';
    static const char LAMBDA_SYMBOL = '_';

private:
    Acceptance acceptance;
    std::map< TransitionCondition, std::vector<TransitionEffect> > delta;
    std::unordered_set<int> finalStates;

    int expandCurrentQueueElement(const QueueElement&, std::queue<QueueElement>&, bool, bool);
public:
    StackTransducer(std::istream& in);
    bool canAcceptWithFinalState(const InstantaneousDescription& id);
    bool canAcceptWithEmptyStack(const InstantaneousDescription& id);
    bool canAcceptState(const InstantaneousDescription& id);
    void writeResult(std::string input, QueueElement qe, int transitionsTaken, bool verbose, std::ostream& out);
    void runInput(std::string input, bool verbose, std::ostream& out);
    
    static std::string removeSubstringsFromString(std::string, std::string);
    static std::string emptyStringToLambdaSymbol(std::string, char lambdaSymbol = StackTransducer::LAMBDA_SYMBOL);
};