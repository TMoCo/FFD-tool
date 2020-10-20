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
    void showFileDialog();
    void changeGrid(int type);
    void sliderValues(int value);

    signals:
    void fileFound(QString fileName);

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
    QCheckBox *attenuation;
    QButtonGroup *gridCheckBoxes;
    QPushButton *changeGridButton;
};

#endif