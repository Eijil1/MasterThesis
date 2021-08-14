% The code initially come from : https://jp.mathworks.com/matlabcentral/fileexchange/7673-wiener-filter?s_tid=FX_rc1_behav
% Modified by Delphine Somerhausen (delsomer@gmail.com)
% It was highly modified while debugging a translation into C++, which
% explaines the loop and so on that are not necessary in MATLAB

function output=WienerScalart96(signal,fs)
% output=WIENERSCALART96(signal,fs,IS)
% Wiener filter based on tracking a priori SNR usingDecision-Directed 
% method, proposed by Scalart et al 96. In this method it is assumed that
% SNRpost=SNRprior +1. based on this the Wiener Filter can be adapted to a
% model like Ephraims model in which we have a gain function which is a
% function of a priori SNR and a priori SNR is being tracked using Decision
% Directed method. 
% Author: Esfandiar Zavarehei
% Created: MAR-05
% Edited: Delphine Somerhausen (2021)

W=1000;
y = buffer(signal,W);
NoiseCounter=0;
G=ones(W, 1);
Gamma=G;
N = zeros(W, 1);
LambdaD = zeros(W, 1);
YPhase = zeros(W, 1);
output = [];

numberOfFrames=size(y,2);
IS=.25; %Initial Silence or Noise Only part in seconds
NIS = (IS*fs)/W; %number of initial silence segments
alpha=.99; %used in smoothing xi (For Deciesion Directed method for estimation of A Priori SNR)
NoiseLength=9;%This is a smoothing factor for the noise updating

for i=1:numberOfFrames
    Y = fft(y(:,i));
    for n=1:W
        YPhase(n)=angle(Y(n)); %Noisy Speech Phase
        Y(n)=abs(Y(n));%Specrogram
    end
    %%%%%%%%%%%%%VAD and Noise Estimation START
    if i<=NIS % If initial silence ignore VAD
        for n=1:W
            N(n) = (N(n) * (i-1) + real(Y(n)))/i;
            LambdaD(n) = (LambdaD(n) * (i-1) + real(Y(n)^2))/i;
        end
        SpeechFlag=0;
        NoiseCounter=100;
    else % Else Do VAD
        [SpeechFlag, NoiseCounter]=vad(Y,N,NoiseCounter); %Magnitude Spectrum Distance VAD
    end
    
    if SpeechFlag==0 % If not Speech Update Noise Parameters
        for n=1:W
            N(n)=(NoiseLength*N(n)+real(Y(n)))/(NoiseLength+1); %Update and smooth noise mean
            LambdaD(n)=(NoiseLength*LambdaD(n)+real(Y(n)^2))/(1+NoiseLength); %Update and smooth noise variance
        end
    end
%     %%%%%%%%%%%%%%%%VAD and Noise Estimation END
    for n=1:W
        gammaNew=real(Y(n)^2)/LambdaD(n); %A postiriori SNR
        xi=alpha*(G(n)^2)*Gamma(n)+(1-alpha)*max(gammaNew-1,0); %Decision Directed Method for A Priori SNR
        Gamma(n)=gammaNew;
        G(n)=(xi/(xi+1));
        Y(n)=G(n)*Y(n); %Obtain the new Cleaned value
    end
    output = [output,real(ifft(Y.*exp(j*YPhase)))'];
end
end

function [SpeechFlag, NoiseCounter]=vad(signal,noise,NoiseCounter)
% Voice activity detector : take the signal, the mean noise and the noise
% counter. Return whether there is speech or not as a speechFlag and the
% updated noiseCounter. For more information, see the initial code.
NoiseMargin=3;
Hangover=8;
W = 1000;
SpectralDist = zeros(W, 1);
for i = 1:W
    SpectralDist(i) = max(20*(log10(signal(i))-log10(noise(i))), 0);
end
Dist=mean(SpectralDist);
if (Dist < NoiseMargin) 
    NoiseCounter=NoiseCounter+1;
else
    NoiseCounter=0;
end
% Detect noise only periods and attenuate the signal     
if (NoiseCounter > Hangover) 
    SpeechFlag=0;    
else 
    SpeechFlag=1; 
end 
end
