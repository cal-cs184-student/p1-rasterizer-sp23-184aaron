#include "transforms.h"

#include "CGL/matrix3x3.h"
#include "CGL/vector2D.h"
#include "CGL/vector3D.h"

namespace CGL {

Vector2D operator*(const Matrix3x3 &m, const Vector2D &v) {
	Vector3D mv = m * Vector3D(v.x, v.y, 1);
	return Vector2D(mv.x / mv.z, mv.y / mv.z);
}

Matrix3x3 translate(float dx, float dy) {
	// Part 3: Fill this in.
    Matrix3x3 translateMatrix = Matrix3x3(1, 0, dx, 0, 1, dy, 0, 0, 1);
	return translateMatrix;
}

Matrix3x3 scale(float sx, float sy) {
	// Part 3: Fill this in.
    Matrix3x3 scaleMatrix = Matrix3x3(sx, 0, 0, 0, sy, 0, 0, 0, 1);

	return scaleMatrix;
}

// The input argument is in degrees counterclockwise
Matrix3x3 rotate(float deg) {
	// Part 3: Fill this in.
    float rad = radians(deg);
    Matrix3x3 rotateMatrix = Matrix3x3(cos(rad), -sin(rad), 0, sin(rad), cos(rad), 0, 0, 0, 1);

	return rotateMatrix;
}

}
