fs = 48000;
N = 1024;
n = 1:N;
t = n/fs;
f0 = fs/N

f = 9*f0;

s = int32((sin(2*pi*f*t))*((0.5*2^8)-1));
m = zeros(1, 2*N);
for i = 1:N
    m(2*i) = s(i);
end

subplot(211);
plot(s);
subplot(212);
plot(m);

csvwrite('myFile_s.txt',s);
csvwrite('myFile_m.txt',m);
max(s)
min(s)