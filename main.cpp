#include <bits/stdc++.h>
#include "StackTranslator.h"

using namespace std;

#define pv(x) std::cerr<<#x<<" = "<<(x)<<"; ";std::cerr.flush()
#define pn std::cerr<<std::endl

// g++ main.cpp .\StackTranslator.cpp -o main.exe; .\main.exe

int main() {
    ifstream in;
    in.open("translator.in");
    StackTranslator translator(in);
    in.close();

    const char *inputFileName = "data.in";
    FILE *fin = fopen("data.in", "r");
    if (fin == NULL) {
        cout << "There was a problem opening the file " << inputFileName;
        return -1;
    }

    int debug;
    char inputString[1000] = {};
    fscanf(fin, "Input: %s\n", inputString);
    fscanf(fin, "Debug (0/1): %i\n", &debug);
    fclose(fin);

    pv(inputString);pn;
    pv(debug);pn;

    ofstream out("data.out");
    translator.runInput(inputString, debug == 1, out);
    out.close();

    return 0;
}