%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Performs foreground/background segmentation based on a graph cut
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% INPUT: image im and scribbleMask with scribbles
% scribbleMask(i,j) = 2 means pixel(i,j) is a foreground seed
% scribbleMask(i,j) = 1 means pixel(i,j) is a background seed
% scribbleMask(i,j) = 0 means pixel(i,j) is not a seed
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% OUTPUT: segm is the segmentation mask of the  same size as input image im
% segm(i,j) = 1 means pixel (i,j) is the foreground
% segm(i,j) = 0 means pixel (i,j) is the background
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function segm = segmentGC(im, scribbleMask)
    
    bg = zeros(size(im));
    fg = zeros(size(im));
    
    bg(scribbleMask == 1) = 1000;
    %bg(scribbleMask == 2) = 1000;
    
    %fg(scribbleMask == 1) = 1000;
    fg(scribbleMask == 2) = 1000;
    
    fga = zeros(1, size(fg,1) * size(fg,2)); %preallocate row vectors
    bga = zeros(1, size(bg,1) * size(bg,2));
    
    for c = 1:size(fg,2) % push matrices into row vectors
        colfg = fg(:,c);
        rowfg = colfg';
        fga((c-1)*size(fg,1)+1:c*size(fg,1)) = rowfg;
        
        colbg = bg(:,c);
        rowbg = colbg';
        bga((c-1)*size(bg,1)+1:c*size(bg,1)) = rowbg;
    end
        
    nr = size(im,1);
    nc = size(im,2);
    W = zeros(2*((nr-1)*nc + nr*(nc-1)), 3); % preallocate weights matrix
    
    lambda = 10; % experimentally determined
    sigma  = 3.14;
    
    w = size(im,2);
    h = size(im,1);
    
    n = 1;
    
    % Wpq = lambda * exp(-abs(f(p)-f(q))^2 / (2*sigma^2))
    
    % calculate weights and push into weights matrix
    for r = 1:size(im,1)
        for c = 1:size(im,2)-1
             W(n,:) = [h*(c-1)+r, h*c+r, ...
                lambda * exp(-(norm(im(r,c) - im(r,c+1)))^2 ...
                               / (2 * sigma^2)) ];
            n = n+1;
            
            W(n,:) = [h*c+r, h*(c-1)+r, W(n-1, 3)];
            n = n+1;
        end
    end
    
    for r = 1:size(im,1)-1
        for c = 1:size(im,2)
             W(n,:) = [h*(c-1)+r, h*(c-1)+r+1, ...
                lambda * exp(-(norm(im(r,c) - im(r+1,c)))^2 ...
                               / (2 * sigma^2)) ];
            n = n+1;
            
            W(n,:) = [h*(c-1)+r+1, h*(c-1)+r, W(n-1, 3)];
            n = n+1;
        end
    end
    
%     disp(max([W(:,1); W(:,2)]));
%     disp(min([W(:,1); W(:,2)]));
%     disp(size(fga,2));
    
    labels = solveMinCut(fga, bga, W);
    
    segm = zeros(size(im,1), size(im,2));
    
    for k = 1:size(labels,2)
        zr = k - floor(k/h)*h;
        if (zr == 0) % so we stay in bounds
            zr = h;
        end
        zc = ceil(k/h);
        segm(zr, zc) = labels(k);
    end
    %segm = ones(row,col);  % return the whole image as the foreground
    
end
