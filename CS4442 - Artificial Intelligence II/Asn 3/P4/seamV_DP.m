function [ M, P ] = seamV_DP( E )

    M = zeros(size(E));
    P = zeros(size(E));
    
    for c = 1:size(E,2)
       M(1,c) = E(1,c);
       P(1,c) = -1;
    end

    for r = 2:size(E,1)
        for c = 1:size(E,2)
            if (c-1 >= 1)
                opt1 = M(r-1,c-1);
            else
                opt1 = Inf;
            end
            
            opt2 = M(r-1,c);
            
            if (c+1 <= size(E,2))
                opt3 = M(r-1,c+1);
            else
                opt3 = Inf;
            end
            
            if (opt1 <= opt2 && opt1 <= opt3)
                M(r,c) = E(r,c) + M(r-1,c-1);
                P(r,c) = c-1;
            elseif (opt2 <= opt1 && opt2 <= opt3)
                M(r,c) = E(r,c) + M(r-1,c);
                P(r,c) = c;
            else
                M(r,c) = E(r,c) + M(r-1,c+1);
                P(r,c) = c+1;
            end
        end
    end
end

