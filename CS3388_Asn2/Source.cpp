/*
Name: Dustin Dobransky
ID:   250575030
Date: 13/10/14

Description:
This program reads in a wiremesh of an object and renders the object.

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

    // used as row vectors, so they can be appended to Matrix easily
    Mat e = (Mat_<float>(1,3) << 15., 15., 10.); // camera vector
    Mat g = (Mat_<float>(1,3) <<  0.,  0.,  1.); // a point through which the gaze direction unit vector n points to
    Mat p = (Mat_<float>(1,3) <<  0.,  0.,  1.); // x, y, z, w

    Mat n;
    normalize(e - g, n);

    Mat u, v;
    u = p.cross(n);
    v = u.cross(n);
    //u = abs(u);
    //v = abs(v);

    Mat ee = (Mat_<float>(3, 1) << 15., 15., 10.); // camera vector
    Mat gg = (Mat_<float>(3, 1) <<  0., 0., 1.); // a point through which the gaze direction unit vector n points to
    Mat pp = (Mat_<float>(3, 1) <<  0., 0., 1.); // x, y, z, w

    Mat nn;
    normalize(ee-gg, nn);
    Mat uu, vv;
    uu = pp.cross(nn);
    vv = uu.cross(nn);

    int64 t0 = getTickCount();
    for (int i = 0; i < 1000000; i++) {
        Mat Mvv = uu;
        hconcat(Mvv, vv, Mvv);
        hconcat(Mvv, nn, Mvv);
        Mvv.push_back(Mat((Mat_<float>(1, 3) << -ee.dot(uu), -ee.dot(vv), -ee.dot(nn))));
        Mvv = Mvv.t();
        Mvv.push_back(Mat((Mat_<float>(1, 4) << 0, 0, 0, 1))); // works
        cout << Mvv << endl;
    }
    cout << "Time hconcat: " << (getTickCount() - t0)/cvGetTickFrequency() << endl;

    t0 = getTickCount();
    for (int i = 0; i < 1000000; i++) {
        Mat Mv(0, 3, CV_32F);
        Mv.push_back(u);
        Mv.push_back(v);
        Mv.push_back(n);
        Mv = Mv.t();
        Mv.push_back(Mat((Mat_<float>(1, 3) << -e.dot(u), -e.dot(v), -e.dot(n))));
        Mv = Mv.t();
        Mv.push_back(Mat((Mat_<float>(1, 4) << 0, 0, 0, 1))); // works
        cout << Mv << endl;
    }
    cout << "Time transform: " << (getTickCount() - t0)/cvGetTickFrequency() << endl;

    float viewingAngle = 60.;
    float aspectRatio = 1;
    float N = 5.;
    float F = 25.;
    float t = N * tan(CV_PI / 180 * (viewingAngle / 2)); // top
    float b = -t; // bottom
    float r = aspectRatio*t; // right
    float l = -r; // left
    int   w = 512,
          h = 512;

    Mat S1T1Mp = (Mat_<float>(4, 4) <<
                  (2*N)/(r-l), 0,          (r+l)/(r-l),  0,
                  0,          (2*N)/(t-b), (t+b)/(t-b),  0,
                  0,           0,         -(F+N)/(F-N), -2*F*N/(F-N),
                  0,           0,          -1,           0
                  );

    Mat WS2T2 = (Mat_<float>(4, 4) <<
                 w/2, 0,   0,  w/2,
                 0,  -h/2, 0, -h/2+h,
                 0,   0,   1,  0,
                 0,   0,   0,  1
                 );

    Mat tt = (Mat_<float>(4, 1) << t, r, -N, 1.0);

    tt = WS2T2 * (S1T1Mp * tt);

    cout << tt << endl;
    cout << tt/tt.at<float>(3, 0) << endl;

    //PolygonalMesh p;
    //p.readFromFile("PolyMesh.xml");

    // chillout until the user has hit a key
    getchar();
    waitKey();
}