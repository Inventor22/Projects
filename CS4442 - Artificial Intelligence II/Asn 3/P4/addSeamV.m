function [ imOut ] = addSeamV( im, seam )

    imOut = zeros(size(im,1), size(im,2)+1, size(im,3));
    %imOut = zeros(size(im));
    
    for i=1:length(seam)
        if (seam(i) == 1)
            imOut(i,:,:) = [seam(i), im(i,1:end,:)];
        elseif (seam(i) == size(im,2))
            imOut(i,:,:) = [im(i,1:end,:), seam(i)];
        else
            %d = im(i, seam(i)-1, :);
            %+ im(i, seam(i)+1, :));
            %f =[(im(i, seam(i)-1, :) + im(i, seam(i)+1, :)) / 2];
            
            imOut(i,:,:) = [im(i,1:seam(i)-1, :), ...
                           (im(i, seam(i)-1, :) + im(i, seam(i), :)) ./ 2, ...
                            im(i, seam(i):end, :)];
        end
    end

end

