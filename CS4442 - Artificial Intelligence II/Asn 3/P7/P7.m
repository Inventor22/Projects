im = {'teddy-left.pgm', 'teddy-right.pgm';
      'venus-left.pgm', 'venus-right.pgm';
      'tsukuba-left.pgm', 'tsukuba-right.pgm'};

  close all;
  
for i = 1:size(im,1)
    imL = imread(im{i,1});
    imR = imread(im{i,2});

    figure;
    subplot(1,3,1);
    subimage(imL);
    subplot(1,3,2);
    subimage(imR);

     disp = stereoCorrespondence(imL, imR, 3, 25);
%    disp = stereoCorrespondence(imL, imR, 11, 50);

    disp = stretch(disp);

    subplot(1,3,3);
    subimage(uint8(disp));

end
