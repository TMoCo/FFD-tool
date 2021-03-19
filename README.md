Assignment 1: Free Form Deformation Tool

Libraries used: 
    Qt version 5.9.5
    OpenGL 4.6
    GLU
    Delaunator for triangulating (https://github.com/delfrrr/delaunator-cpp) 

How to use the tool:

Open a mesh file with the "Load" button, save a mesh with the "Save" button.

Change grid type by selecting grid options in GIU (regular 2D grid, mesh from triangulation of random points, regular 3D cage) and edit number of grid vertices with slider.
To apply the desired changes to the grid, click apply changes. This will reset the model mesh to the one originally loaded. 

Attenutation can be switched on or off (default off) for any grid by checking the attenuation checkbox, and scaled up or down with the slider.

![Example](https://media.giphy.com/media/btRWgnTUqRztYbBdZD/giphy.gif)
