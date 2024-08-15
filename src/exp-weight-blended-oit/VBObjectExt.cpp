#include "VBObjectExt.h"

VBObjectExt::VBObjectExt()
{
    position[0] = position[1] = position[2] = .0;
    rotation_matrix = vmath::mat4::identity();
}

bool VBObjectExt::Translate(float x, float y, float z)
{
    position[0] += x;
    position[1] += y;
    position[2] += z;

    return true;
}

bool VBObjectExt::Rotate(float angle, float vX, float vY, float vZ)
{
    vmath::vec3 vector;
    vector[0] = vX;
    vector[1] = vY;
    vector[2] = vZ;
    rotation_matrix = vmath::rotate(angle, vector);

    return true;
}

bool VBObjectExt::SetColor(float r, float g, float b, float a)
{
    color[0] = r;
    color[1] = g;
    color[2] = b;
    color[3] = a;

    return true;
}

bool VBObjectExt::GetColor(float out[4]) const
{
    out[0] = color[0];
    out[1] = color[1];
    out[2] = color[2];
    out[3] = color[3];

    return true;
}

vmath::mat4 VBObjectExt::GetTransform() const
{
    // Rotate then translate.
    return vmath::translate(position[0], position[1], position[2]) * rotation_matrix;
}

