function [ seam, c ] = bestSeamV( M, P )

    seam = zeros(size(M,1),1);
    [c, i] = min(M(end,:));
    seam(end) = i;
    
    for r = size(M,1)-1:-1:1
        seam(r) = P(r+1, seam(r+1));
    end

end

