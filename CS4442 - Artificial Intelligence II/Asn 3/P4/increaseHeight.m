function [ seam, im, c, mask ] = increaseHeight( im, w, mask, maskWeight )

    im = permute(im, [2 1 3]);
    
    if (nargin == 2)
        [seam, im] = increaseWidth(im, w);
    else
        mask = mask';
        [seam, im, c, mask] = increaseWidth(im, w, mask, maskWeight);
        mask = mask';
    end
    
    im = permute(im, [2 1 3]);

end

