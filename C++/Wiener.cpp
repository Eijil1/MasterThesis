// Created by Delphine Somerhausen (delsomer@gmail.com)

#include "Wiener.h"
#include <iostream>
#include <fftw3.h>

using namespace std;

void Wiener::process(double* signal, int buf, double *output){
/**
 * Apply noise reduction to the signal
 * @param signal
 * @param buf : the size of the given signal
 * @param output : where to write the output
 */
    // Init of variables
    float IS = 0.25;
    int NIS = floor(IS * 48000/buf);
    float alpha = 0.99;
    int noiseLength = 9;
    if(buf > mean.size()){
        cout<<"Resize"<<endl;
        mean.resize(buf);
        power.resize(buf);
        G.resize(buf);
        Gamma.resize(buf);
        for (int i = 0; i < buf-mean.size(); i++){
            mean.push_back(0);
            power.push_back(0);
            G.push_back(1);
            Gamma.push_back(1);
        }
    }
    //Start process
    double Yphase[buf];
    Complex Y[buf];
    fftw_plan p = fftw_plan_dft_r2c_1d(buf, signal, reinterpret_cast<fftw_complex*>(Y), FFTW_ESTIMATE);
    fftw_execute(p);
    for (int i = 0; i < buf; i++){
        if (i<buf/2)  Y[buf-i] = conj(Y[i]);
        Yphase[i] = arg(Y[i]);
        Y[i] = abs(Y[i]);
    }
    //Frequency domain
    if (count < NIS){
        for (int i = 0; i < buf; i++){
            mean[i] = (mean[i] * count + real(Y[i]))/(count+1);
            power[i] = (power[i] * count + real(Y[i] * Y[i]))/(count+1);
        }
        speechFlag = false;
        noiseCounter = 100;
    }
    else{
        vad(Y, buf);
    }
    if (!speechFlag){
        for (int i = 0; i < buf; i++){
            mean[i] = (noiseLength * mean[i] + real(Y[i]))/(noiseLength + 1);
            power[i] = (noiseLength * power[i] + real(Y[i] * Y[i]))/(noiseLength + 1);
        }
    }
    //New values
    double gammaNew;
    double xi;
    for (int i = 0; i < buf; i++){
        gammaNew = real(Y[i]*Y[i])/power[i];
        xi = alpha * G[i]*G[i] * Gamma[i] + (1-alpha) * max(gammaNew-1, 0.0);
        Gamma[i] = gammaNew;
        G[i] = xi/(xi + 1);
        Y[i] = G[i] * Y[i];
    }
    // Back to temporal domain
    for (int i = 0; i < buf; i++){
        Y[i] = Y[i] * exp(Complex(0.0, 1.0) * Yphase[i])/1000.0;
    }
    p = fftw_plan_dft_c2r_1d(buf, reinterpret_cast<fftw_complex*>(Y), output, FFTW_ESTIMATE);
    fftw_execute(p);
    count++;
    fftw_destroy_plan(p);
}

void Wiener::vad(Complex * Y, int buf) {
/**
 * Voice activity detector
 * @param Y is the FFT of the signal
 * @param buf is the size of the array
 */
    int noiseMargin = 3;
    int hangover = 8;
    double spectralDist[buf];
    double dist = 0;
    for (int i = 0; i < buf; i++){
        spectralDist[i] = max(20.0 * (log10(real(Y[i])) - log10(mean[i])), 0.0);
        dist += spectralDist[i];
    }
    dist /= buf;

    if (dist < noiseMargin){
        (noiseCounter)++;
    } else noiseCounter = 0;
    if (noiseCounter > hangover){
        speechFlag = false;
    } else speechFlag = true;
}
