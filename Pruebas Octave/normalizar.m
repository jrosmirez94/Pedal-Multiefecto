function [N0,N1,N2,D1,D2] = normalizar(num,den)
a0=den(1);
a1=den(2);
a2=den(3);
b0=num(1);
b1=num(2);
b2=num(3);
%Normalize so that A0 = 1
B0 = b0/a0;
B1 = b1/(2*a0);
B2 = b2/a0;
A1 = a1/(-2*a0);
A2 = a2/(-a0);
Mx = max(abs([B0, B1, B2]));
if Mx > 1
B0new = B0/Mx;
B1new = B1/Mx;
B2new = B2/Mx;
else
B0new = B0;
B1new = B1;
B2new = B2;
end
NB = 16; % number of bits
Range = 2^(NB-1)-1;
N0 = floor(B0new*Range);
N1 = floor(B1new*Range);
N2 = floor(B2new*Range);
D1 = floor(A1*Range);
D2 = floor(A2*Range);
N0 = N0/16384;
N1 = N1/16384;
N2 = N2/16384;
D1 = D1/16384;
D2 = D2/16384;
end
