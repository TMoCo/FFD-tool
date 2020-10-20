#ifndef _MESH_H
#define _MESH_H

#include <string>
#include <vector>

#include "Vector.h"
#include "GridBuilder.h"

class Mesh
{
    public:

    // constructor
    Mesh();

    // method for loading mesh
    void loadMesh(std::string fileName);
    // draw mesh functions
    void drawMesh(GridBuilder* gridBuilder);
    // method for getting the right vertex weights depending on grid type
    void getVertexWeights(GridBuilder* gridBuilder);
    
    // bilinear
    void getBilinearWeights(int gridSize);
    void drawBilinearMesh(std::vector<Vector>& gridVertices, int gridSize);
    Vector calcBilinear(float u, float v, Vector p00, Vector p10, Vector p01, Vector p11);

    // barycentric
    void getBarycentricWeights(std::vector<Vector>& triangulationMesh);
    void drawBarycentricMesh(std::vector<Vector>& triangulationMesh);


    void drawFileMesh();
    // check for whether a mesh has been loaded 
    bool isEmpty();
    float getModelSize();


    private:
    // Mesh Data
    std::vector<Vector> _meshVertices;
    std::vector<Vector> _weights;
    std::vector<Vector> _faces;

    Vector _meshMidPoint;
    float _modelSize;
};

#endif