#include <GL/gl.h>
#include <GL/glu.h>

#include "Bilinear.h"

// concstructor
Bilinear::Bilinear(int size, float modelSize) : Deform(size, modelSize)
{
    _grid.resize(0.0);
}

// generate a regular 2D grid depending on a loaded
void Bilinear::generate2DGrid(int size)
{
    // initialise grid vertices
    _grid.resize(size * size);    
    // start at -length/2
    Vector startPos = Vector(-_boundingSphereSize/2.0, _boundingSphereSize/2.0, 0.0);
    // y loop
    for(int row = 0; row < size; row++)
    {
        // x loop
        for(int col = 0; col < size; col++)
        {
            _grid[row * size + col] = startPos + Vector(col * _boundingSphereSize / (float)(size-1), row * -_boundingSphereSize / (float)(size-1), 0.0);
        }
    }
}

// draw a regular 2D grid
void Bilinear::draw2DGrid()
{
    // draw all x lines
    // y loop
    for(int row = 0; row < _gridSize; row++)
    {   
        // x loop
        for(int col = 0; col < _gridSize - 1; col++)
        {
            glBegin(GL_LINES);
            glVertex3f(_grid[row * _gridSize + col    ].x, _grid[row * _gridSize + col    ].y, _grid[row * _gridSize + col    ].z);
            glVertex3f(_grid[row * _gridSize + col + 1].x, _grid[row * _gridSize + col + 1].y, _grid[row * _gridSize + col + 1].z);
            glEnd();
        }
    }

    // draw all y lines
    // y loop
    for(int row = 0; row < _gridSize - 1; row++)
    {   
        // x loop
        for(int col = 0; col < _gridSize; col++)
        {
            glBegin(GL_LINES);
            glVertex3f(_grid[row       * _gridSize + col].x, _grid[row       * _gridSize + col].y, _grid[row       * _gridSize + col].z);
            glVertex3f(_grid[(row + 1) * _gridSize + col].x, _grid[(row + 1) * _gridSize + col].y, _grid[(row + 1) * _gridSize + col].z);
            glEnd();
        }
    }
}