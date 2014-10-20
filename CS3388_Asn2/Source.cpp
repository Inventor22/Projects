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

static const char* wndName = "Le Fancy Vase";

int main(int argc, char** argv) {

    cout << "Welcome to Le Fancy Vase Drawer.\n" << endl;
    printf("Instructions:\n"\
           "r,g,b - Change colour of mesh to red, green, blue\n"\
           "k     - Colour the mesh black\n"\
           "p     - RAINBOW.\n"\
           "1,3   - Zoom in, zoom out\n"\
           "w,s   - Move camera up, down\n"\
           "a,d   - Rotate camera left, right\n"\
           "q,e   - Move gaze point up, down\n"\
           "z,c   - Move camera and gaze point up, down\n"\
           "\nPress <Enter> to begin.");

    getchar();

    float viewingAngle = 60.;
    float aspectRatio = 1;
    float N = 5.;
    float F = 30.;
    float t = N * tan(CV_PI / 360 * viewingAngle); // top
    float b = -t; // bottom
    float r = aspectRatio*t; // right
    float l = -r; // left
    int   w = 512,
          h = 512;

    bool camChanged = true;
    bool rainbow = true;

    namedWindow(wndName, CV_WINDOW_AUTOSIZE);

    // used as row vectors, so they can be appended to Matrix easily
    Mat e = (Mat_<float>(1, 3) << 30., 30., 22.); // camera vector.  15, 15, 10
    Mat g = (Mat_<float>(1, 3) <<  0., 0., 18.); // a point through which the gaze direction unit vector n points to
    Mat p = (Mat_<float>(1, 3) <<  0., 0., 1.); // x, y, z, w
    Mat n, u, v;
    Mat screen(w, h, CV_8UC3);
    int flip = 1; // -1 to flip along X

    Mat Mv(0, 3, CV_32FC1);

    Mat S1T1Mp = (Mat_<float>(4, 4) <<
                  (2*N)/(r-l), 0, (r+l)/(r-l), 0,
                  0, (2*N)/(t-b), (t+b)/(t-b), 0,
                  0, 0, -(F+N)/(F-N), -2*F*N/(F-N),
                  0, 0, -1, 0
                  );

    Mat WS2T2 = (Mat_<float>(4, 4) <<
                 w/2, 0, 0, w/2,
                 0, flip*h/2, 0, -h/2+h,
                 0, 0, 1, 0,
                 0, 0, 0, 1
                 );

    Scalar bgColour(255, 255, 255);
    Scalar lineColour(0);

    int sat = 200, val = 200;
    Mat    hsv(Size(1,1),CV_8UC3, Scalar(10,sat,val)), bgr;

    PolygonalMesh poly;
    poly.readFromFile("PolyVase.xml");

    char c = -1;
    while (true) {
        int64 t0 = getTickCount();

        if (camChanged) {
            normalize(e - g, n);
            u = p.cross(n);
            v = u.cross(n);
            //u = abs(u);
            //v = abs(v);

            Mv = Mat(0, 3, CV_32FC1);
            Mv.push_back(u);
            Mv.push_back(v);
            Mv.push_back(n);
            Mv = Mv.t();
            Mv.push_back(Mat((Mat_<float>(1, 3) << -e.dot(u), -e.dot(v), -e.dot(n))));
            Mv = Mv.t();
            Mv.push_back(Mat((Mat_<float>(1, 4) << 0, 0, 0, 1))); // works

            S1T1Mp = (Mat_<float>(4, 4) <<
                          (2*N)/(r-l), 0, (r+l)/(r-l), 0,
                          0, (2*N)/(t-b), (t+b)/(t-b), 0,
                          0, 0, -(F+N)/(F-N), -2*F*N/(F-N),
                          0, 0, -1, 0
                          );

            WS2T2 = (Mat_<float>(4, 4) <<
                         w/2, 0, 0, w/2,
                         0, flip*h/2, 0, -h/2+h,
                         0, 0, 1, 0,
                         0, 0, 0, 1
                         );

            screen.setTo(bgColour);

            camChanged = false;
        }
           
        vector<Point2i> coords;
        coords.reserve(poly.vertsH.size());
        for (int i = 0; i < poly.vertsH.size(); i++) {
            Mat pt = WS2T2 * (S1T1Mp * (Mv * poly.vertsH[i]));
            pt /= pt.at<float>(3, 0);
            coords.push_back(Point2f((int)pt.at<float>(0), (int)pt.at<float>(1)));
        }

        //for (int i = 0; i < poly.norms.size(); i++) {
        //    Normal nor = poly.norms[i];
        //    Mat n4 = (Mat_<float>(4, 1) << nor.x, nor.y, nor.z, 0);
        //    n4 = Mv * n4;
        //    //n4 = WS2T2 * (S1T1Mp * (Mv * n4));
        //    n4 /= n4.at<float>(3);
        //    //cout << n4 << endl;
        //    poly.norms[i] = Normal(n4.at<float>(0), n4.at<float>(1), n4.at<float>(2));
        //}

        if (rainbow) {
            hsv = Mat(Size(1, 1), CV_8UC3, Scalar(10, sat, val));
        }
        for (int i = 0; i < poly.faces.size(); i++) {
            Normal faceNormal = poly.norms[poly.faces[i].data[3]];
            Mat tv = poly.vertsH[poly.faces[i].data[0]]; //  triangle 1st vertex
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
                //lineColour = Scalar(rand.uniform(0, 255), rand.uniform(0, 255), rand.uniform(0, 255));
                if (rainbow) {
                    Vec3b clr = hsv.at<Vec3b>(0, 0);
                    clr[0]++;
                    hsv.at<Vec3b>(0, 0) = clr;
                    cvtColor(hsv, bgr, CV_HSV2BGR);
                    Vec3b bgr3 = bgr.at<Vec3b>(0, 0);

                    lineColour = Scalar(bgr3[0], bgr3[1], bgr3[2]);
                }

                line(screen, coords[poly.faces[i].data[0]], coords[poly.faces[i].data[1]], lineColour);
                line(screen, coords[poly.faces[i].data[0]], coords[poly.faces[i].data[2]], lineColour);
                line(screen, coords[poly.faces[i].data[1]], coords[poly.faces[i].data[2]], lineColour);
            }
        }

        imshow(wndName, screen);

        cout << (getTickCount() - t0)/cvGetTickFrequency() << endl;

        c = waitKey(0);
        switch (c) {
            case 'w':
                e.at<float>(2) += 1;
                camChanged = true;
                break;
            case 's':
                e.at<float>(2) -= 1;
                camChanged = true;
                break;
            case 'a':
                e = ((Mat_<float>(3, 3) <<
                     cos(5./180.*CV_PI), sin(5./180.*CV_PI), 0,
                     -sin(5./180.*CV_PI), cos(5./180.*CV_PI), 0,
                     0, 0, 1) * e.t()).t();
                camChanged = true;
                break;
            case 'd':
                e = ((Mat_<float>(3, 3) <<
                     cos(-5./180.*CV_PI), sin(-5./180.*CV_PI), 0,
                     -sin(-5./180.*CV_PI), cos(-5./180.*CV_PI), 0,
                     0, 0, 1) * e.t()).t();
                camChanged = true;
                break;
            case 'q':
                g.at<float>(2) -= 1;
                camChanged = true;
                break;
            case 'e':
                g.at<float>(2) += 1;
                camChanged = true;
                break;
            case 'c':
                g.at<float>(2) += 1;
                e.at<float>(2) += 1;
                camChanged = true;
                break;
            case 'z':
                g.at<float>(2) -= 1;
                e.at<float>(2) -= 1;
                camChanged = true;
                break;
            case '1':
                e *= 0.9;
                camChanged = true;
                break;
            case '3':
                e *= 1.1;
                camChanged = true;
                break;
            case 'r':
                lineColour = Scalar(0, 0, 255);
                rainbow = false;
                break;
            case 'g':
                lineColour = Scalar(0, 150, 0);
                rainbow = false;
                break;
            case 'b':
                lineColour = Scalar(255, 0, 0);
                rainbow = false;
                break;
            case 'k':
                lineColour = Scalar(0, 0, 0);
                rainbow = false;
                break;
            case 'p':
                rainbow = true;
                break;
            default:
                break;
        }
        
    }

    // chillout until the user has hit a key
    waitKey();
    getchar();
}