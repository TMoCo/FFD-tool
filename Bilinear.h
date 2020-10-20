#ifndef _BILINEAR_H
#define _BILINEAR_H

#include <vector>

#include "Vector.h"
#include "Deform.h"

// inherit from Deform
class Bilinear : public Deform
{
    public:
    
    // constructor, argument is size of 2d grid
    Bilinear(int size, float gridSize);

    private:
    // inherit grid data from Deform
    // grid dimensions
    // grid

    // generate a 2D grid
    void generate2DGrid(int size);
    void draw2DGrid();
};

#endif