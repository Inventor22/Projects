function [ seam, im, c, mask ] = increaseWidth( im, w, mask, maskWeight )
    
    eng = computeEngGradH(im) + computeEngColor(im, w);
    
    if (nargin == 4)
        eng = eng + (mask .* maskWeight);
    end
    
    [M, P] = seamV_DP(eng);
    [seam, c] = bestSeamV(M, P);
    
    im = addSeamV(im, seam);
    
    if (nargin == 4)
        mask = addSeamV(mask, seam);
    end
    
end

