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

Mat getRotationMatrix(char axis, double a) {
    a = a / 180 * CV_PI; // convert from degrees to radians
    switch (axis) {
        case 'x':
            return (Mat_<float>(4, 4) <<
                    1., 0., 0., 0.,
                    0., cos(a), -sin(a), 0.,
                    0., sin(a), cos(a), 0.,
                    0., 0., 0., 1.);
            break;
        case 'y':
            return (Mat_<float>(4, 4) <<
                    cos(a), 0, sin(a), 0,
                    0, 1, 0, 0,
                    -sin(a), 0, cos(a), 0,
                    0, 0, 0, 1);
            break;
        case 'z':
            return (Mat_<float>(4, 4) <<
                    cos(a), -sin(a), 0, 0,
                    sin(a), cos(a), 0, 0,
                    0, 0, 1, 0,
                    0, 0, 0, 1);
        default:
            return Mat::eye(Size(4, 4), CV_8U);
    }
}

int main(int argc, char** argv) {

    // create polygonal mesh object;
    PolygonalMesh p;

    // open profile points file
    ifstream fin("vase.txt");
    string s;

    // read in rotation axis
    getline(fin, s);
    istringstream sin(s);
    char axis;
    sin >> axis;

    // read angle increment
    getline(fin, s);
    istringstream sin2(s);
    double angleInc = 1;
    sin2 >> angleInc;

    //read a line into 's' from 'fin' until EOF and store vertices
    for (int i = 0; getline(fin, s); i++) {
        istringstream sin3(s);
        double x, y, z, h;
        while (sin3 >> x >> y >> z >> h) {
           p.vertsH.push_back((Mat_<float>(4,1) << x, y, z, h));
        }
    }

    int profileVerticesSize = (int) p.vertsH.size();

    int rotations = (int) (360.0 / angleInc);

    for (double i = 1; i < rotations; i++) {
        // generate rotation matrix according to angle
        Mat rotationMatrix = getRotationMatrix(axis, angleInc * i);
        for (int j = 0; j < profileVerticesSize; j++) {
            // rotate original points according to rotation matrix
            p.vertsH.push_back(rotationMatrix * p.vertsH.at(j));
        }
    }

    /*
    Generate faces and normals.
    a a---c
    |\ \ 2|
    | \ \ |
    |1 \ \|
    b---d d
    */
    for (int i = 0; i < rotations; i++) {
        for (int j = 0; j < profileVerticesSize-1; j++) {
            // retrieve indices of coords, according to diagram above
            int indexA =   i                 * profileVerticesSize + j;
            int indexB =   i                 * profileVerticesSize + j + 1;
            int indexC = ((i+1)%(rotations)) * profileVerticesSize + j;
            int indexD = ((i+1)%(rotations)) * profileVerticesSize + j + 1;

            // retrieve homogeneous vertices
            Mat ha = p.vertsH.at(indexA);
            Mat hb = p.vertsH.at(indexB);
            Mat hc = p.vertsH.at(indexC);
            Mat hd = p.vertsH.at(indexD);

            // remove homogeneous point (last point), to make 3-vector
            ha.pop_back();
            hb.pop_back();
            hc.pop_back();
            hd.pop_back();

            // store as 3d points with float precision
            Point3f a(ha);
            Point3f b(hb);
            Point3f c(hc);
            Point3f d(hd);

            // (p2-p1)x(p3-p2)
            // a = p1, b = p2, d = p3
            // a = p1, d = p2, c = p3
            //Normal n1 = (a - b).cross(a - d);
            //Normal n2 = (a - c).cross(a - d);
            //Normal n1 = (b - a).cross(d - a);
            //Normal n2 = (c-a).cross(d-a);
            // compute normals 
            Normal n1 = (b - a).cross(d - b);
            Normal n2 = (d - a).cross(c - d);

            // store normals and faces
            p.norms.push_back(n1);
            p.norms.push_back(n2);

            p.faces.push_back(Face(indexA, indexB, indexD, (int)p.norms.size() - 2)); // face 1
            p.faces.push_back(Face(indexA, indexD, indexC, (int)p.norms.size() - 1)); // face 2
        }
    }

    cout << "Model generated" << endl;

    string fileName = "PolyVase.xml";

    p.writeToFile(fileName);

    cout << "Model written to file: " << fileName << endl;
    cout << "Press <Enter> to exit" << endl;

    // chillout until the user has hit a key
    getchar();
    waitKey();

    return 0;
}