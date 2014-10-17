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
           p.vertsH.push_back((Mat_<float>(4,1) << x, y, z, h));
        }
        cout << p.vertsH.back() << endl;
    }

    int profileVerticesSize = (int) p.vertsH.size();
    int rotations = (int) (360.0 / angleInc);

    for (double i = 1; i < rotations; i++)
    {
        Mat rotationMatrix = getRotationMatrix(axis, angleInc * i);
        for (int j = 0; j < profileVerticesSize; j++)
        {
            p.vertsH.push_back(rotationMatrix * p.vertsH.at(j)); // get the original points
            cout << p.vertsH.back() << endl;
        }
    }


    // p.generateCartesianCoords();

    /*
    Generate faces and normals.
    a a---c
    |\ \ 2|
    | \ \ |
    |1 \ \|
    b---d d
    */
    for (int i = 0; i < rotations-1; i++) {
        for (int j = 0; j < profileVerticesSize-1; j++) {
            int indexA =  i    * profileVerticesSize + j;
            int indexB =  i    * profileVerticesSize + j + 1;
            int indexC = (i+1) * profileVerticesSize + j;
            int indexD = (i+1) * profileVerticesSize + j + 1;

            Mat ha = p.vertsH.at(indexA);
            Mat hb = p.vertsH.at(indexB);
            Mat hc = p.vertsH.at(indexC);
            Mat hd = p.vertsH.at(indexD);

            ha.pop_back();
            hb.pop_back();
            hc.pop_back();
            hd.pop_back();

            Point3f a(ha);
            Point3f b(hb);
            Point3f c(hc);
            Point3f d(hd);

            Normal n1 = (a - b).cross(a - d);
            Normal n2 = (a - c).cross(a - d);

            p.norms.push_back(n1);
            p.norms.push_back(n2);

            p.faces.push_back(Face(indexA, indexB, indexD, (int)p.norms.size() - 2)); // face 1
            p.faces.push_back(Face(indexA, indexD, indexC, (int)p.norms.size() - 1)); // face 2
        }
    }
    
    p.printMesh();

    ofstream of("PolyVase.txt");

    p.writeToFile(of);

    cout << "finished generating Mesh!" << endl;

    // chillout until the user has hit a key
    getchar();
    waitKey();

    return 0;
}