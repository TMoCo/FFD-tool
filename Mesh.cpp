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
    _modelSize = 1.0;
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
        _meshVertices.clear();
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
        // this happens if a mesh file only contains the same vertices
        if(_modelSize == 0.0)
            throw std::exception();
    }
    catch (const std::exception& e)
    {
        _modelSize = 1.0;
        _meshVertices.clear();
        QMessageBox errorMsg;
        errorMsg.setText("Could not open file.");
        errorMsg.exec();
    }
}

// save the deformed mesh to a file, we need the grid data to deform the vertices 
void Mesh::saveMesh(std::string fileName, GridBuilder* gridBuilder)
{
    try
    {
        // open a mesh file
        std::ofstream meshFile;
        // clear the mesh file
        meshFile.open(fileName, std::ofstream::trunc);
        // then start adding the mesh data
        
        // number of faces
        meshFile << _meshVertices.size() / 3 << "\n";
        // each vertex
        for(unsigned int vertex = 0; vertex < _meshVertices.size(); vertex++)
        {
            switch(gridBuilder->_gridType)
            {
                case Grid::Bilinear:
                {
                    Vector deformedVertex = deformBilinear(vertex, gridBuilder->_grid, gridBuilder->_gridSize);
                    meshFile << deformedVertex.x << " " << deformedVertex.y << " " << deformedVertex.z << "\n";
                    break;
                }
                case Grid::Barycentric:
                {
                    Vector deformedVertex = deformBarycentric(vertex++, gridBuilder->_triangulationMesh);
                    meshFile << deformedVertex.x << " " << deformedVertex.y << " " << deformedVertex.z << "\n";
                    break;
                }
                case Grid::Trilinear:
                {
                    Vector deformedVertex = deformTrilinear(vertex, gridBuilder->_grid, gridBuilder->_gridSize);
                    meshFile << deformedVertex.x << " " << deformedVertex.y << " " << deformedVertex.z << "\n";
                    break;
                }
            }
        }
        meshFile.close();
    
    }
    catch(const std::exception& e)
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
    case Grid::Trilinear:
        getTrilinearWeights(gridBuilder->_gridSize);
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
        case Grid::Trilinear:
            drawTrilinearMesh(gridBuilder->_grid, gridBuilder->getGridSize());
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
        Vector toOrigin = (gridOrigin - _meshVertices[vertex]) / (_modelSize / (gridSize-1));
        // Add data to weights and faces for easy rendering
        _weights[vertex] = Vector((int)toOrigin.x - toOrigin.x, toOrigin.y - (int)toOrigin.y, 0.0);
        _faces[vertex] = Vector(-(int)toOrigin.x, (int)toOrigin.y, 0.0);
    }
}

// mesh draw function for a regular grid using bilinear interpolation
void Mesh::drawBilinearMesh(std::vector<Vector>& gridVertices, int gridSize)
{
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    glBegin(GL_TRIANGLES);
    // for each vertex, draw a vertex at the interpolated position
    for(unsigned int vertex = 0; vertex < _meshVertices.size(); )
    {
       Vector v0 = deformBilinear(vertex++, gridVertices, gridSize);
       Vector v1 = deformBilinear(vertex++, gridVertices, gridSize);
       Vector v2 = deformBilinear(vertex++, gridVertices, gridSize);
       
       // We don't compute the normals here because we are in fact squishing all
       // the faces of the model onto the xy plane, which gives awful results for 3d
       // meshes and overlapping faces
       glVertex3fv(&v0.x);
       glVertex3fv(&v1.x);
       glVertex3fv(&v2.x);
    }
    glEnd();
}

// returns the bilinear interpolation of a given vertex relative to its grid vertices 
Vector Mesh::deformBilinear(int vertex, std::vector<Vector>& gridVertices, int gridSize)
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

    Vector deformedVertex = p10 * u * v + p00 * v * (1-u) + p11 * u * (1-v) + p01 * (1-u) * (1-v);
    return deformedVertex;
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
            // calculate the areas of the triangles and two of its subtriangles
            float triangleArea = Vector::dot(Vector(0.0, 0.0, 1.0), 
                Vector::cross(triangulationMesh[triangle + 1] - triangulationMesh[triangle], triangulationMesh[triangle + 2] - triangulationMesh[triangle]));
            float zeta1 = Vector::dot(Vector(0.0, 0.0, 1.0), 
                Vector::cross(_meshVertices[vertex] - triangulationMesh[triangle], triangulationMesh[triangle + 2] - triangulationMesh[triangle]));
            float zeta2 = Vector::dot(Vector(0.0, 0.0, 1.0), 
                Vector::cross(triangulationMesh[triangle + 1] - triangulationMesh[triangle], _meshVertices[vertex] - triangulationMesh[triangle]));

            // get the values of s and t 
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
    for (unsigned int vertex = 0; vertex < _meshVertices.size(); )
    {
        Vector v0 = deformBarycentric(vertex++, triangulationMesh);
        Vector v1 = deformBarycentric(vertex++, triangulationMesh);
        Vector v2 = deformBarycentric(vertex++, triangulationMesh);
        
        // Don't draw normal for same reasons as bilinear
        glVertex3fv(&v0.x);
        glVertex3fv(&v1.x);
        glVertex3fv(&v2.x);
    }
    glEnd();
}

Vector Mesh::deformBarycentric(int vertex, std::vector<Vector>& triangulationMesh)
{
    // get triangle data for desired vertex and multiply by the corresponding weight
    Vector deformedVertex = triangulationMesh[_faces[vertex].x] * _weights[vertex].x + 
                                triangulationMesh[_faces[vertex].y] * _weights[vertex].y + 
                                triangulationMesh[_faces[vertex].z] * _weights[vertex].z;
    return deformedVertex;
}

// Trilinear                                                        //
// -----------------------------------------------------------------//
//                                                                  //

// virtually identical to bilinear except that we also want the z component
void Mesh::getTrilinearWeights(int gridSize)
{
    // resize weights vertex
    _weights.resize(_meshVertices.size());
    _faces.resize(_meshVertices.size());
    // determine what the grid origin is
    Vector gridOrigin = Vector(-_modelSize/2.0, _modelSize/2.0, -_modelSize/2.0);
    // iterate over each mesh vertex 
    for (unsigned int vertex = 0; vertex < _meshVertices.size(); vertex++)
    {
        // determine what it's grid row and column (face) it belongs to
        // Vector from grid origin to vertex position
        Vector toOrigin = (gridOrigin - _meshVertices[vertex]) / (_modelSize / (gridSize-1));
        // Add data to weights and faces for easy rendering
        _weights[vertex] = Vector((int)toOrigin.x - toOrigin.x, toOrigin.y - (int)toOrigin.y, (int)toOrigin.z - toOrigin.z);
        _faces[vertex] = Vector(-(int)toOrigin.x, (int)toOrigin.y, -(int)toOrigin.z);
    }
}

void Mesh::drawTrilinearMesh(std::vector<Vector>& gridVertices, int gridSize)
{
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    glBegin(GL_TRIANGLES);
    // for each vertex, draw a vertex at the interpolated position
    for(unsigned int vertex = 0; vertex < _meshVertices.size(); )
    {
        Vector v0 = deformTrilinear(vertex++, gridVertices, gridSize);
        Vector v1 = deformTrilinear(vertex++, gridVertices, gridSize);
        Vector v2 = deformTrilinear(vertex++, gridVertices, gridSize);
        // now compute the normal vector
        Vector uVec = v1 - v0;
        Vector vVec = v2 - v0;
        Vector normal = Vector::cross(uVec, vVec).normalise();

        glNormal3fv(&normal.x);
        glVertex3fv(&v0.x);
        glVertex3fv(&v1.x);
        glVertex3fv(&v2.x);
    }
    glEnd();
}

Vector Mesh::deformTrilinear(int vertex, std::vector<Vector>& gridVertices, int gridSize)
{
    // get the row and column indices for face point calc
        // row and col are both in relation to gridsize with the top right 
        // corner as the origin
        int cel = _faces[vertex].z;
        int row = _faces[vertex].y;
        int col = _faces[vertex].x;
        // get weights
        float u = _weights[vertex].x; 
        float v = _weights[vertex].y;
        float w = _weights[vertex].z;
        // fetch vectors
        Vector p000 = gridVertices[cel*gridSize*gridSize + row*gridSize + col];
        Vector p001 = gridVertices[cel*gridSize*gridSize + row*gridSize + col+1];
        Vector p010 = gridVertices[cel*gridSize*gridSize + (row+1)*gridSize + col];
        Vector p011 = gridVertices[cel*gridSize*gridSize + (row+1)*gridSize + col+1];
        Vector p100 = gridVertices[(cel+1)*gridSize*gridSize + row*gridSize + col];
        Vector p101 = gridVertices[(cel+1)*gridSize*gridSize + row*gridSize + col+1];
        Vector p110 = gridVertices[(cel+1)*gridSize*gridSize + (row+1)*gridSize + col];
        Vector p111 = gridVertices[(cel+1)*gridSize*gridSize + (row+1)*gridSize + col+1];

        // calculate vertex position based on grid vertices which we can access with row and col (x and y)
        Vector p0 = p011 * u * v + p010 * v * (1 - u) + p001 * u * (1 - v) + p000 * (1 - u) * (1 - v);
        Vector p1 = p111 * u * v + p110 * v * (1 - u) + p101 * u * (1 - v) + p100 * (1 - u) * (1 - v);
        // apply another step for z
        Vector deformedVertex = p0 * (1-w) + p1 * w;

        return deformedVertex;
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
