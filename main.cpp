#include <bits/stdc++.h>
#include "StackTransducer.h"

using namespace std;

#define pv(x) std::cerr<<#x<<" = "<<(x)<<"; ";std::cerr.flush()
#define pn std::cerr<<std::endl

// compile:
// g++ -Wextra -Wall main.cpp StackTransducer.cpp -o main.exe

int main() {
    ifstream in;
    in.open("transducer.in");
    StackTransducer transducer(in);
    in.close();

    const char *inputFileName = "data.in";
    FILE *fin = fopen("data.in", "r");
    if (fin == NULL) {
        cout << "There was a problem opening the file " << inputFileName;
        return -1;
    }

    int verbose;
    char inputString[1000] = {};
    fscanf(fin, "Input: %s\n", inputString);
    fscanf(fin, "Verbose (0/1): %i\n", &verbose);
    fclose(fin);

    ofstream out("results.out");
    transducer.runInput(inputString, verbose, out);
    out.close();

    return 0;
}