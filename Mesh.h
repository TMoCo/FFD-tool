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
    // method for saving the deformed mesh data 
    void saveMesh(std::string fileName, GridBuilder* gridBuilder);

    // draw mesh functions
    void drawMesh(GridBuilder* gridBuilder);
    // method for getting the right vertex weights depending on grid type
    void getVertexWeights(GridBuilder* gridBuilder);
    
    // bilinear
    void getBilinearWeights(int gridSize);
    void drawBilinearMesh(std::vector<Vector>& gridVertices, int gridSize);
    Vector deformBilinear(int vertex, std::vector<Vector>& gridVertices, int gridSize);

    // barycentric
    void getBarycentricWeights(std::vector<Vector>& triangulationMesh);
    void drawBarycentricMesh(std::vector<Vector>& triangulationMesh);
    Vector deformBarycentric(int vertex, std::vector<Vector>& triangulationMesh);

    // trilinear
    void getTrilinearWeights(int gridSize);
    void drawTrilinearMesh(std::vector<Vector>& gridVertices, int gridSize);
    Vector deformTrilinear(int vertex, std::vector<Vector>& gridVertices, int gridSize);

    // draw mesh as it was on load
    void drawFileMesh();
    // check for whether a mesh has been loaded 
    bool isEmpty();
    float getModelSize();


    private:
    // Mesh Data
    std::vector<Vector> _meshVertices;
    std::vector<Vector> _weights;
    std::vector<Vector> _faces;

    //std::string

    Vector _meshMidPoint;
    float _modelSize;
};

#endif