/*
Name: Dustin Dobransky
ID:   250575030
Date: 13/10/14

Description:
This program reads a wiremesh from file and renders it
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

enum { X=0, Y, Z, W }; // array indices for point/vector components

Mat getRotationMatrix(Vec3f v, float angle) {
    angle = angle/180*CV_PI;
    Mat Jv = (Mat_<float>(3, 3) <<
                0, -v[Z], v[Y],
              v[Z],   0, -v[X],
             -v[Y], v[X],   0);
    Mat r;
    add(Mat::eye(3, 3, CV_32F), sin(angle)*Jv, r);
    //pow(Jv, 2, Jv);
    add(r , (1.0-cos(angle))*(Jv*Jv), r);
    return r;
}

int main(int argc, char** argv) {

    cout << "Welcome to Le Fancy Vase Drawer.\n" << endl;
    printf("Instructions:\n"\
           "r,g,b - Change colour of mesh to red, green, blue\n"\
           "k     - Colour the mesh black\n"\
           "p     - RAINBOW.\n"\
           "1,3   - Zoom in, zoom out\n"\
           "w,s   - Move camera up, down\n"\
           "a,d   - Rotate mesh left, right\n"\
           "q,e   - Move gaze point up, down\n"\
           "z,c   - Move camera and gaze point up, down\n");

    float viewingAngle = 60.; // degrees
    float aspectRatio = 1;
    float N = 5.;  // near plane
    float F = 30.; // far plane
    float t = N * tan(CV_PI / 360 * viewingAngle); // top
    float b = -t; // bottom
    float r = aspectRatio*t; // right
    float l = -r; // left
    int   w = 512,
          h = 512;

    bool camChanged = true;
    bool rainbow = true;
    int rotationInc = 5;
    int roll = 0;

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

    // container for screen coords
    vector<Point2i> coords;

    // background colour and line colour
    Scalar bgColour(255, 255, 255);
    Scalar lineColour(0);

    // HSV and BGR matrices for colours of the rainbow
    int sat = 200, val = 200;
    Mat hsv(Size(1,1),CV_8UC3, Scalar(10,sat,val)), bgr;

    // the polygonal mesh object
    PolygonalMesh poly;
    poly.readFromFile("PolyVase.xml");

    bool normalChanged = true;

    char c = -1; // input char
    while (true) {
        if (camChanged) {
            if (normalChanged) {
                // normalize vector from camera to gaze point
                normalize(e - g, n);
                // generate vectors describing camera plane
                //u = (getRotationMatrix(n, rotationInc) * u.t()).t();
                //v = (getRotationMatrix(n, rotationInc) * v.t()).t();
                // normalize to keep window same size
                //normalize(u, u);
                //normalize(v, v);
                u = p.cross(n);
                v = u.cross(n);
                u = (getRotationMatrix(n, roll) * u.t()).t();
                v = (getRotationMatrix(n, roll) * v.t()).t();
                normalize(u, u);
                normalize(v, v);

                normalChanged = false;
            }

            // construct matrix for world coords to camera viewing coords
            Mv = Mat(0, 3, CV_32FC1);
            Mv.push_back(u);
            Mv.push_back(v);
            Mv.push_back(n);
            Mv = Mv.t();
            Mv.push_back(Mat((Mat_<float>(1, 3) << -e.dot(u), -e.dot(v), -e.dot(n))));
            Mv = Mv.t();
            Mv.push_back(Mat((Mat_<float>(1, 4) << 0, 0, 0, 1))); // works

            //scale, transformation, and projection matrix
            S1T1Mp = (Mat_<float>(4, 4) <<
                          (2*N)/(r-l), 0, (r+l)/(r-l), 0,
                          0, (2*N)/(t-b), (t+b)/(t-b), 0,
                          0, 0, -(F+N)/(F-N), -2*F*N/(F-N),
                          0, 0, -1, 0
                          );

            // scal, transform from viewing volume to canonical viewing volume.
            // Flip along X-axis is desired
            WS2T2 = (Mat_<float>(4, 4) <<
                         w/2, 0, 0, w/2,
                         0, flip*h/2, 0, -h/2+h,
                         0, 0, 1, 0,
                         0, 0, 0, 1
                         );

            // colour the background
            screen.setTo(bgColour);

            camChanged = false;
        }
        
        coords.clear();
        coords.reserve(poly.vertsH.size());
        for (int i = 0; i < poly.vertsH.size(); i++) {
            // Apply transformations to convert from world coordinates to screen coords
            Mat pt = WS2T2 * (S1T1Mp * (Mv * poly.vertsH[i]));
            // Perspective divide
            pt /= pt.at<float>(3, 0);
            // store generated coordinate in coords container
            coords.push_back(Point2f((int)pt.at<float>(0), (int)pt.at<float>(1)));
        }
        if (rainbow) {
            hsv = Mat(Size(1, 1), CV_8UC3, Scalar(10, sat, val));
        }
        for (int i = 0; i < poly.faces.size(); i++) {
            Face& f           = poly.faces[i];
            Normal faceNormal = poly.norms[f.data[Face::NORM]];
            Mat tv            = poly.vertsH[f.data[Face::PT0]]; //  triangle 1st vertex
            // generate camera to triangle vector
            Vec3f  camToTri(tv.at<float>(X) - e.at<float>(X),
                            tv.at<float>(Y) - e.at<float>(Y),
                            tv.at<float>(Z) - e.at<float>(Z));

            // compute dot product to determine which faces to draw.
            float b = faceNormal.dot(camToTri); // camToTri.dot(faceNormal);

            if (rainbow) {
                // increment hue value, then convert from HSV to BGR
                Vec3b clr = hsv.at<Vec3b>(0, 0);
                clr[0]++;
                hsv.at<Vec3b>(0, 0) = clr;
                cvtColor(hsv, bgr, CV_HSV2BGR);
                Vec3b bgr3 = bgr.at<Vec3b>(0, 0);
                lineColour = Scalar(bgr3[0], bgr3[1], bgr3[2]);
            }
            if (b >= 0) {
                // draw triangle from 3 face coords
                line(screen, coords[f.data[Face::PT0]], coords[f.data[Face::PT1]], lineColour);
                line(screen, coords[f.data[Face::PT0]], coords[f.data[Face::PT2]], lineColour);
                line(screen, coords[f.data[Face::PT1]], coords[f.data[Face::PT2]], lineColour);
            }
        }

        // draw the image to the screen
        imshow(wndName, screen);

        c = waitKey(0);
        switch (c) {
            case 'w':
                // move camera up
                e.at<float>(Z) += 1;
                camChanged = true;
                normalChanged = true;
                break;
            case 's':
                // move camera down
                e.at<float>(Z) -= 1;
                camChanged = true;
                normalChanged = true;
                break;
            case 'a':{
                // rotate by 5 degrees along z axis
                float a = rotationInc/180.*CV_PI;
                e = ((Mat_<float>(3, 3) <<
                    cos(a), sin(a), 0,
                    -sin(a), cos(a), 0,
                    0, 0, 1) * e.t()).t();
                camChanged = true;
                normalChanged = true;
            }
                break;
            case 'd':{
                // rotate by -5 degrees along z axis
                float a = -rotationInc/180.*CV_PI;
                e = ((Mat_<float>(3, 3) <<
                    cos(a), sin(a), 0,
                    -sin(a), cos(a), 0,
                    0, 0, 1) * e.t()).t();
                camChanged = true;
                normalChanged = true;
            }
                break;
            case 'y': {
                roll += rotationInc;
                // rotate camera
                // rotate around unit vector 'n' -- from gaze point to cam
                //u = (getRotationMatrix(n, rotationInc) * u.t()).t();
                //v = (getRotationMatrix(n, rotationInc) * v.t()).t();
                // normalize to keep window same size
                //normalize(u, u);
                //normalize(v, v);
                normalChanged = true;
                camChanged = true;
            }
                break;
            case 'u':
                roll -= rotationInc;
                //u = (getRotationMatrix(n, -rotationInc) * u.t()).t();
                //v = (getRotationMatrix(n, -rotationInc) * v.t()).t();
                //normalize(u, u);
                //normalize(v, v);
                normalChanged = true;
                camChanged = true;
                break;
            case 'q':
                // shift gaze vector down
                g.at<float>(Z) -= 1;
                camChanged = true;
                normalChanged = true;
                break;
            case 'e':
                // shift gaze vector up
                g.at<float>(Z) += 1;
                camChanged = true;
                normalChanged = true;
                break;
            case 'c':
                // move camera and gaze vector up
                g.at<float>(Z) += 1;
                e.at<float>(Z) += 1;
                camChanged = true;
                break;
            case 'z':
                // move camera and gaze vector down
                g.at<float>(Z) -= 1;
                e.at<float>(Z) -= 1;
                camChanged = true;
                break;
            case '1':
                // move camera closer to object by 10%
                e *= 0.9;
                camChanged = true;
                break;
            case '3':
                // move cam further from object by 10%
                e *= 1.1;
                camChanged = true;
                break;
            case 'r':
                // colour the mesh red, green, blue, black (next 4 cases)
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
                //set the rainbow flag
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