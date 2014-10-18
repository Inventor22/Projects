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
    // roation matrix around z axis.
    Matx44f rz(
        cos(a), -sin(a), 0, 0,
        sin(a), cos(a), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1);

    // used as row vectors, so they can be appended to Matrix easily
    Mat e = (Mat_<float>(1,3) << 15., 15., 10.); // camera vector
    Mat g = (Mat_<float>(1,3) <<  0.,  0.,  1.); // a point through which the gaze direction unit vector n points to
    Mat p = (Mat_<float>(1,3) <<  0.,  0.,  1.); // x, y, z, w

    cout << e - g << endl;

    Mat n;
    normalize(e - g, n);
    cout << n << endl;

    Mat u, v;
    u = p.cross(n);
    v = u.cross(n);

    cout << u << endl;
    cout << v << endl;

    Mat M(0,3,CV_32F);
    M.push_back(u);
    M.push_back(v);
    M.push_back(n);
    M.push_back(e);
    cout << M << endl;
    M = M.t();
    cout << M << endl;
    try {
        //M.push_back((Mat_<float>(1, 4) << 0, 0, 0, 1));    // fails
        //M.push_back(Vec4f(0, 0, 0, 1));                    // fails
        M.push_back(Mat((Mat_<float>(1, 4) << 0, 0, 0, 1)));
    }
    catch (cv::Exception e) {
        cout << e.what() << endl;
    }

//    Mat M(1, 3, CV_32F);
//
//    try {
//        hconcat(M, u, M);
//    }
//    catch (cv::Exception e) {
//        cout << e.what() << endl;
//    }
//    hconcat(M, v, M);
//    hconcat(M, n, M);
//    hconcat(M, e, M);
////    M.push_back(j);
//    Mat my = (Mat_<float>(4, 1) << 0, 0, 0, 1);
//    M.push_back(my);

    Mat Mv;
    Mv.push_back( u.t() );

    //Matx<float, 3, 1> up3 = up.get_minor<3, 1>(3, 1);
    //Matx<float, 3, 1> N3  = N.get_minor<3, 1>(3, 1);

    //Mat asd(up.get_minor<1, 3>(1, 3));
    //Mat asdf(N.get_minor<1, 3>(1, 3));

    //asd.cross(asdf);



    //PolygonalMesh p;
    //p.readFromFile("PolyMesh.xml");


    // chillout until the user has hit a key
    waitKey();
}