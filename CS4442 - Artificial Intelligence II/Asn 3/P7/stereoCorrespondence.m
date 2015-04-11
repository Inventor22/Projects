function [ disp ] = stereoCorrespondence(left,right,wSize,maxDisp)

    bestDisparity = zeros(size(left));
    bestWindCost  = Inf(size(left));
    
    o = floor(wSize/2);
    w = size(left,1);
    h = size(left,2);
    
    for d = 1:maxDisp
        % Pad with zeros on left side and
        % Compute absolute difference at disparity d
        ad = [zeros(size(left,1), d), abs(left(:,(1+d):end) - right(:,1:end-d))];
        

        % compute integral image
        int = integralImage(ad);
        %int = padarray(int,[w w]);

        for r = 1:size(left,1)
            for c = 1:size(left,2)
                currentCost = ...
                      int(min(w, r+o+1), min(h, c+o+1))... % bottom right
                    + int(max(2, r+1-o), max(2, c+1-o))... % top left
                    - int(max(2, r+1-o), min(h, c+o+1))... % top right
                    - int(min(w, r+o+1), max(2, c+1-o));   % bottom left
                
                if (currentCost < bestWindCost(r,c))
                    bestWindCost(r,c) = currentCost;
                    bestDisparity(r,c) = d;
                end
            end
        end
    end
    
    disp = bestDisparity;

end

