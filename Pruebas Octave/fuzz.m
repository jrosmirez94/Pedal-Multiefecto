filename="C:/Users/jrosmirez/Desktop/guitarraone.wav";
[y, fs] = audioread (filename);
y=y(:,1); % es para agarrar un canal
y=y/max(y);
audiowrite(filename,y,fs);
DMA_HALF_SIZE=2048;

out=[];

for i=1:floor(length(y)/DMA_HALF_SIZE)-1
    block=y(1+DMA_HALF_SIZE*i:(i+1)*DMA_HALF_SIZE); % un bloque de 2048
    z=[];
    for j=1:DMA_HALF_SIZE
        x=(block(j));
        if x>0
            s=1;
        else
            s=-1;
        endif    
        x=abs(block(j));
        %z=[z s*(sqrt(x+0.0390625).*181.03-35.779631836)/184.33];  
        z=[z s*(-((abs(x)-1).*(abs(x)-1))+1)];
        
    endfor
    out=[out z];
endfor
filename_out="C:/Users/jrosmirez/Desktop/salida.wav";
%normalizacion
out=out/max(out)*max(y);
audiowrite(filename_out,out,fs);