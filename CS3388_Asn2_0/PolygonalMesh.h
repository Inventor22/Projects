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

        fs << "vertices" << "[:";
        for (int i = 0; i < vertsH.size(); ++i) {
            fs << "{:" << "v" << vertsH.at(i) << "}";
        }
        fs << "]";

        fs << "faces" << "[:";
        for (int i = 0; i < faces.size(); i++) {
            fs << "{:" << "f" << faces.at(i).data << "}";
        }
        fs << "]";

        fs << "norms" << "[:";
        for (int i = 0; i < norms.size(); i++) {
            fs << "{:" << "n" << norms.at(i) << "}";
        }
        fs << "]";

        fs.release();
    }

    void readFromFile(std::string s) {
        cv::FileStorage fs2;
        try {
            fs2 = cv::FileStorage(s, cv::FileStorage::READ);
        }
        catch (cv::Exception e) {
            std::cout << e.what() << std::endl;
        }

        reset();

        cv::FileNode verts = fs2["vertices"];
        cv::FileNodeIterator it = verts.begin(), it_end = verts.end();
        // iterate through a sequence using FileNodeIterator
        for (; it != it_end; ++it) {
            cv::Mat m;
            (*it)["v"] >> m;
            vertsH.push_back(m);
        }

        cv::FileNode fcs = fs2["faces"];
        it = fcs.begin(), it_end = fcs.end();
        for (; it != it_end; ++it) {
            std::vector<int> data;
            (*it)["f"] >> data;
            faces.push_back(Face(data));
        }

        cv::FileNode ns = fs2["norms"];
        it = ns.begin(), it_end = ns.end();
        for (; it != it_end; ++it) {
            Normal n;
            (*it)["n"] >> n;
            norms.push_back(n);
        }

        fs2.release();

        writeToFile("test.xml");
    }

    void printMesh() {
        for (int i = 0; i < faces.size(); i++) {
            Face f = faces.at(i);
            std::cout << "Face: " << i <<
                "\nP0: " << vertsH.at(f.data.at(Face::PT0)) <<
                "\nP1: " << vertsH.at(f.data.at(Face::PT1)) <<
                "\nP2: " << vertsH.at(f.data.at(Face::PT2)) <<
                "\nNormal: " << norms.at(f.data.at(Face::NORM)) << std::endl << std::endl;
        }
    }

private:
    void reset() {
        vertsH.clear();
        vertsC.clear();
        faces.clear();
        norms.clear();
    }
};

#endif