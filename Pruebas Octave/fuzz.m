filename='C:\Users\Juan Rosmirez\Desktop\asusopen.wav';
[y, fs] = audioread (filename);
y=y(:,1); % es para agarrar un canal
y=y/max(y);
audiowrite(filename,y,fs);
DMA_HALF_SIZE=2048;
pote=1;

out=[];
y=y(10000:end);
for i=1:floor(length(y)/DMA_HALF_SIZE)-1
    block=y(1+DMA_HALF_SIZE*i:(i+1)*DMA_HALF_SIZE); % un bloque de 2048
    z=[];
    for j=1:DMA_HALF_SIZE
        x=(block(j));
        z= [z fuzz_funcs(x,pote)];
        
        
    end
    out=[out z];
end
filename_out='C:\Users\Juan Rosmirez\Desktop\salida.wav';
%normalizacion para que suene a maximo volumen
out=out/max(out)*max(y);
audiowrite(filename_out,out,fs);