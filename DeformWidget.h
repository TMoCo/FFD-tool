#ifndef _DEFORM_WIDGET_H
#define _DEFORM_WIDGET_H

#include <QGLWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QGridLayout>
#include <QSize>
#include <QString>

#include <vector>

// utility class defining vector object (3 floats) and operations 
#include "Vector.h"
#include "Ball.h"
#include "GridBuilder.h"
#include "Mesh.h"

class DeformWidget : public QGLWidget
{
    Q_OBJECT
    public:

    // constructor
    DeformWidget(QWidget *parent);

    // slots
    public slots:
    // get a mesh file name from parent widget to be loaded 
    void loadMesh(QString fileName);
    // upon click a button, create a grid
    void buildGrid();
    // get the new value of the slider
    void changeGridSize(int value);
    // get the new grid type value
    void changeGridType(int value);
    // set the attenuation flag
    void setAttenuation(int value);

    //signals

    protected:
    // Qt opengl functions
	// called when OpenGL context is set up
	void initializeGL();
	// called every time the widget is resized
	void resizeGL(int w, int h);
	// called every time the widget needs painting
	void paintGL();

    // process mouse inputs
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);


    private:
    int mouseButton;
    // flag representing drag status
    bool dragging;
    // flag representing rotation status
    bool rotating;
    // flag representing attenuation 
    bool attenuation;

    // mouse Input
    Vector previousMousePos;
    Vector currentPos;

    BallData objectBall;

    GLfloat translate_x, translate_y;
	GLfloat last_x, last_y;

    // mesh data
    Mesh mesh;

    // grid vertices (array)
    int closest;
    GridBuilder gridBuilder;

    // widget size
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    // check that area clicked on screen is close to a grid vertex
    bool checkClick2D(float mouseX, float mouseY);
    // check the volume around the point along 
    bool checkClick3D();

    // 
    // debug
    // 
    std::vector<float> points;
    void drawPoints();

};

#endif