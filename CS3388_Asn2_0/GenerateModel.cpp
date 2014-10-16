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
#include "PolygonalMesh.h"

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

    // create polygonal mesh object;
    PolygonalMesh p;

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

    //read a line into 's' from 'fin' each time
    for (int i = 0; getline(fin, s); i++) {
        istringstream sin3(s);
        double x, y, z, h;
        while (sin3 >> x >> y >> z >> h) {
           p.vertices.push_back((Mat_<double>(4,1) << x, y, z, h));
        }
        cout << p.vertices.back() << endl;
    }

    int profileVerticesSize = p.vertices.size();
    int rotations = 360.0 / angleInc;

    for (double i = 1; i <= rotations; i++)
    {
        Mat rotationMatrix = getRotationMatrix(axis, angleInc * i);
        for (int j = 0; j < profileVerticesSize; j++)
        {
            p.vertices.push_back(rotationMatrix * p.vertices.at(j)); // get the original points
            cout << p.vertices.back() << endl;
        }
    }

    for (int i = 0; i < rotations; i++) {
        for (int j = 0; j < profileVerticesSize-1; j++) {
            Point3d a = p.vertices.at(i * rotations     + j);
            Point3d b = p.vertices.at(i * rotations     + j + 1);
            Point3d c = p.vertices.at(i * (rotations+1) + j + 1);

            normalize(a);

            Normal na = (a - b).cross(a - c);
        }
    }

    // chillout until the user has hit a key
    getchar();
    waitKey();

    return 0;
}