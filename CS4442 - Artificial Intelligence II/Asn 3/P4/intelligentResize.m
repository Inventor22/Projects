function [ imOut ] = intelligentResize( im, v, h, w, mask, maskWeight )

    nn = 2*min(abs(v),abs(h));
    nv = abs(v);
    nh = abs(h);
    
    totalCost = 0;
    
    for n = 1:nn
        if (mod(n,2) == 0)
            if (v < 0)
                [seam, im, c, mask] = reduceHeight(im, w, mask, maskWeight);
            else
                [seam, im, c, mask] = increaseHeight(im, w, mask, maskWeight);
            end
            nv = nv - 1;
        else
            if (h < 0)
                [seam, im, c, mask] = reduceWidth(im, w, mask, maskWeight);
            else
                [seam, im, c, mask] = increaseWidth(im, w, mask, maskWeight);
            end
            nh = nh - 1;
        end
        totalCost = totalCost + c;
    end
    
    for vv = 1:nv
        if (v < 0)
            [seam, im, c, mask] = reduceHeight(im, w, mask, maskWeight);
        else
            [seam, im, c, mask] = increaseHeight(im,w,mask,maskWeight);
        end
        totalCost = totalCost + c;
    end
    
    for hh = 1:nh
        if (h < 0)
            [seam, im, c, mask] = reduceWidth(im, w, mask, maskWeight);
        else
            [seam, im, c, mask] = increaseWidth(im,w,mask,maskWeight);
        end
        totalCost = totalCost + c;
    end
    
    fprintf('totalCost: %d\n', totalCost);

    imOut = im;
    
end

