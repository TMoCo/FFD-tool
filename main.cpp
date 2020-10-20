// Where the main window is created 
#include <QApplication>

#include "DeformWidget.h"
#include "Window.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    Window aWindow(NULL);

    aWindow.resize(800,550);

    aWindow.show();

    return app.exec();
}