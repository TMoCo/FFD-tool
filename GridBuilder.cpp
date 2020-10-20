#include <GL/gl.h>
#include <GL/glu.h>

#include <random>
#include <chrono>

#include "GridBuilder.h"

#include "delaunator.hpp"

// default constructor
GridBuilder::GridBuilder()
{
    _gridSize = 2;
    _gridType = Grid::Bilinear;
    _grid.resize(0.0);
    generateGrid(1.0);
}

// getters and setters for the current state of the grid
Grid GridBuilder::getGridType()
{
    return _gridType;
}
int GridBuilder::getGridSize()
{
    return _gridSize;
}
const Vector& GridBuilder::getGridVertex(int index) const
{
    return _grid[index];
}

void GridBuilder::setGridVector(int index, Vector vertex)
{
    _grid[index] = vertex;
}

void GridBuilder::setGridSize(int size)
{
    _gridSize = size;
}
void GridBuilder::setGridType(Grid gridType)
{
    _gridType = gridType;
}

// generate the current grid
void GridBuilder::generateGrid(float modelSize)
{
    switch (_gridType)
    {
        case Grid::Bilinear:
            // generate 2D grid
            generateRegular2DGrid(modelSize);
            break;
        case Grid::Barycentric:
            // generate triangular mesh
            generateTriangularGrid(modelSize);
            break;
        default:
            break;
    }
}

// draw the current grid
void GridBuilder::drawGrid()
{
    switch (_gridType)
    {
    case Grid::Bilinear:
        draw2DGrid();
        break;
    case Grid::Barycentric:
        drawTriangularGrid();
        break;
    default:
        break;
    }
}

void GridBuilder::moveVertex(Vector move, int index, bool attenuation)
{
    if (attenuation)
    {
        Vector min = Vector(0.0, 0.0, 0.0);
        Vector max = Vector(0.0, 0.0, 0.0);
        // loop over each vertex and build two vectors representing the largest and smallest x and y values currently in the grid
        for(unsigned int vertex = 0; vertex < _grid.size(); vertex++)
        {
            if(_grid[vertex].x < min.x)
                min.x = _grid[vertex].x;
            if(_grid[vertex].y < min.y)
                min.y = _grid[vertex].y;
            if(_grid[vertex].x > max.x)
                max.x = _grid[vertex].x;
            if(_grid[vertex].y > max.y)
                max.y = _grid[vertex].y;
        }
        float maxDistance = (min - max).magnitude();
        for(int vertex = 0; vertex < (int)_grid.size(); vertex++)
        {
            if( vertex != index )
            {
                // calculate new weight for given vertex
                float weight = pow(1.0 - ((_grid[index] - _grid[vertex]).magnitude() / maxDistance ), 2);
                // now update the vertex 
                updateGrid(move * weight, vertex);
            }
            else
            {
            // directly move grid vertex
            updateGrid(move, vertex);
            }            
        }
    }
    else
    {
        updateGrid(move, index);
        
    }
}

// update the triangulation mesh for a given vertex
void GridBuilder::updateGrid(Vector move, int index)
{
    switch (_gridType)
        {
        case Grid::Bilinear:
            // directly move grid vertex
            _grid[index].x += move.x;
            _grid[index].y += move.y;
            _grid[index].z += move.z;
            break;

        case Grid::Barycentric:
            // find all occurences of the vertex in the triangulation mesh for when rendering the mesh
            for(unsigned int i = 0; i < _triangulationMesh.size(); i++)
            {
                if((_triangulationMesh[i].x == _grid[index].x) && (_triangulationMesh[i].y == _grid[index].y))
                {
                    _triangulationMesh[i].x += move.x;
                    _triangulationMesh[i].y += move.y;
                    _triangulationMesh[i].z += move.z;
                }
            }
            // update the _grid vertex
            _grid[index].x += move.x;
            _grid[index].y += move.y;
            _grid[index].z += move.z;
            break;
        
        default:
            break;
        }
}
//
// Bilinear 
//

// generate a regular 2D grid depending on a loaded mesh's size
void GridBuilder::generateRegular2DGrid(float modelSize)
{
    // initialise grid vertices
    _grid.resize(_gridSize * _gridSize);    
    // start at -length/2
    Vector startPos = Vector(-modelSize/2.0, modelSize/2.0, 0.0);
    // y loop
    for(int row = 0; row < _gridSize; row++)
    {
        // x loop
        for(int col = 0; col < _gridSize; col++)
        {
            _grid[row * _gridSize + col] = startPos + Vector(col * modelSize / (float)(_gridSize-1), row * -modelSize / (float)(_gridSize-1), 0.0);
        }
    }
}

// draw a regular 2D grid
void GridBuilder::draw2DGrid()
{
    // draw all x lines
    // y loop
    glBegin(GL_LINES);
    for(int row = 0; row < _gridSize; row++)
    {   
        // x loop
        for(int col = 0; col < _gridSize - 1; col++)
        {
            glVertex3f(_grid[row * _gridSize + col    ].x, _grid[row * _gridSize + col    ].y, _grid[row * _gridSize + col    ].z);
            glVertex3f(_grid[row * _gridSize + col + 1].x, _grid[row * _gridSize + col + 1].y, _grid[row * _gridSize + col + 1].z);
        }
    }
    // draw all y lines
    // y loop
    for(int row = 0; row < _gridSize - 1; row++)
    {   
        // x loop
        for(int col = 0; col < _gridSize; col++)
        {
            glVertex3f(_grid[row       * _gridSize + col].x, _grid[row       * _gridSize + col].y, _grid[row       * _gridSize + col].z);
            glVertex3f(_grid[(row + 1) * _gridSize + col].x, _grid[(row + 1) * _gridSize + col].y, _grid[(row + 1) * _gridSize + col].z);
        }
    }
    glEnd();
}

//
// Barycentric
//

// generate a triangular mesh using delaunay triangulation
void GridBuilder::generateTriangularGrid(float modelSize)
{
    // a list of doubles representing 2D positions for the delaunay triangulator
    std::vector<double> vertices;

    // set the number of points to generate
    _grid.resize(_gridSize * _gridSize);
    vertices.resize(_gridSize * _gridSize * 2);

    // intialise the triangulation corner vertices
    vertices[0] = -modelSize/2.0;
    vertices[1] = modelSize/2.0;
    vertices[2] = modelSize/2.0;
    vertices[3] = modelSize/2.0;
    vertices[4] = modelSize/2.0;
    vertices[5] = -modelSize/2.0;
    vertices[6] = -modelSize/2.0;
    vertices[7] = -modelSize/2.0;
    // also initialise _grid for mouse input
    _grid[0] = Vector(-modelSize/2.0, modelSize/2.0, 0.0);
    _grid[1] = Vector(modelSize/2.0, modelSize/2.0, 0.0);
    _grid[2] = Vector(modelSize/2.0, -modelSize/2.0, 0.0);
    _grid[3] = Vector(-modelSize/2.0, -modelSize/2.0, 0.0);

    // seed random number generator
    std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<float> distrib(-modelSize/2.0, modelSize/2.0);
    
    // generate a random coordinate within the grid described by the four previous positions
    for (unsigned int i = 4; i < _grid.size(); i++)
    {
        vertices[i * 2] = distrib(generator);
        vertices[i * 2 + 1] = distrib(generator);
        _grid[i] = Vector(vertices[i * 2], vertices[i * 2 + 1], 0.0);        
    }

    //triangulation happens here
    delaunator::Delaunator d(vertices);

    // resize triangles vector to accomodate triangulation vertices
    _triangulationMesh.resize(d.triangles.size());
    // loop over to copy into _grid vertices
    for (unsigned int i = 0; i < d.triangles.size(); i+=3)
    {
        // triangle points
        _triangulationMesh[i    ] = Vector( d.coords[2 * d.triangles[i]], d.coords[2 * d.triangles[i] + 1], 0.0);
        _triangulationMesh[i + 1] = Vector( d.coords[2 * d.triangles[i + 1]], d.coords[2 * d.triangles[i + 1] + 1], 0.0);
        _triangulationMesh[i + 2] = Vector( d.coords[2 * d.triangles[i + 2]], d.coords[2 * d.triangles[i + 2] + 1], 0.0);        
    }
}

// draw triangular mesh
void GridBuilder::drawTriangularGrid()
{
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glBegin(GL_TRIANGLES);
    for (unsigned int i = 0; i < _triangulationMesh.size(); i++)
    {
        Vector* v = &(_triangulationMesh[i]);
        glVertex3fv(&v->x);        
    }
    glEnd();
}
