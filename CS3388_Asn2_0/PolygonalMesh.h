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

    void writeToFile(std::ofstream& of) {
        FileStorage fs("test.yml", FileStorage::WRITE);

        fs << "frameCount" << 5;
        time_t rawtime; time(&rawtime);
        fs << "calibrationDate" << asctime(localtime(&rawtime));
        Mat cameraMatrix = (Mat_<double>(3, 3) << 1000, 0, 320, 0, 1000, 240, 0, 0, 1);
        Mat distCoeffs = (Mat_<double>(5, 1) << 0.1, 0.01, -0.001, 0, 0);
        fs << "cameraMatrix" << cameraMatrix << "distCoeffs" << distCoeffs;


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

    void readFromFile(std::ifstream& is) {
        int numVerts, numNormsAndFaces;

        string s;
        getline(is, s);
        istringstream issVerts(s);

        issVerts >> numVerts;

        for (int i = 0; i < numVerts; i++) {
            getline(is, s);
            istringstream iss(s);
            iss >> cv::Mat;
        }



        //read a line into 's' from 'fin' each time
        for (int i = 0; getline(fin, s); i++) {
            istringstream sin3(s);
            double x, y, z, h;
            while (sin3 >> x >> y >> z >> h) {
                p.vertsH.push_back((Mat_<float>(4, 1) << x, y, z, h));
            }
            cout << p.vertsH.back() << endl;
        }
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