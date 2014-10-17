/*
Name: Dustin Dobransky
ID:   250575030
Date: 13/10/14

Description:
This program reads in coordinates for a skeleton of an object and renders the object.

*/
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <fstream>

#include "Face.h"
#include "Normal.h"
#include "PolygonalMesh.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv) {

    float a = 18. / 180 * CV_PI;

    Matx44f rz(
        cos(a), -sin(a), 0, 0,
        sin(a), cos(a), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1);

    Vec4f cam(15., 15., 10., 1.);
    Vec4f gaze(0., 0., 1., 1.);

    try {
        // nothing
    }
    catch (cv::Exception e) {
        cout << e.what() << endl;
    }

    Vec4f up(0, 0, 1, 1); // x, y, z, w

    Vec4f N;

    normalize(cam - gaze, N);

    cout << N << endl;

    //Matx<float, 3, 1> up3 = up.get_minor<3, 1>(3, 1);
    //Matx<float, 3, 1> N3  = N.get_minor<3, 1>(3, 1);

    Mat asd(up.get_minor<1, 3>(1, 3));
    Mat asdf(N.get_minor<1, 3>(1, 3));

    asd.cross(asdf);



    //PolygonalMesh p;
    //p.readFromFile("PolyMesh.xml");


    // chillout until the user has hit a key
    waitKey();
}