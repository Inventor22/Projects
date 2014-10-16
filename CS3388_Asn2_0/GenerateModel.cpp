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
#include <vector>

using namespace cv;
using namespace std;

#define PI 3.14159265358979323846264338327950

Mat getRotationMatrix(char axis, double a) {
    a = a / 180 * PI; // convert from degrees to radians
    switch (axis) {
        case 'x':
            return (Mat_<double>(4, 4) <<
                    1., 0., 0., 0.,
                    0., cos(a), -sin(a), 0.,
                    0., sin(a), cos(a), 0.,
                    0., 0., 0., 1.);
            break;
        case 'y':
            return (Mat_<double>(4, 4) <<
                    cos(a), 0, sin(a), 0,
                    0, 1, 0, 0,
                    -sin(a), 0, cos(a), 0,
                    0, 0, 0, 1);
            break;
        case 'z':
            return (Mat_<double>(4, 4) <<
                    cos(a), -sin(a), 0, 0,
                    sin(a), cos(a), 0, 0,
                    0, 0, 1, 0,
                    0, 0, 0, 1);
    }
}

int main(int argc, char** argv) {

    // create matrix objects
    vector<Mat> vertices;

    ifstream fin("vase.txt");
    string s;

    getline(fin, s);
    istringstream sin(s);

    char axis;
    sin >> axis;

    getline(fin, s);
    istringstream sin2(s);
    double angleInc = 1;
    sin2 >> angleInc;

    // original points, which all lie on the same plane
    vector<Mat> origPointPlane;

    //read a line into 's' from 'fin' each time
    for (int i = 0; getline(fin, s); i++) {
        istringstream sin3(s);
        double x, y, z, h;
        //Vec4d d;
        while (sin3 >> x >> y >> z >> h) {
            //d = Vec4d(x, y, z, h);
            vertices.push_back((Mat_<double>(4,1) << x, y, z, h));
        }
        //cout << d << endl;
        cout << origPointPlane.back() << endl;
    }

    //Mat a(getRotationMatrix(axis, angleInc));
    //Mat b(origPointPlane.at(0));

    //cout << a << endl;
    //cout << b << endl;

    //cout << a*b << endl;

    for (double angle = angleInc; angle <= 360.0; angle += angleInc) {
        vector<Mat> rotPointPlane; // rotated points, which will also lie on the plane
        Mat rotationMatrix = getRotationMatrix(axis, angle);
        for (int j = 0; j < origPointPlane.size(); j++) {
            rotPointPlane.push_back(rotationMatrix * origPointPlane.at(j)); // get the original points
            cout << rotPointPlane.back() << endl;
        }
        points.push_back(rotPointPlane);
    }

    

    for (int i = 0; i < points.size(); i++) {

    }

    

  /*  for (int i = 0; i < points.size(); i++) {
        cout << getRotationMatrix(axis, angleInc) * points.at(i) << endl;
    }*/

    getchar();

    // chillout until the user has hit a key
    waitKey();

    return 0;
}