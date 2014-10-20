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
    Mat e = (Mat_<float>(1,3) << 30., 30., 15.); // camera vector.  15, 15, 10
    Mat g = (Mat_<float>(1,3) <<  0.,  0.,  18.); // a point through which the gaze direction unit vector n points to
    Mat p = (Mat_<float>(1,3) <<  0.,  0.,  1.); // x, y, z, w

    Mat n;
    normalize(e - g, n);

    Mat u, v;
    u = p.cross(n);
    v = u.cross(n);
    //u = abs(u);
    //v = abs(v);

    Mat Mv(0, 3, CV_32F);
    Mv.push_back(u);
    Mv.push_back(v);
    Mv.push_back(n);
    Mv = Mv.t();
    Mv.push_back(Mat((Mat_<float>(1, 3) << -e.dot(u), -e.dot(v), -e.dot(n))));
    Mv = Mv.t();
    Mv.push_back(Mat((Mat_<float>(1, 4) << 0, 0, 0, 1))); // works

    float viewingAngle = 60.;
    float aspectRatio = 1;
    float N = 5.;
    float F = 30.;
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

    int flip = 1;

    Mat WS2T2 = (Mat_<float>(4, 4) <<
                 w/2, 0,   0,  w/2,
                 0,  flip*h/2, 0, -h/2+h,
                 0,   0,   1,  0,
                 0,   0,   0,  1
                 );

    // take point in world coords and transform to screen coords
    Mat tt = (Mat_<float>(4, 1) << t, r, -N, 1.0);
    Mat tf = (Mat_<float>(4, 1) << t, r, N, 1.0);

    tt = WS2T2 * (S1T1Mp * tt);

    cout << tt << endl;
    cout << tt/tt.at<float>(3, 0) << endl;

    Mat screen(512, 512, CV_8U);
    screen.setTo(Scalar(255, 255, 255));

    PolygonalMesh poly;
    poly.readFromFile("PolyVase.xml");

    namedWindow("s", CV_WINDOW_AUTOSIZE);
    imshow("s", screen);

    vector<Point2i> coords;
    coords.reserve(poly.vertsH.size());
    for (int i = 0; i < poly.vertsH.size(); i++) {
        Mat pt = WS2T2 * (S1T1Mp * (Mv * poly.vertsH[i]));
        pt /= pt.at<float>(3, 0);
        coords.push_back(Point2f((int)pt.at<float>(0), (int)pt.at<float>(1)));
        //cout << coords.back() << endl;
        //screen.at<uchar>(coords.back()) = 0;
    }

    //for (int i = 0; i < poly.norms.size(); i++) {
    //    Normal nor = poly.norms[i];
    //    Mat n4 = (Mat_<float>(4, 1) << nor.x, nor.y, nor.z, 0);
    //    n4 = WS2T2 * (S1T1Mp * (Mv * n4));
    //    n4 /= n4.at<float>(3);
    //    //cout << n4 << endl;
    //    poly.norms[i] = Normal(n4.at<float>(0), n4.at<float>(1), n4.at<float>(2));
    //}

    for (int i = 0; i < poly.faces.size(); i++) {
        Normal faceNormal = poly.norms[poly.faces[i].data[3]];
        Mat tv = poly.vertsH[ poly.faces[i].data[0] ]; //  triangle 1st vertex
        Vec3f  camToTri(tv.at<float>(0) - e.at<float>(0),
                        tv.at<float>(1) - e.at<float>(1),
                        tv.at<float>(2) - e.at<float>(2));
        //cout << "fn:\n" << faceNormal << endl;
        //cout << "tv:\n" << tv << endl;
        //cout << "t:\n" << t << endl;
        float b = faceNormal.dot(camToTri); // camToTri.dot(faceNormal);
        // faceNormal.dot(nn);
        //cout << a << endl;
        //cout << b << endl;
        if (b >= 0) {
            line(screen, coords[poly.faces[i].data[0]], coords[poly.faces[i].data[1]], Scalar(0, 0, 0));
            line(screen, coords[poly.faces[i].data[0]], coords[poly.faces[i].data[2]], Scalar(0, 0, 0));
            line(screen, coords[poly.faces[i].data[1]], coords[poly.faces[i].data[2]], Scalar(0, 0, 0));
        }
    }

    imshow("s", screen);

    // chillout until the user has hit a key
    waitKey();
    getchar();
}