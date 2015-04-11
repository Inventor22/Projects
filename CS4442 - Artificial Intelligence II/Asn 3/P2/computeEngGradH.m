function [ eng ] = computeEngGradH( m )

    M = [1 0 -1];
    
    R = applyMask(m(:,:,1), M);
    G = applyMask(m(:,:,2), M);
    B = applyMask(m(:,:,3), M);
    
    eng = abs(R) + abs(G) + abs(B);
    
end

