%% Cat

im = imread('cat.png');

close all;

imshow(im);
im = double(im);

w = [1 -2 1];
v = -20;
h = -20;
mask = zeros(size(im, 1), size(im, 2));
maskWeight = 0;

% s = tic;

outIm = intelligentResize(im, v, h, w, mask, maskWeight);

% e = toc(s);
% disp(e); % ~30 seconds

figure;
imshow(uint8(outIm));

imwrite(uint8(outIm), 'P4/catResized.png');

%% Face

im = imread('face.jpg');

figure;
imshow(im);

im = double(im);

v = -20;
h = 10;

mask = zeros(size(im,1), size(im,2));

outIm = intelligentResize(im, v, h, w, mask, maskWeight);

figure;
imshow(uint8(outIm));

imwrite(uint8(outIm), 'P4/faceResized.jpg');

%% DogCat
% 
% im = imread('DogCat.png');
% 
% close all;
% 
% imshow(im);
% im = double(im);
% 
% w = [1 -2 1];
% v = 0;
% h = -40;
% mask = zeros(size(im, 1), size(im, 2));
% maskWeight = 0;
% 
% % s = tic;
% 
% outIm = intelligentResize(im, v, h, w, mask, maskWeight);
% 
% figure;
% imshow(uint8(outIm));
% 
% imwrite(uint8(outIm), 'P4/DogCatResized.png');

%% Spring Lamb

% im = imread('SpringLamb.jpg');
% 
% close all;
% 
% imshow(im);
% im = double(im);
% 
% w = [1 -2 1];
% v = 0;
% h = -150;
% mask = zeros(size(im, 1), size(im, 2));
% maskWeight = 0;
% 
% % s = tic;
% 
% outIm = intelligentResize(im, v, h, w, mask, maskWeight);
% 
% figure;
% imshow(uint8(outIm));
% 
% imwrite(uint8(outIm), 'P4/SpringLambResized.jpg');