#ifndef FACE_H
#define FACE_H

class Face {
public:
    Face(int pt1, int pt2, int pt3, int norm) {
        vertices[0] = pt1;
        vertices[1] = pt2;
        vertices[2] = pt3;
        normal      = norm;
    }

    int vertices[3];
    int normal;
};

#endif