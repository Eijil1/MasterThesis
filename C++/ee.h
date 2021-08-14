// Created by Delphine Somerhausen (delsomer@gmail.com)

#ifndef ENVENH_EE_H
#define ENVENH_EE_H


class ee {

public:
    static void process(double *signal, double tau, int buf, double* output);

private:

    static void filter(const double *signal, const double *a, const double *b, int n, int buf, double *filtered, double* z);

    static void enhancedBand(const double *signal, double tau, int bandNumber, int buf, double *enhanced);

    static void fullWaveRect(const double *signal, int buf, double *rectified);

    static void enhancement(double *envelope, const double *filtered, double tau, int buf, double *enhanced);

    static void sum4bands(double *signal, double tau, int buf, double *sum);

    static double rms(const double *signal, int buf);

    static void normalize(double *signal, int buf, float tau, double *normalized);
};


#endif //ENVENH_EE_H
