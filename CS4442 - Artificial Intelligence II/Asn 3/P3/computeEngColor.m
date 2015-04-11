function [ eng ] = computeEngColor( im, w )
    
    eng = w(1)*im(:,:,1) + w(2)*im(:,:,2) + w(3)*im(:,:,3);

end