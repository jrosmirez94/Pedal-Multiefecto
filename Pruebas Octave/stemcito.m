x=0:0.0001:1;
%z=(sqrt(x+0.0390625).*181.03-0.197631836)/184.33;
z=-(0.2.*(abs(x)-0.8).*0.2.*(abs(x)-0.8))+1;
stem(z);
max(z)
min(z)
