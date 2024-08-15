#pragma once

#include "vbm.h"
#include "vmath.h"

class VBObjectExt : public VBObject
{
public:
    VBObjectExt();
    ~VBObjectExt() = default;

    bool Translate(float x, float y, float z);
    bool Rotate(float angle, float vX, float vY, float vZ);

    bool SetColor(float r, float g, float b, float a);
    bool GetColor(float out[4]) const;

    vmath::mat4 GetTransform() const;

private:
    vmath::vec3 position;
    vmath::mat4 rotation_matrix;

    vmath::vec4 color;
};