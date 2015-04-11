function [ outIm ] = applyMask( im, M )
    sr = size(M,1);
    sc = size(M,2);
    
    or = (sr-1)/2;
    oc = (sc-1)/2;
    
    outIm = zeros(size(im,1), size(im,2));
    
    im = padarray(im, [or oc]); % pad array with zeros
    
    for i=1:size(outIm,1)
        for j=1:size(outIm,2)
            outIm(i, j) = sum(sum( im(i:i+sr-1, j:j+sc-1) .* M) );
        end
    end
    
end

