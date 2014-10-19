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

    cout << "e: " << e << endl;
    cout << "g: " << e << endl;
    cout << "p: " << e << endl;

    Mat n;
    normalize(e - g, n);
    cout << "n: " << n << endl;

    Mat u, v;
    u = p.cross(n);
    v = u.cross(n);
    //u = n.cross(p);
    //v = n.cross(u);
    //u = abs(u);
    //v = abs(v);

    cout << "u: " << u << endl;
    cout << "v: " << v << endl;

    Mat M(0,3,CV_32F);
    M.push_back(u);
    M.push_back(v);
    M.push_back(n);
    M = M.t();
    M.push_back(Mat((Mat_<float>(1,3) << -e.dot(u), -e.dot(v), -e.dot(n))));
    M = M.t();
    cout << M << endl;
    try {
        //M.push_back((Mat_<float>(1, 4) << 0, 0, 0, 1));    // fails
        //M.push_back(Vec4f(0, 0, 0, 1));                    // fails
        M.push_back(Mat((Mat_<float>(1, 4) << 0, 0, 0, 1))); // works
    }
    catch (cv::Exception e) {
        cout << e.what() << endl;
    }

    cout << "M:\n" << M << endl;

    Mat Mv = M;

    cout << "Mv:\n" << Mv << endl;

    float viewingAngle = 60.;
    float aspectRatio = 1;
    float N = 5.;
    float F = 25.;
    float t = N * tan(CV_PI / 180 * (viewingAngle / 2)); // top
    float b = -t; // bottom
    float r = aspectRatio*t; // right
    float l = -r; // left

    printf("N: %f, F: %f, t: %f, b: %f, r: %f, l: %f\n", N, F, t,b,r,l);

    Mat S1T1Mp = (Mat_<float>(4, 4) <<
                  (2 * N) / (r - l), 0, (r + l) / (r-l), 0,
                  0, (2 * N) / (t - b), (t + b) / (t-b), 0,
                  0, 0, -(F + N) / (F - N), -2 * F * N / (F-N),
                  0, 0, -1, 0
                  );

    cout << "S1T1Mp:\n" << S1T1Mp << endl;

    int w = 512, h = 512;

    Mat WS2T2 = (Mat_<float>(4, 4) <<
                 w/2, 0, 0, w/2,
                 0, -h/2, 0, -h/2+h,
                 0,0,1,0,
                 0,0,0,1
                 );

    cout << "WS2T2:\n" << WS2T2 << endl;

    Mat Pw = (Mat_<float>(4, 1) << 0, 0, 1, 1);
    Mat Ew = (Mat_<float>(4, 1) << 15,15,10,1);
    //Mat Pw2 = (Mat_<float>(4, 1) << 5, 5, 5, 1);

    cout << Pw << endl;

    Mat Ps = WS2T2 * (S1T1Mp * (Mv * Pw));

    cout << "Mv*Pw:\n" << Mv*Pw << endl;
    cout << "(S1T1Mp * (Mv * Pw):\n" << S1T1Mp * (Mv * Pw) << endl;

    cout << "Ps:\n" << Ps << endl;

    Mat Mp = (Mat_<float>(4, 4) <<
              N, 0, 0, 0,
              0, N, 0, 0,
              0, 0, -1.0*(F+N)/(F-N), -2.0*(F*N)/(F-N),
              0, 0, -1, 0);

    Mat Pv = Mp * (Mv * Pw);

    cout << "Gaze point in cam coords:\n" << Mv * Pw << endl;
    cout << "center of proj cam coords:\n" << Mv * Ew << endl;
    cout << "The gaze point after persp. trans.:\n" << Pv << endl;
    
    //float persp = Pv.at<float>(3, 0);
    //Pv.at<float>(0, 0) /= persp;
    //Pv.at<float>(1, 0) /= persp;
    //Pv.at<float>(2, 0) /= persp;
    //Pv.at<float>(3, 0) /= persp;

    Pv /= Pv.at<float>(3, 0);

    cout << "The gaze point after persp, proj. is:\n" << Pv << endl;

    cout << "Mp:\n" << Mp << endl;
    cout << "Mv:\n" << Mv << endl;
    cout << "Pw:\n" << Pw << endl;

    Mat tr = (Mat_<float>(4, 1) <<
              t, r, -N, 1.0
              );


    printf("\n Before perspective transformation, top right corner of near plane is:\n");
    cout << tr << endl;

    printf("\n After perspective transformation, the top right corner of the near plane is:\n");
    cout << S1T1Mp * tr << endl;

    tr = S1T1Mp * tr;

    printf("\n After perspective division, the top right corner of the near plane is:\n");

    //persp = tr.at<float>(3, 0);
    //tr.at<float>(0, 0) /= persp;
    //tr.at<float>(1, 0) /= persp;
    //tr.at<float>(2, 0) /= persp;
    //tr.at<float>(3, 0) /= persp;

    tr /= tr.at<float>(3, 0);

    cout << tr << endl;

    printf("\n Top right corner of near plane in pixel coordinates is\n");

    cout << WS2T2 * tr << endl;

    Mat tt = (Mat_<float>(4, 1) <<
              t, r, -N, 1.0
              );

    int64 t0 = getTickCount();
    for (int i = 0; i < 1000000; i++) {
        tt = (WS2T2 * ((S1T1Mp * tt)));
    }
    cout << "time: " << (getTickCount() - t0)/getTickFrequency() << endl;

    cout << tt << endl;
    cout << tt/tt.at<float>(3, 0) << endl;

    //PolygonalMesh p;
    //p.readFromFile("PolyMesh.xml");

    // chillout until the user has hit a key
    getchar();
    waitKey();
}