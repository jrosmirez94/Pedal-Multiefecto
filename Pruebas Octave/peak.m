function [N0,N1,N2,D1,D2] = peak (fs,f0,gain,Q)

%intermediate parameters
wo = 2*pi*f0/fs
cosW = cos(wo)
sinW = sin(wo)
A = 10^(gain/40)
alpha = sinW/(2*Q*A)
% %Peaking EQ coefficients

b0 = 10^(gain/20)*(1 + alpha*A)
b1 = 10^(gain/20)*(-2*cos(wo))
b2 = 10^(gain/20)*(1 - alpha*A)
a0 = 1 + (alpha/A)
a1 = -2*cos(wo)
a2 = 1 - (alpha/A)

den = [a0, a1, a2];
num=[b0,b1,b2]; 

H = tf(num,den,0.1)
bode(H), grid


%Normalize so that A0 = 1
B0 = b0/a0
B1 = b1/(2*a0)
B2 = b2/a0
A1 = a1/(-2*a0)
A2 = a2/(-a0)
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

N0 = N0/32768;
N1 = N1/32768;
N2 = N2/32768;
D1 = D1/32768;
D2 = D2/32768;
end
