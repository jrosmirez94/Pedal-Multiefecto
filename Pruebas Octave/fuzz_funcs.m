function [y] = fuzz_funcs(x,pote)


	%pote+=0x008E;
	threshold=[0, hex2dec('0100'), hex2dec('1000'),hex2dec('4000'),hex2dec('7FFF')];
    threshold=threshold/hex2dec('7FFF');
    pendiente=[2, 1.5, 1, 0.5, 0];
    prev=0;
	y=x;
        for i=1:length(threshold)
	
            if abs(x)<threshold(i)
		
                y=(abs(x)-threshold(i-1))*pendiente(i)+prev;
                y=y*sign(x);
                break;
            end
            prev=prev+(threshold(i)-prev)*pendiente(i);
        end
	
end
