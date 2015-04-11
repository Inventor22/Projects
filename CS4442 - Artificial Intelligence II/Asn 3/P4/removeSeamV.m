function [ imOut ] = removeSeamV( im, seam )

    if (ndims(im) == 3)

        imOut = zeros(size(im,1), size(im,2)-1, size(im,3));

        for i=1:length(seam)
            if (seam(i) == 1)
                imOut(i,:,:) = im(i,2:end,:);
            elseif (seam(i) == size(im,2))
                imOut(i,:,:) = im(i,1:end-1,:);
            else
                imOut(i,:,:) = [im(i,1:seam(i)-1, :), im(i, seam(i)+1:end, :)];
            end
        end
        
    elseif (ndims(im) == 2)
        
        imOut = zeros(size(im,1), size(im,2)-1);
        
        for i=1:length(seam)
            if (seam(i) == 1)
                imOut(i,:) = im(i,2:end);
            elseif (seam(i) == size(im,2))
                imOut(i,:) = im(i,1:end-1);
            else
                imOut(i,:) = [im(i,1:seam(i)-1), im(i, seam(i)+1:end)];
            end
        end
        
    end
    
end

