# Nondeterministic Stack Transducer

This is a C++ implementation of a nondeterministic finite-state transducer ([Wikipedia](https://en.wikipedia.org/wiki/Finite-state_transducer)) that also uses a stack.
The Transducer is implemented using an OOP design in StackTransducer.h and StackTransducer.cpp. 

A StackTransducer object is created from an input stream, instead of a file, from which the transducer description is read. 
This description must give the transducer **acceptance mode**, **transitions** and **final states**.
(See **transducer.in** and **transducerFormat.txt** for an example of how this description should look).

The transducer should be run with the `runInput(std::string input, bool verbose, std::ostream& out)` method.
It will run the input and dump any output (as soon as it is found) into the output stream. 
If there's an **infinite** amount of results, then the transducer will gradually print results 
ordered by the number of steps needed to find each result and it will never stop.

If **verbose** is enabled, along with any output, the steps taken will also be printed (in the form of **instantaneous descriptions**).

### Use example
A use example can be found in main.cpp.
In order to compile this, use in a terminal:

    g++ main.cpp StackTransducer.cpp -o main.exe
    
In order to run this, use in a terminal:

    main.exe

The description used to create the transducer is the one in **transducer.in**. 

The input and verbose options used to call `runInput` are read from **data.in**.

The transducer.in and data.in files have an example of translating any a^n . b^n word to c^(2n) . Just change the input in data.in and run main.exe. 

The results can be found in **results.txt**

### Concrete StackTransducer examples
More transducer.in and data.in examples can be found in the **moreExamples.txt** file.
