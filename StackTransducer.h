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
    };

    struct TransitionEffect {
        int state;
        std::string stackString, outputString;
    };

    struct InstantaneousDescription {
        int state;
        std::string currentInput, currentStack, currentOutput;
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
    static const char INPUT_LAMBDA_SYMBOL = '_';
    static const char STACK_LAMBDA_SYMBOL = '_';
    static const char OUTPUT_LAMBDA_SYMBOL = '_';

private:
    Acceptance accept;
    std::map< TransitionCondition, std::vector<TransitionEffect> > delta;
    std::vector<int> finalStates;

    std::string removeSubstringsFromString(std::string, std::string);
    void expandCurrentQueueElement(const QueueElement&, std::queue<QueueElement>&, bool, bool);
public:
    StackTransducer(std::ifstream& in);
    void runInput(const std::string& input, bool verbose, std::ofstream& out);
};