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
    PolygoanlMesh();
    std::vector<Mat> vertices;
    std::vector<Face> faces;
    std::vector<Normal> normals;

    void writeToFile(ofstream& of);
};

#endif