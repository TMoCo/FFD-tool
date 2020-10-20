#include <fstream>
#include <iostream>

#include <GL/gl.h>
#include <GL/glu.h>

#include <QMessageBox>

#include "Mesh.h"

// initialise Mesh variables
Mesh::Mesh()
{
    _meshMidPoint = Vector(0.0, 0.0, 0.0);
    _modelSize = 1.5;
    _meshVertices.resize(0.0);
    _weights.resize(0.0);
    _faces.resize(0.0);
}

// Mesh methods (called by DeformWidget)                            //
// -----------------------------------------------------------------//
//                                                                  //

// load requested mesh from file
void Mesh::loadMesh(std::string fileName)
{
    try 
    {
        std::ifstream inFile(fileName);

        Vector minCoords = Vector(1000000.0, 1000000.0, 1000000.0);
	    Vector maxCoords = Vector(-1000000.0, -1000000.0, -1000000.0);
        
        // open the input file
        
        // set the number of vertices and faces
        float nTriangles = 0, nVertices = 0;
        
        // set the midpoint to the origin
        _meshMidPoint = Vector(0.0, 0.0, 0.0);

        // read in the number of vertices
        inFile >> nTriangles;
        nVertices = nTriangles * 3;

        // now allocate space for them all
        _meshVertices.resize(nVertices);
        
        // now loop to read the vertices in, and hope nothing goes wrong
        for (int vertex = 0; vertex < nVertices; vertex++)
        {
            inFile >> _meshVertices[vertex].x >> _meshVertices[vertex].y >> _meshVertices[vertex].z;

            // keep running track of midpoint, &c.
            _meshMidPoint = _meshMidPoint + _meshVertices[vertex];
            if (_meshVertices[vertex].x < minCoords.x) minCoords.x = _meshVertices[vertex].x;
            if (_meshVertices[vertex].y < minCoords.y) minCoords.y = _meshVertices[vertex].y;
            if (_meshVertices[vertex].z < minCoords.z) minCoords.z = _meshVertices[vertex].z;

            if (_meshVertices[vertex].x > maxCoords.x) maxCoords.x = _meshVertices[vertex].x;
            if (_meshVertices[vertex].y > maxCoords.y) maxCoords.y = _meshVertices[vertex].y;
            if (_meshVertices[vertex].z > maxCoords.z) maxCoords.z = _meshVertices[vertex].z;
        }

        // now sort out the size of a bounding sphere for viewing
        // and also set the midpoint's location
        _meshMidPoint = _meshMidPoint / _meshVertices.size();
        
        // now go back through the vertices, subtracting the mid point
        for (int vertex = 0; vertex < nVertices; vertex++)
        {
            _meshVertices[vertex] = _meshVertices[vertex] - _meshMidPoint;
        } 

        // the bounding sphere radius is just half the distance between these
        _modelSize = (maxCoords - minCoords).magnitude();

        // generate a grid
        //gridBuilder = GridBuilder(gridSize, modelSize, gridType);

        // trigger a resizeGL to paint the changes and incorporate new projection
        //resizeGL(height(), width());
    }
    catch (const std::exception& e)
    {
        QMessageBox errorMsg;
        errorMsg.setText("Could not open file.");
        errorMsg.exec();
    }
}

// generates vertex weights depending on the type of grid chosen

void Mesh::getVertexWeights(GridBuilder* gridBuilder)
{
    switch (gridBuilder->getGridType())
    {
    case Grid::Bilinear:
        getBilinearWeights(gridBuilder->_gridSize);
        break;
    case Grid::Barycentric:
        getBarycentricWeights(gridBuilder->_triangulationMesh);
        break;
    default:
        break;
    }
}

// draws the mesh as loaded from the file
void Mesh::drawMesh(GridBuilder* gridBuilder)
{
    switch (gridBuilder->getGridType())
    {
        case Grid::Bilinear:
            drawBilinearMesh(gridBuilder->_grid, gridBuilder->getGridSize());
            break;
        case Grid::Barycentric:
            drawBarycentricMesh(gridBuilder->_triangulationMesh);
           break;

        default:
            break;
    }
}

// Biliear                                                          //
// -----------------------------------------------------------------//
//                                                                  //

// for each vertex in the mesh, determine what is u,v weights are
void Mesh::getBilinearWeights(int gridSize)
{
    // resize weights vertex
    _weights.resize(_meshVertices.size());
    _faces.resize(_meshVertices.size());
    // determine what the grid origin is
    Vector gridOrigin = Vector(-_modelSize/2.0, _modelSize/2.0, 0.0);
    // iterate over each mesh vertex 
    for (unsigned int vertex = 0; vertex < _meshVertices.size(); vertex++)
    {
        // determine what it's grid row and column (face) it belongs to
        // Vector from grid origin to vertex position
        Vector newVec = (gridOrigin - _meshVertices[vertex]) / (_modelSize / (gridSize-1));
        // Add data to weights and faces for easy rendering
        _weights[vertex] = Vector((int)newVec.x - newVec.x, newVec.y - (int)newVec.y, 0.0);
        _faces[vertex] = Vector(-(int)newVec.x, (int)newVec.y, 0.0);
    }
}

// mesh draw function for a regular grid using bilinear interpolation
void Mesh::drawBilinearMesh(std::vector<Vector>& gridVertices, int gridSize)
{
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    glBegin(GL_TRIANGLES);
    // for each vertex, draw a vertex at the interpolated position
    for(unsigned int vertex = 0; vertex < _meshVertices.size(); vertex++)
    {
        // get the row and column indices for face point calc
        // row and col are both in relation to gridsize with the top right 
        // corner as the origin
        int row = _faces[vertex].y;
        int col = _faces[vertex].x;
        // get weights
        float u = _weights[vertex].x; 
        float v = _weights[vertex].y;
        // calculate vertex position based on grid vertices which we can access with row and col
        Vector p00 = gridVertices[(row + 1) * gridSize + col    ];
        Vector p10 = gridVertices[(row + 1) * gridSize + col + 1];
        Vector p01 = gridVertices[row * gridSize + col    ];
        Vector p11 = gridVertices[row * gridSize + col + 1];

        Vector deformedVertex = calcBilinear(u, v, p00, p01, p10, p11);

        glVertex3f(deformedVertex.x, deformedVertex.y, deformedVertex.z);
    }
    glEnd();
}

// return the bilinear interpolation of the given points for a regular grid
Vector Mesh::calcBilinear(float u, float v, Vector p00, Vector p01, Vector p10, Vector p11)
{
    return p10 * u * v + p00 * v * (1 - u) + p11 * u * (1 - v) + p01 * (1 - u) * (1 - v);
}

// Barycentric                                                      //
// -----------------------------------------------------------------//
//                                                                  //

void Mesh::getBarycentricWeights(std::vector<Vector>& triangulationMesh)
{
    // for each vertex in the mesh, apply the halfplane test on the whole triangular mesh 
    // for the triangle that satisfies the test, determine the weights associated and store in _weights vector
    // for each triangle in the triangular  
    _weights.resize(_meshVertices.size());
    _faces.resize(_meshVertices.size());

    for(unsigned int vertex = 0; vertex < _meshVertices.size(); vertex++)
    {
        // get weights for the vertex depending ontriangulation mesh
        for(unsigned int triangle = 0; triangle < triangulationMesh.size(); triangle+=3)
        {
            // get the values of s and t
            float triangleArea = Vector::dot(Vector(0.0, 0.0, 1.0), 
                Vector::cross(triangulationMesh[triangle + 1] - triangulationMesh[triangle], triangulationMesh[triangle + 2] - triangulationMesh[triangle]));
            float zeta1 = Vector::dot(Vector(0.0, 0.0, 1.0), 
                Vector::cross(_meshVertices[vertex] - triangulationMesh[triangle], triangulationMesh[triangle + 2] - triangulationMesh[triangle]));
            float zeta2 = Vector::dot(Vector(0.0, 0.0, 1.0), 
                Vector::cross(triangulationMesh[triangle + 1] - triangulationMesh[triangle], _meshVertices[vertex] - triangulationMesh[triangle]));

            float s = zeta1 / triangleArea;
            float t = zeta2 / triangleArea;

            // if s and t satisfiy these conditions, then the point is inside the triangle!
            if (( (0.0 <= s) && (s <= 1.0) ) && ( (0.0 <= t) && (t <= 1.0) ) && (s + t <= 1.0)) 
            {
                // store alpha, beta, gamma
                _weights[vertex] = Vector(1 - s - t, s, t);
                // store the triangulation vertices for when we want to draw the model mesh
                _faces[vertex] = Vector(triangle, triangle + 1, triangle + 2);
                // break the loop over triangulation mesh
                std::cout << _weights[vertex] << std::endl;
                break;
            }
        }
    }

}

void Mesh::drawBarycentricMesh(std::vector<Vector>& triangulationMesh)
{
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    glBegin(GL_TRIANGLES);
    // iterate over the mesh vertices
    // get the vertex's weights
    // get the positions of each vertex stored in faces (by id)
    // interpolate based on weights and faces
    for (unsigned int vertex = 0; vertex < _meshVertices.size(); vertex++)
    {
        // get triangle data and multiply by the corresponding weight
        Vector deformedVertex = triangulationMesh[_faces[vertex].x] * _weights[vertex].x + 
                                triangulationMesh[_faces[vertex].y] * _weights[vertex].y + 
                                triangulationMesh[_faces[vertex].z] * _weights[vertex].z;
        // add deformed vertex
        glVertex3f(deformedVertex.x, deformedVertex.y, deformedVertex.z);
    }
    glEnd();
}

// Utility                                                          //
// -----------------------------------------------------------------//
//                                                                  //

// Utility function for drawing mesh as in file
void Mesh::drawFileMesh()
{
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glBegin(GL_TRIANGLES);

    // assume CCW order
    for (unsigned int vertex = 0; vertex < _meshVertices.size(); )
    { 
        // use increment to step through them
        Vector *v0 = &(_meshVertices[vertex++]);
        Vector *v1 = &(_meshVertices[vertex++]);
        Vector *v2 = &(_meshVertices[vertex++]);
        // now compute the normal vector
        Vector uVec = *v1 - *v0;
        Vector vVec = *v2 - *v0;
        Vector normal = Vector::cross(uVec, vVec).normalise();

        glNormal3fv(&normal.x);
        glVertex3fv(&v0->x);
        glVertex3fv(&v1->x);
        glVertex3fv(&v2->x);
    }
    glEnd();
}

// check mesh
bool Mesh::isEmpty()
{
    if(_meshVertices.empty())
        return true;
    else
        return false;
    
}

float Mesh::getModelSize()
{
    return _modelSize;
}
