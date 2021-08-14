/*
 * Copyright 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
// Modified by Delphine Somerhausen (delsomer@gmail.com)

#ifndef SAMPLES_FULLDUPLEXPASS_H
#define SAMPLES_FULLDUPLEXPASS_H

#include "FullDuplexStream.h"
#include <complex>
#include <vector>

using namespace std;

typedef complex<double> Complex;

class FullDuplexPass : public FullDuplexStream {
public:
    virtual oboe::DataCallbackResult onBothStreamsReady(
            std::shared_ptr<oboe::AudioStream> inputStream,
            const void *inputData,
            int   numInputFrames,
            std::shared_ptr<oboe::AudioStream> outputStream,
            void *outputData,
            int   numOutputFrames);

    void process(double *signal, bool nr, float tau, int buf, double* output);

private:
	vector<double> mean;
    vector<double> power;
    vector<double> G;
    vector<double> Gamma;

    int noiseCounter = 0;
    bool speechFlag = false;
    long count = 0;

    void noiseReduction(double *signal, int buf, double *output);

    void vad(Complex * y, int buf);

    void filter(double *signal, const double *a, const double *b, int n, int buf, double *filtered, double* z);

	void enhancedBand(double *signal, float tau, int bandNumber, int buf, double *enhanced);

	void fullWaveRect(double *signal, int buf, double *rectified);

	void enhancement(double *envelope, double *filtered, float tau, int buf, double *enhanced);

	void sum4bands(double *signal, float tau, int buf, double *sum);

	void normalize(double *signal, int buf, float tau, double *normalized);
};
#endif //SAMPLES_FULLDUPLEXPASS_H