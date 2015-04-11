function [ seam, im, c, mask ] = reduceWidth( im, w, mask, maskWeight )

    eng = computeEngGradH(im) + computeEngColor(im, w);
    
    if (nargin == 4)
        eng = eng + (mask .* maskWeight);
    end
    
    [M, P] = seamV_DP(eng);
    [seam, c] = bestSeamV(M, P);
    
    im = removeSeamV(im, seam);
    
    if (nargin == 4)
        mask = removeSeamV(mask, seam);
    end 
    
end

