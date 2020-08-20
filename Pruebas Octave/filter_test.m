fs = 48000; %sampling frequency
f0 = 600;
gain =10; %10 dB
Q = 1;
[N0,N1,N2,D1,D2] = peak (fs,f0,gain,Q)




