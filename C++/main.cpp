// Created by Delphine Somerhausen (delsomer@gmail.com)

#include <iostream>
#include<fstream>
#include <vector>

using namespace std;

#include "ee.h"
#include "Wiener.h"

int main() {
    string input = "Path\\to\\input\\file.txt";
    string output = "Path\\to\\output\\file.txt";
    vector<double> data;
    double x;
    // Read the input file
    ifstream inFile;
    inFile.open(input);
    if (!inFile) {
        cout << "Unable to open file";
        exit(1); // terminate with error
    }
    while (inFile >> x) {
        data.push_back(x);
    }
    inFile.close();
    // Declarations
    long l = (data.size()-1);
    int buf = 1000;
    int n = l/buf;
    double processed[buf];
    double intermediate[buf];
    double toProcess[buf];
    Wiener processor;
    ee dothething;
    // Open output file and process the input by buffer of size buff
    ofstream outFile;
    outFile.open(output);
    if (!outFile) {
        cout << "Unable to open file";
        exit(1); // terminate with error
    }
    outFile << data[0];
    outFile << "\n";
    for (int part = 0; part < n; part++) {
        for (int i = 0; i < buf; i++) {
            toProcess[i] = data[1 + i + part * buf];
        }
        processor.process(toProcess, buf, intermediate);
        dothething.process(intermediate, 0.001, buf, processed);
        for (int i = 0 ; i < buf; i++){
            outFile << processed[i];
            outFile << "\n";
        }
    }

    cout << "Finished processing";
    outFile.close();

    return 0;
}

