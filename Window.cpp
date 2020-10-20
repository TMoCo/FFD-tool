#include <iostream>

#include <QFileDialog>
#include <QLineEdit>

#include "Window.h"

Window::Window(QWidget *parent) 
    : QWidget(parent)
{
    // OpenGL widget
    deformGroupBox = new QGroupBox(tr("Preview"));
    deform = new DeformWidget(this);
    deformLayout = new QGridLayout;

    deformLayout->setContentsMargins(0,0,0,20);
    deformLayout->addWidget(deform, 0, 0, Qt::AlignCenter);
    deformGroupBox->setLayout(deformLayout);
    
    // file options layout
    fileGroupBox = new QGroupBox(tr("File Options"));
    loadButton = new QPushButton("Load", this);
    saveButton = new QPushButton("Save", this);
    fileLayout = new QGridLayout;

    fileLayout->setAlignment(Qt::AlignTop);
    fileLayout->addWidget(loadButton, 0, 0, 1, 1);
    fileLayout->addWidget(saveButton, 0, 2, 1, 1);
    fileGroupBox->setLayout(fileLayout);

    // grid options layout
    gridGroupBox = new QGroupBox(tr("Grid Options"));
    gridSliderLabel = new QLabel(tr("Grid size"), this);
    gridSlider = new QSlider(Qt::Horizontal, this);
    gridCheckBoxes = new QButtonGroup(this);
    regular2DGrid = new QCheckBox("Regular grid (2D)", this);
    triangular2DGrid = new QCheckBox("Triangular grid", this);
    regular3DGrid = new QCheckBox("Regular grid (3D)", this);
    attenuation = new QCheckBox("Attenuation", this);
    changeGridButton = new QPushButton("Apply changes", this);
    resetRotation = new QPushButton("Reset rotation", this);
    gridLayout = new QGridLayout;

    gridSlider->setRange(2, 10);    
    gridSlider->setSingleStep(1);   
    regular2DGrid->setCheckState(Qt::Checked);
    gridCheckBoxes->setExclusive(true);
    gridCheckBoxes->addButton(regular2DGrid, 0);
    gridCheckBoxes->addButton(triangular2DGrid, 1);
    gridCheckBoxes->addButton(regular3DGrid, 2);

    gridLayout->setAlignment(Qt::AlignTop); 
    gridLayout->addWidget(gridSliderLabel, 0, 0);
    gridLayout->addWidget(gridSlider, 1, 0, 1, 2);
    gridLayout->addWidget(regular2DGrid, 3, 0);
    gridLayout->addWidget(triangular2DGrid, 4, 0);
    gridLayout->addWidget(regular3DGrid, 5, 0);
    gridLayout->addWidget(attenuation, 6, 0);
    gridLayout->addWidget(resetRotation, 7, 0, 2, 1);
    gridLayout->addWidget(changeGridButton, 7, 1, 3, 1);
    gridGroupBox->setLayout(gridLayout);

    // Window layout
    QGridLayout *layout = new QGridLayout;
    layout->setAlignment(Qt::AlignTop); 
    layout->addWidget(deformGroupBox, 0, 0, 4, 1);
    layout->addWidget(fileGroupBox, 0, 1);
    layout->addWidget(gridGroupBox, 1, 1);
    setLayout(layout);

    // connect widgets
    QObject::connect(loadButton, SIGNAL(clicked()), this, SLOT(showFileDialog()));
    QObject::connect(this, SIGNAL(fileFound(QString)), deform, SLOT(loadMesh(QString)));
    QObject::connect(gridSlider, SIGNAL(valueChanged(int)), deform, SLOT(changeGridSize(int)));
    QObject::connect(gridCheckBoxes, SIGNAL(buttonClicked(int)), deform, SLOT(changeGridType(int)));
    QObject::connect(attenuation, SIGNAL(stateChanged(int)), deform, SLOT(setAttenuation(int)));
    QObject::connect(changeGridButton, SIGNAL(clicked()), deform, SLOT(buildGrid()));
    QObject::connect(resetRotation, SIGNAL(clicked()), deform, SLOT(resetRotation()));
}

// opens up a file browser dialog and emits a signal to the GL widget if a mesh file is chosen
void Window::showFileDialog()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Load Mesh"), "./", tr("Meshes (*.mesh)"));
    
    if (!fileName.isEmpty())
    {
        emit fileFound(fileName);
    }
}

void Window::changeGrid(int type)
{
    std::cout << type << std::endl;
}

void Window::sliderValues(int value)
{
    std::cout << value << std::endl;
}
