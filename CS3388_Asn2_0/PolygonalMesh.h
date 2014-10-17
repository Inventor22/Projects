#ifndef POLYGONAL_MESH_H
#define POLYGONAL_MESH_H

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "Face.h"
#include "Normal.h"

class PolygonalMesh {
public:
    PolygonalMesh() {}

    std::vector<cv::Mat> vertsH;
    std::vector<cv::Mat> vertsC;
    std::vector<Face>    faces;
    std::vector<Normal>  norms;

    void generateCartesianCoords() {
        try {
            // failing right now. http://stackoverflow.com/questions/23290000/assertion-failed-convertpointsfromhomogenous 
            cv::convertPointsFromHomogeneous(vertsH, vertsC);
        }
        catch (cv::Exception e) {
            std::cout << e.what() << std::endl;
        }
    }

    void writeToFile(std::string s) {
        cv::FileStorage fs(s, cv::FileStorage::WRITE);

        fs << "vertices" << "[";
        for (int i = 0; i < vertsH.size(); ++i) {
            // Fill each node with data
            fs << "{:" << "v" << vertsH.at(i) << "}";
        }
        fs << "]";

        fs << "faces" << "[";
        for (int i = 0; i < faces.size(); i++) {
            Face* f = &faces.at(i);
            fs << "{:" << "v0" << f->vertices[0] << "v1" << f->vertices[1] << "v2" << f->vertices[2] << "n" << f->normal;
            fs << "}";
        }
        fs << "]";

        fs << "norms" << "[";
        for (int i = 0; i < norms.size(); i++) {
            fs << "{:" << "n" << norms.at(i) << "}";
        }
        fs << "]";

        //fs <<
        //    "vertices" << vertsH <<
        //    "faces" << faces <<
        //    "norms" << norms;

        fs.release();


        //of << vertsH.size() << std::endl;
        //for (int i = 0; i < vertsH.size(); i++) {
        //    of << vertsH.at(i).t() << std::endl;
        //}
        //of << faces.size() << std::endl;
        //for (int i = 0; i < faces.size(); i++) {
        //    of <<
        //        faces.at(i).vertices[0] << " " <<
        //        faces.at(i).vertices[1] << " " <<
        //        faces.at(i).vertices[2] << " " <<
        //        faces.at(i).normal << std::endl;
        //}
        //for (int i = 0; i < norms.size(); i++) {
        //    of << norms.at(i) << std::endl;
        //}
    }

    void readFromFile(std::string s) {
        cv::FileStorage fs2;
        try {
            fs2 = cv::FileStorage(s, cv::FileStorage::READ);
        }
        catch (cv::Exception e) {
            std::cout << e.what() << std::endl;
        }

        cv::FileNode verts = fs2["verts"];
        cv::FileNodeIterator it = verts.begin(), it_end = verts.end();
        // iterate through a sequence using FileNodeIterator
        for (; it != it_end; ++it) {
            (*it)["v"] >> vertsH;
        }

        cv::FileNode fcs = fs2["faces"];
        it = fcs.begin(), it_end = fcs.end();
        // iterate through a sequence using FileNodeIterator
        for (; it != it_end; ++it) {
            Face f;
            (*it)["v0"] >> f.vertices[0];
            (*it)["v1"] >> f.vertices[1];
            (*it)["v2"] >> f.vertices[2];
            (*it)["n"] >> f.normal;
            faces.push_back(f);
        }

        cv::FileNode ns = fs2["faces"];
        it = ns.begin(), it_end = ns.end();
        // iterate through a sequence using FileNodeIterator
        for (; it != it_end; ++it) {
            Normal n;
            (*it)["n"] >> n;
            norms.push_back(n);
        }


        //fs2["vertices"] >> vertsH;
        //fs2["faces"] >> faces;
        //fs2["norms"] >> norms;

        fs2.release();

        //int numVerts, numNormsAndFaces;

        //string s;
        //getline(is, s);
        //istringstream issVerts(s);

        //issVerts >> numVerts;

        //for (int i = 0; i < numVerts; i++) {
        //    getline(is, s);
        //    istringstream iss(s);
        //    iss >> cv::Mat;
        //}



        ////read a line into 's' from 'fin' each time
        //for (int i = 0; getline(fin, s); i++) {
        //    istringstream sin3(s);
        //    double x, y, z, h;
        //    while (sin3 >> x >> y >> z >> h) {
        //        p.vertsH.push_back((Mat_<float>(4, 1) << x, y, z, h));
        //    }
        //    cout << p.vertsH.back() << endl;
        //}
    }

    void printMesh() {
        for (int i = 0; i < faces.size(); i++) {
            Face f = faces.at(i);
            std::cout << "Face: " << i <<
                "\nP0: " << vertsH.at(f.vertices[0]) <<
                "\nP1: " << vertsH.at(f.vertices[1]) <<
                "\nP2: " << vertsH.at(f.vertices[2]) <<
                "\nNormal: " << norms.at(f.normal) << std::endl << std::endl;
        }
    }
};

#endif