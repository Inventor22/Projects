f = [3 5 4 4 2 4 2;
     7 4 1 4 4 2 6;
     2 7 46 46 46 6 7;
     5 9 46 46 44 9 7;
     4 7 47 47 47 2 4;
     4 7 56 56 46 6 7;
     3 4 4 1 4 3 2];
 
 g = [3 5 4 4 2 4 2;
      7 4 1 4 4 2 6;
      46 46 46 3 6 6 7;
      48 46 44 6 4 9 7;
      47 47 47 7 4 2 4;
      58 56 46 5 6 6 7;
      3 4 4 1 4 3 2];
  
  d = 2;
  
  ad = abs(f(:,(1+d):end) - g(:,1:end-d));
  
  ad = [zeros(size(ad,1),d), ad];
  
  