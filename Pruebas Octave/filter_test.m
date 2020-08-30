fs = 32000; %sampling frequency
f0 = 500;
gain =6; %10 dB
Q = 1;
[N0,N1,N2,D1,D2] = peak (fs,f0,gain,Q)



