function [ outIm ] = applyMask( im, M )
    outIm = zeros(size(im,1)+2, size(im,2)+2);
    
    outIm(2:end-1, 2:end-1) = im;
    
    
    
end

