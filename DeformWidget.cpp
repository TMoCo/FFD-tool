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

    Ball_Init(&objectBall);		
    Ball_Place(&objectBall, qOne, 0.80);

    // resize data 
    points.resize(0.0);
    
    // init flags
    dragging = false;
    attenuation = false;
    setMouseTracking(true);

    // init val
    closest = -1;
    previousMousePos = Vector(0.0, 0.0, 0.0);
    currentPos = Vector(0.0, 0.0, 0.0);    
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
    float aspectRatio = (float) w / (float) h;

    if (aspectRatio > 1.0)
		glOrtho(-aspectRatio * dimensions, aspectRatio * dimensions, -dimensions, dimensions, -dimensions, dimensions);
	else
        glOrtho(-dimensions, dimensions, -dimensions/aspectRatio, dimensions/aspectRatio, -dimensions, dimensions);
	
    
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
    
    GLfloat mNow[16];
    Ball_Value(&objectBall, mNow);
	glMultMatrixf(mNow);

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
    mouseButton = event->button();
    HVect vNow;
    vNow.x =   (2.0 * event->localPos().x() / width() -1) * mesh.getModelSize();
    vNow.y = (-2.0 * event->localPos().y() / height() +1) * mesh.getModelSize();
    // check that we are clicking on a vertex
    switch(mouseButton)
    {
        case(Qt::LeftButton):
            if (checkClick2D(vNow.x, vNow.y))
                dragging = true;
            break;
        case(Qt::RightButton):
            Ball_Mouse(&objectBall, vNow);
			// start dragging
			Ball_BeginDrag(&objectBall);
			// update the widget
			updateGL();
            break;
    }
}

// process mouse movement (deform the grid according the direction we are travelling in)
void DeformWidget::mouseMoveEvent(QMouseEvent *event)
{
    // get mouse movement direction vector
    HVect vNow;
    vNow.x =   (2.0 * event->localPos().x() / width()  - 1 ) * mesh.getModelSize(); // worldX
    vNow.y = (-2.0 * event->localPos().y() / height() + 1 ) * mesh.getModelSize(); // worldY

    // invert by the rotation in objectBall (the transpose since it is a rotation/orthogonal matrix)
    float rotatedX = objectBall.mNow[0][0]*(vNow.x - previousMousePos.x) + objectBall.mNow[0][1]*(vNow.y - previousMousePos.y) + objectBall.mNow[0][2]*(vNow.z - previousMousePos.z); 
    float rotatedY = objectBall.mNow[1][0]*(vNow.x - previousMousePos.x) + objectBall.mNow[1][1]*(vNow.y - previousMousePos.y) + objectBall.mNow[1][2]*(vNow.z - previousMousePos.z);
    float rotatedZ = objectBall.mNow[2][0]*(vNow.x - previousMousePos.x) + objectBall.mNow[2][1]*(vNow.y - previousMousePos.y) + objectBall.mNow[2][2]*(vNow.z - previousMousePos.z);
    
    switch(mouseButton)
    {
        case(Qt::LeftButton):
            if(dragging)
                gridBuilder.moveVertex(Vector(rotatedX, rotatedY, rotatedZ), closest, attenuation);        
            break;
        case(Qt::RightButton):
            Ball_Mouse(&objectBall, vNow);
			Ball_Update(&objectBall);
            break;
    }    
    updateGL();
    // update mouse position
    previousMousePos.x = (2.0 * event->localPos().x() / width()  - 1 ) * mesh.getModelSize();
    previousMousePos.y = (-2.0 * event->localPos().y() / height() + 1 ) * mesh.getModelSize();
}

// stop processing the mouses movement
void DeformWidget::mouseReleaseEvent(QMouseEvent *event)
{
    switch(mouseButton)
    {
        case(Qt::LeftButton):
            dragging = false;
            updateGL();
            break;
        case(Qt::RightButton):
            Ball_EndDrag(&objectBall);
            updateGL();
            break;
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
        // rotate the grid vertex according to the arc ball rotation
        // just get the x and y values
        float rotatedX = objectBall.mNow[0][0]*gridBuilder._grid[i].x + objectBall.mNow[1][0]*gridBuilder._grid[i].y + objectBall.mNow[2][0]*gridBuilder._grid[i].z; 
        float rotatedY = objectBall.mNow[0][1]*gridBuilder._grid[i].x + objectBall.mNow[1][1]*gridBuilder._grid[i].y + objectBall.mNow[2][1]*gridBuilder._grid[i].z; 
        float rotatedZ = objectBall.mNow[0][2]*gridBuilder._grid[i].x + objectBall.mNow[1][2]*gridBuilder._grid[i].y + objectBall.mNow[2][2]*gridBuilder._grid[i].z; 
        
        // then project mesh vertex on x y plane
        Vector vertexOnXY = Vector(rotatedX, rotatedY, 0.0);
        // find the closest 
        if((clickOnXY - vertexOnXY).magnitude() < clickRadius)
        {
            // if we have multiple points in the click radius, keep the closest
            if ((closest == -1) || (objectBall.mNow[0][2]*gridBuilder._grid[closest].x + objectBall.mNow[1][2]*gridBuilder._grid[closest].y + objectBall.mNow[2][2]*gridBuilder._grid[closest].z < rotatedZ))
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
