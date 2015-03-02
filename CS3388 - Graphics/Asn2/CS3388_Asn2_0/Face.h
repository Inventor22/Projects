#ifndef FACE_H
#define FACE_H

#include <vector>

class Face {
public:
    Face() {}
    Face(std::vector<int> f) {
        data = f;
    }
    Face(int pt1, int pt2, int pt3, int norm) {
        data = std::vector <int>();
        data.reserve(4);
        data.push_back(pt1);
        data.push_back(pt2);
        data.push_back(pt3);
        data.push_back(norm);
    }

    enum { PT0 = 0, PT1, PT2, NORM };
    std::vector<int> data;
};

#endif