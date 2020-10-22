#ifndef _WINDOW_H
#define _WINDOW_H

#include <string>


#include <QtWidgets>
#include <QMouseEvent>

#include "DeformWidget.h"

class Window : public  QWidget
{
    Q_OBJECT
    public:

    // constructor
    Window(QWidget *parent);

    public slots:
    void loadFileDialog();
    void saveFileDialog();
    signals:
    void loadMeshFile(QString fileName);
    void saveMeshFile(QString fileName);

    private:
    // widgets for mesh preview
    QGroupBox *deformGroupBox;
    QGridLayout *deformLayout;
    DeformWidget *deform;

    // widgets for file options
    QGroupBox *fileGroupBox;
    QGridLayout *fileLayout;
    QPushButton *loadButton;
    QPushButton *saveButton;
    
    // widgets for grid control
    QGroupBox *gridGroupBox;
    QGridLayout *gridLayout;
    QLabel *gridSliderLabel;
    QSlider *gridSlider;
    QCheckBox *regular2DGrid;
    QCheckBox *triangular2DGrid;
    QCheckBox *regular3DGrid;
    QButtonGroup *gridCheckBoxes;
    QPushButton *changeGridButton;
    
    // widgets for attenuation
    QGroupBox *attenuationGroupBox;
    QGridLayout *attenuationLayout;
    QLabel *attenuationSliderLabel;
    QSlider *attenuationSlider;
    QCheckBox *attenuation;
    
    QPushButton *resetRotation;
};

#endif