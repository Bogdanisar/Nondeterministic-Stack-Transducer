#include <bits/stdc++.h>

#define pv(x) std::cerr<<#x<<" = "<<(x)<<"; ";std::cerr.flush()
#define pn std::cerr<<std::endl

class StackTranslator {
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

    class TranslatorException: public std::exception {
        const char * message;

        public:
        TranslatorException(const char * message) {
            this->message = message;
        }

        virtual const char* what() const throw()
        {
            return this->message;
        }
    };
    
    struct QueueElement {
        StackTranslator::InstantaneousDescription id;

        // will only be filled with values if we are running in debug mode;
        std::vector< StackTranslator::InstantaneousDescription > previousStates;
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
    StackTranslator(std::ifstream& in);
    void runInput(const std::string& input, bool debug, std::ofstream& out);
};