im = imread('swan.png');

imshow(im);

im = double(im);

M = [-1 0 1;
     -2 0 2;
     -1 0 1];

outIm = applyMask(im, M);

sum(sum(abs(outIm)))
 
outIm = stretch(outIm);

imshow(uint8(outIm));

imwrite(uint8(outIm), 'P1/filteredSwan.png');