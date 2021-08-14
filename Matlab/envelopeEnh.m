% Created by Delphine Somerhausen (delsomer@gmail.com)

function final = envelopeEnh(filename, tau, b, output, buf)
% Open the audio file of path filename and use the enhancement algorithm of
% parameter tau on it. b is the number of sample by buffer if buf is set to
% true. output is the path of the file the write the output in (as audio).
% Return the processed signal
[s, fs]= audioread(filename);
signal = s(:,1);
% Uncomment the next line to use noise reduction (should use a parameter)
% signal = WienerScalart96(signal, fs);
z = zeros(1,4);
z11 = zeros(1,7);z12 = zeros(1,2);z21 = zeros(1,7);z22 = zeros(1,2);
z31 = zeros(1,7);z32 = zeros(1,2);z41 = zeros(1,7);z42 = zeros(1,2);
if(buf)
    y = buffer(signal,b);
    m = 0;
    final = [];
    for column = 1:size(y,2)
        [processed,z, z11, z12, z21, z22, z31, z32, z41, z42] = ee(y(:,column)',fs,tau, z, z11, z12, z21, z22, z31, z32, z41, z42, buf);
        final = [final processed];
    end
else
    final = ee(signal,fs,tau, z, z11, z12, z21, z22, z31, z32, z41, z42, buf);
end
% audiowrite(output,final,fs);
end

function [enhancedBand, z1, z2] = enhancement(signal, fcmin, fcmax, fs, tau, z1, z2)
% Enhance a band of the given signal. fcmin and fcmax are the cutoff
% frequency of the band ; fs the sample rate ; tau the parameter of the
% enhancement ; z1 and z2 are the state of the filter
% Return the enhanced band and the states of the filters
  %6th order BP Butterworth filter
  [bBP,aBP] = butter(3,2*[fcmin,fcmax]/fs,'bandpass');
  [filtered, z1] = myFilter(bBP,aBP,signal, z1);
  % Full wave rectification
  rectified = abs(filtered);
  % 1st order LP Butterworth filter
  [b1LP,a1LP] = butter(1,2*32/fs,'low');
  [envelope,z2] = myFilter(b1LP,a1LP,rectified,z2);
  % Envelope enhancement
  kmin = 0.3;
  kmax = 4;
  kbi = exp((0-envelope)/tau)*(kmax-kmin)+kmin;
  exEnv = power(envelope, kbi);
  % Correction factor
  factor = exEnv./envelope;
  % Expanded signal
  enhancedBand = factor .* filtered;
end
 
 function [finalSig, z, z11, z12, z21, z22, z31, z32, z41, z42] = ee(signal, fs, tau, z, z11, z12, z21, z22, z31, z32, z41, z42, buf)
% Enhance the envelope of signal of sample rate fs ; z, z11, z12, z21, z22,
% z31, z32, z41, z42 are the states of the filters ; buf is a boolean, true
% if we process by buffer, else false ; tau is the parameter of the
% processing
% Return the enhanced signal and the states of the filters
  [rawSig1, z11, z12] = enhancement(signal,150,550,fs,tau,z11,z12);
  [rawSig2, z21, z22] = enhancement(signal,550,1550,fs,tau, z21, z22);
  [rawSig3, z31, z32] = enhancement(signal,1550,3550,fs,tau, z31, z32);
  [rawSig4, z41, z42] = enhancement(signal,3550,8000,fs,tau, z41, z42);
  rawSig = rawSig1 + rawSig2 + rawSig3 + rawSig4;
  
  [b,a] = butter(3,2*8000/fs,'low');
  [finalSig,z] = myFilter(b,a,rawSig,z);
  
  if (buf)
    max_theo = 10^(-0.3036 * (log10(tau))^2 - 3.3179 * log10(tau) - 6.6786);
    finalSig = finalSig / max_theo;
    for i = 1:length(finalSig)
        if finalSig(i)>1
            finalSig(i) = 0.99;
        end
        if finalSig(i)<-1
            finalSig(i) = -0.99;
        end
    end
  else
      p = RMS(signal);
      nRMS = RMS(finalSig);
      finalSig = finalSig*p/nRMS;
  end
 end

 function rms = RMS(signal)
% Custom rms function to translate into C++, take the signal and return the
% rms value
  ssum = mean(signal.^2);
  rms = sqrt(ssum);
 end
 
function [Y,z] = myFilter(b, a, X, z)
    % Author: Jan Simon, Heidelberg, (C) 2011
    % Edited: Delphine Somerhausen
% Filter the signal X using the vectors a and b as the parameters of the
% filter. z is the initial state of the filter.
% Returns the filtered signal Y and the end state z
% See : https://nl.mathworks.com/help/matlab/ref/filter.html
    n = length(a);
    b = b / a(1);  % [Edited, Jan, 26-Oct-2014, normalize parameters]
    a = a / a(1);
    Y = zeros(size(X));
    for m = 1:length(Y)
       Y(m) = b(1) * X(m) + z(1);
       for i = 2:n
          z(i - 1) = b(i) * X(m) + z(i) - a(i) * Y(m);
       end
    end
    z(n) = 0;
end