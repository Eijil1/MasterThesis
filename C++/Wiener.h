// Created by Delphine Somerhausen (delsomer@gmail.com)

#ifndef ENVENH_WIENER_H
#define ENVENH_WIENER_H

#include <complex>
#include <vector>

using namespace std;

typedef complex<double> Complex;

class Wiener {

    vector<double> mean;
    vector<double> power;
    vector<double> G;
    vector<double> Gamma;

    int noiseCounter = 0;
    bool speechFlag = false;
    int count = 0;

    void vad(Complex * y, int buf);

public:

    void process(double *signal, int buf, double *output);

};


#endif //ENVENH_WIENER_H
