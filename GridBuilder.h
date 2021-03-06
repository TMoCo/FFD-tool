#ifndef _DEFORM_H
#define _DEFORM_H

#include <vector>

#include "Vector.h"

enum struct Grid 
{
    Bilinear, Barycentric, Trilinear
};

// Grid builder class containts all data relating the 
// deformable grid
class GridBuilder
{
    public:

    GridBuilder();

    // generate grid depending on grid type
    void generateGrid(float modelSize);
    // draw a grid depending on grid type
    void drawGrid();

    // grid data 
    int _gridSize;
    Grid _gridType;
    std::vector<Vector> _grid;
    std::vector<Vector> _triangulationMesh;

    // update the grid when dragging the mouse in deform widget
    void moveVertex(Vector move, int index, bool attenuation, int attenuationScale);
    void updateGrid(Vector move, int index);

    // Bilinear methods
    void draw2DGrid();
    void generateRegular2DGrid(float modelSize);
    // Barycentric methods
    void drawTriangularGrid();
    void generateTriangularGrid(float modelSize);
    // Trilinear methods
    void draw3DGrid();
    void generateRegular3DGrid(float modelSize);

    void setGridSize(int size);
    void setGridType(Grid gridType);
    void setGridVector(int index, Vector vertex);

    Grid getGridType();
    int getGridSize();
    const Vector& getGridVertex(int index) const;
};

#endif