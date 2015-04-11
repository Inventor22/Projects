function [ seam, im, c, mask ] = reduceHeight( im, w, mask, maskWeight )

    im = permute(im, [2 1 3]);
    
    if (nargin == 2)
        [seam, im] = reduceWidth(im, w);
    else
        mask = mask';
        [seam, im, c, mask] = reduceWidth(im, w, mask, maskWeight);
        mask = mask';
    end
    
    im = permute(im, [2 1 3]);

end

