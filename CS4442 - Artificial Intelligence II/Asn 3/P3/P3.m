im = imread('cat.png');

imshow(im);
im = double(im);

w = [1 -2 1];

outIm = computeEngColor(im, w);

sum(sum(outIm))
 
outIm = stretch(outIm);

figure;
imshow(uint8(outIm));

imwrite(uint8(outIm), 'P3/catEngC.png');