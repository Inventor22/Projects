im = imread('face.jpg');

imshow(im);
im = double(im);

outIm = computeEngGradH(im);

sum(sum(outIm))
 
outIm = stretch(outIm);

figure;
imshow(uint8(outIm));

imwrite(uint8(outIm), 'P2/faceEngG.jpg');