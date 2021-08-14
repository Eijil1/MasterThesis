// Created by Delphine Somerhausen (delsomer@gmail.com)

#include "ee.h"
#include <cmath>
using namespace std;

// Values computed with Matlab 2020, fs = 48000 (/!\ depends on fs)
const double a0[7] = {1, -5.89112487926268, 14.4654961302089, -18.9502918532601, 13.9691611259643, -5.49380660995052, 0.900566088981622};
const double b0[7] = {1.70396773247905e-05, 0, -5.11190319743714e-05, 0, 5.11190319743714e-05, 0,-1.70396773247905e-05};
const double a1[7] = {1, -5.69646472129517, 13.5651029985116, -17.2849629893385, 12.4300513783776, -4.78324487479802, 0.769520944446164};
const double b1[7] = {0.000247000815256913, 0, -0.000741002445770740, 0, 0.000741002445770740, 0, -0.000247000815256913};
const double a2[7] = {1, -5.22014029308371, 11.5945406124218, -14.0177905363876, 9.72887018794246, -3.67632341745084, 0.591483917467668};
const double b2[7] = {0.00175493044621185, 0, -0.00526479133863556, 0, 0.00526479133863556, 0, -0.00175493044621185};
const double a3[7] = {1, -3.68307128291900, 6.44823749814630, -6.68561458170832, 4.34159456504589, -1.66591795030139, 0.306295799105344};
const double b3[7] = {0.0149149334138019, 0, -0.0447448002414056, 0, 0.0447448002414056, 0, -0.0149149334138019};
const double aLP32[2] = {1, -0.995819958342083};
const double bLP32[2] = {0.00209002082895848, 0.00209002082895848};
const double aLP8000[4] = {1, -0.965779713179161, 0.582644165984302, -0.106017056545330};
const double bLP8000[4] = {0.0638559245324764, 0.191567773597429, 0.191567773597429, 0.0638559245324764};
// States of the filter
double z0[4] = {0,0,0,0};
double z01[7] = {0,0,0,0,0,0,0};
double z02[2] = {0,0};
double z11[7] = {0,0,0,0,0,0,0};
double z12[2] = {0,0};
double z21[7] = {0,0,0,0,0,0,0};
double z22[2] = {0,0};
double z31[7] = {0,0,0,0,0,0,0};
double z32[2] = {0,0};

void ee::process(double* signal, double tau, int buf, double* output){
/**
 * Apply envelope enhancement to the given signal
 * @param signal
 * @param tau : the parameter of the enhancement
 * @param buf : the size of the given signal
 * @param output : where to write the output
 */
    double sum[buf];
    sum4bands(signal,tau, buf, sum);
    double filtered[buf];
    filter(sum,aLP8000, bLP8000, 4, buf, filtered, z0);
    normalize(filtered, buf, tau, output);
}

void ee::sum4bands(double *signal, double tau, int buf, double *sum) {
/**
 * Divide the signal into four bands of frequencies, enhancement them and sum them
 * @param signal
 * @param tau : the parameter of the enhancement
 * @param buf : the size of the given signal
 * @param sum : where to write the output
 */
    double band0[buf];
    double band1[buf];
    double band2[buf];
    double band3[buf];
    enhancedBand(signal, tau, 0, buf, band0);
    enhancedBand(signal, tau, 1, buf, band1);
    enhancedBand(signal, tau, 2, buf, band2);
    enhancedBand(signal, tau, 3, buf, band3);
    for (int i = 0; i < buf; i++){
        sum[i] = band0[i] + band1[i] + band2[i] + band3[i];
    }
}

void ee::enhancedBand(const double *signal, double tau, int bandNumber, int buf, double *enhanced) {
/**
 * Filter the signal to extract one band of frequence and enhance it
 * @param signal
 * @param tau : the parameter of the enhancement
 * @param bandNumber : which band is being processed
 * @param buf : the size of the given signal
 * @param enhanced : where to write the output
 */
    // Assign a, b, z1 and z2 for the band
    const double *a; const double *b;
    double *z1; double *z2;
    switch(bandNumber){
       case 0 : a = a0; b = b0; z1 = z01; z2 = z02; break;
       case 1 : a = a1; b = b1; z1 = z11; z2 = z12; break;
       case 2 : a = a2; b = b2; z1 = z21; z2 = z22; break;
       case 3 : a = a3; b = b3; z1 = z31; z2 = z32; break;
    }
    // Extract the band
    double  filtered[buf];
    filter(signal, a, b, 7, buf, filtered, z1);
    // Extract envelope
    double rectified[buf];
    fullWaveRect(filtered, buf, rectified);
    double envelope[buf];
    filter(rectified, aLP32, bLP32, 2, buf, envelope, z2);
    // Enhancement
    enhancement(envelope, filtered, tau, buf, enhanced);
}

void ee::filter(const double* signal, const double *a, const double* b, int n, int buf, double* filtered, double* z) {
/**
 * Filter the given signal, translation of : https://nl.mathworks.com/matlabcentral/answers/9900-use-filter-constants-to-hard-code-filter
 * @param signal
 * @param a : the coefficient of the numerator of the filter
 * @param b : the coefficient of the denominator of the filter
 * @param n : the order (size of a and b)
 * @param buf : the size of the given signal
 * @param filtered : where to write the output
 * @param z : the state of the filter
 */
    for (int m = 0; m < buf; m++){
        filtered[m] = b[0] * signal[m] + z[0];
        for (int i = 1; i < n; i++) {
            z[i - 1] = b[i] * signal[m] + z[i] - a[i] * filtered[m];
        }
    }
    z[n-1] = 0;
}

void ee::fullWaveRect(const double *signal, int buf, double* rectified) {
/**
 * Take the absolute value of the signal
 * @param signal
 * @param buf : the size of the given signal
 * @param rectified : where to write the output
 */
    for (int i = 0; i < buf; i++){
        if (signal[i] < 0){
            rectified[i] = -signal[i];
        }
        else{
            rectified[i] = signal[i];
        }
    }
}

void ee::enhancement(double *envelope, const double *filtered, double tau, int buf, double *enhanced) {
/**
 * Enhance the given envelope
 * @param envelope : the envelope of the signal
 * @param filtered : the initial signal (on the band)
 * @param tau : the parameter of the enhancement
 * @param buf : the size of the given signal
 * @param enhanced : where to write the output
 */
    double temp;
    double kmin = 0.3;
    double kmax = 4;
    for (int i = 0; i < buf; i++){
        temp = exp((0 - envelope[i])/tau)*(kmax-kmin) + kmin;
        temp = pow(envelope[i],temp);
        temp = temp/envelope[i];
        enhanced[i] = temp * filtered[i];
    }
}

double ee::rms(const double *signal, int buf) {
/**
 * Compute the rms of the signal (not used anymore)
 * @param signal
 * @param buf : the size of the given signal
 */
    double squareSum = 0;
    for (int i = 0; i < buf; i++){
        squareSum = squareSum + signal[i] * signal[i];
    }
    return sqrt(squareSum);
}

void ee::normalize(double *signal, int buf, float tau, double *normalized) {
/**
 * Normalize the signal
 * @param signal
 * @param tau : the parameter of the enhancement
 * @param buf : the size of the given signal
 * @param normalized : where to write the output
 */
    double factor = pow(10, (-0.3036 * pow(log10(tau), 2) - 3.3179 * log10(tau) - 6.6786));
    for (int i = 0; i < buf; i++){
        normalized[i] = signal[i]/factor;
        if (normalized[i] < -1) normalized[i] = -0.99;
        else if (normalized[i] > 1) normalized[i] = 0.99;
    }
}

