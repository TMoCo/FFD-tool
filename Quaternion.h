#ifndef _QUATERNION_H
#define _QUATERNION_H

#include "Vector.h"

class Quaternion
{
    public:
    
    float scalar;
    Vector vector;

    // Constructor
    Quaternion();
    Quaternion(float scalar, const Vector &vector);
    Quaternion(const Quaternion &quaternion);

    // Quaternion addition
    Quaternion operator+(const Quaternion &other);
    
    // Quaternion subtraction
    Quaternion operator-(const Quaternion &other);
    
    // Quaternion multiplication
    Quaternion operator*(const Quaternion &other);

    // Scalar multiplication
    Quaternion operator*(float scalar);

    // Scalar division
    Quaternion operator/(float scalar);

    // Complex conjugate
    Quaternion conjugate();

    // Quaternion inverse
    Quaternion inverse();

    // Normalised quaternion
    Quaternion normalise();

    // Quaternion norm
    float norm();
};

std::ostream& operator<<(std::ostream &outStream, Quaternion quaternion);

#endif