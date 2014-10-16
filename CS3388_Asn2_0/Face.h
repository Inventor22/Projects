#ifndef FACE_H
#define FACE_H

class Face {
public:
    Face() {
        for (int i = 0; i < 3; i++) {
            vertices[i] = normals[i] = 0;
        }
    }

    int vertices[3];
    int normals[3];
};

#endif