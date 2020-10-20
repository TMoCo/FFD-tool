#include <iostream>
#include <fstream>
#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <QMessageBox>

#include "DeformWidget.h"

static float light_position[] = {0.0, 0.0, 1.0, 0.0};		

// Constructor
DeformWidget::DeformWidget(QWidget *parent) 
    : QGLWidget(parent)
{

    mesh = Mesh();
    gridBuilder = GridBuilder();
    
    // resize data 
    points.resize(0.0);
    
    // init flags
    dragging = false;
    attenuation = false;
    setMouseTracking(true);

    // init val
    closest = -1;
    previousMousePos = Vector(0.0, 0.0, 0.0);
    
}

//
// OpenGL methods
//

void DeformWidget::initializeGL()
{
    // enable Z-buffering
    glEnable(GL_DEPTH_TEST);

    // set lighting parameters
    glShadeModel(GL_FLAT);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);

    // background is pink
    glClearColor(1.0, 0.7, 0.7, 1.0);
}

// called every time the widget is resized
void DeformWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);

    // set projection matrix to be glOrtho based on zoom & window size
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float dimensions = mesh.getModelSize();
    // stick with orthogonal projection
    glOrtho(-dimensions, dimensions, -dimensions, dimensions, -dimensions, dimensions);

}
	
// called every time the widget needs painting
void DeformWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set lighting on
    glEnable(GL_LIGHTING);

    // set model view matrix based on stored translation, rotation &c.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // set light position first
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    
    gridBuilder.drawGrid();

    if (!mesh.isEmpty())
        mesh.drawMesh(&gridBuilder);

    // drawPoints();
}

// Keep the width and height of the opengl widget constant for simplicity
QSize DeformWidget::minimumSizeHint() const
{
    return QSize(500, 500);
}

QSize DeformWidget::sizeHint() const
{
    return QSize(500, 500);
}

//
// Mouse methods
// 

// start processing mouse event
void DeformWidget::mousePressEvent(QMouseEvent *event)
{
    float worldX = (2.0 * event->localPos().x() / width()  - 1 ) * mesh.getModelSize();;
    float worldY = (-2.0 * event->localPos().y() / height() + 1 ) * mesh.getModelSize();;
    // check that we are clicking on a vertex
    if((event->button() == Qt::LeftButton ) && checkClick2D(worldX, worldY))
    {
        // set dragging to true if we are
        dragging = true;

        // for debug
        points.push_back( worldX );
        points.push_back( worldY );
    }
}

// process mouse movement (deform the grid according the direction we are travelling in)
void DeformWidget::mouseMoveEvent(QMouseEvent *event)
{
    if((event->buttons() & Qt::LeftButton) && dragging)
    {
        // move closest vector 
        // get mouse movement direction vector
        float worldX = (-2.0 * event->localPos().x() / width()  + 1 ) * mesh.getModelSize();
        float worldY = ( 2.0 * event->localPos().y() / height() - 1 ) * mesh.getModelSize();
        
        // change all occurences of the given vertex
        gridBuilder.moveVertex(previousMousePos.x - worldX, previousMousePos.y - worldY, closest, attenuation);

        // update meshes         
        updateGL();
    }
    // update mouse position
    previousMousePos.x = (-2.0 * event->localPos().x() / width()  + 1 ) * mesh.getModelSize();
    previousMousePos.y = ( 2.0 * event->localPos().y() / height() - 1 ) * mesh.getModelSize();
}

// stop processing the mouses movement
void DeformWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && dragging)
    {
        dragging = false;
    }
}

// for given coordinates, check if within a vertex's radius
bool DeformWidget::checkClick2D(float mouseX, float mouseY)
{
    // fraction of sphere size guarantees the same pixel radius for any mesh 
    float clickRadius = mesh.getModelSize() / 8.0;
    Vector clickOnXY = Vector(mouseX, mouseY, 0.0);

    closest = -1;

    // iterate over the list of grid vertices to find the one within the clicked area's radius
    for(unsigned int i = 0; i < gridBuilder._grid.size(); i++)
    {
        // project mesh vertex on x y plane
        Vector vertexOnXY = Vector(gridBuilder._grid[i].x, gridBuilder._grid[i].y, 0.0); 

        // find the closest 
        if((clickOnXY - vertexOnXY).magnitude() < clickRadius)
        {
            // if we have multiple points in the click radius, keep the closest
            if ((closest == -1) || abs(gridBuilder._grid[closest].z) > abs(gridBuilder._grid[i].z))
                closest = i;
        }
    }

    if (closest != -1)
    {
        return true;
    }

    return false;
}


//
// Mesh Methods
//

// load a mesh when a file has been chosen
void DeformWidget::loadMesh(QString fileName)
{
    // load mesh in Mesh object
    mesh.loadMesh(fileName.toStdString());
    // generate a new grid
    buildGrid();
}

//
// Grid Methods
//

// slot for receiving grid size from slider
void DeformWidget::changeGridSize(int value)
{
    gridBuilder.setGridSize(value);
}
// slot for receiving grid type from checkboxes
void DeformWidget::changeGridType(int value)
{
    gridBuilder.setGridType(static_cast<Grid>(value));
}
// slot for creating a new grid 
void DeformWidget::buildGrid()
{
    // update the grid
    gridBuilder.generateGrid(mesh.getModelSize());
    // update the mesh weights 
    mesh.getVertexWeights(&gridBuilder);
    // update projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float dimensions = mesh.getModelSize();
    // stick with orthogonal projection
    glOrtho(-dimensions, dimensions, -dimensions, dimensions, -dimensions, dimensions);
    // update gl widget
    updateGL();
}
// slot for activating/deactivating attenuation
void DeformWidget::setAttenuation(int value)
{
    attenuation = value;
}
//
// Debug
//

// debug function to test mouse input
void DeformWidget::drawPoints()
{
    glPointSize(10.0);
    for (unsigned int i = 0; i < points.size(); i+=2)
    {
        glBegin(GL_POINTS);
        glVertex3f( points[i], points[i+1], 0.0);
        glEnd();
    }
}
