#include "Quaternion.h"

#include <math.h>

// constructors
// default to unit quaternion
Quaternion::Quaternion() : scalar(0.0), vector(Vector())
{}

Quaternion::Quaternion(float scalar, const Vector &vector) : scalar(scalar), vector(vector)
{}

Quaternion::Quaternion(const Quaternion &other) : scalar(other.scalar), vector(other.vector)
{}

// quaternion addition
Quaternion Quaternion::operator+(const Quaternion &other)
{
    return Quaternion
    (
        scalar + other.scalar, 
        Vector(vector.x + other.vector.x, vector.y + other.vector.y, vector.z + other.vector.z)
    );
}

// quaternion subtraction
Quaternion Quaternion::operator-(const Quaternion &other)
{
    return Quaternion
    (
        scalar - other.scalar, 
        Vector(vector.x - other.vector.x, vector.y - other.vector.y, vector.z - other.vector.z)
    );
}

// quaternion multiplication
Quaternion Quaternion::operator*(const Quaternion &other)
{
    return Quaternion
    (
        scalar * other.scalar - Vector::dot(vector, other.vector),
        other.vector * scalar + vector * other.scalar + Vector::cross(vector, other.vector)
    );
}

// Scalar multiplication
Quaternion Quaternion::operator*(float s)
{
    return Quaternion(scalar * s, vector * s);
}

// Scalar division
Quaternion Quaternion::operator/(float s)
{
    return Quaternion(scalar / s, vector / s);
}

// return the quaternion's complex conjugate 
Quaternion Quaternion::conjugate()
{
    return Quaternion(scalar, vector * -1);
}

// return the quaternion's invers
Quaternion Quaternion::inverse()
{
    return Quaternion(conjugate() / (norm() * norm()));
}

// return the quaternion's norm
float Quaternion::norm()
{
    return scalar * scalar + Vector::dot(vector, vector);
}

// return the normalised quaternion
Quaternion Quaternion::normalise()
{
    return Quaternion(*this / norm());
}

// output quaternion to a stream
std::ostream& operator<<(std::ostream &outStream, Quaternion quaternion)
{
    outStream << "(" << quaternion.scalar << ", " << quaternion.vector << ")";
    return outStream;
}

