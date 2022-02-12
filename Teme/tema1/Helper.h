#pragma once
#include "components/simple_scene.h"
#include "lab_m1/tema1/Transform2D.h"
#include "core/gpu/mesh.h"
#include <core/engine.h>
#include "utils/gl_utils.h"
#include <iostream>
#include <vector>
#include <math.h>
#include <string.h>

using namespace std;

#define NUM_TRIANGLES 50
#define TWO_PI 6.28318530718f

class Helper : public gfxc::SimpleScene
{
public:
  
    Helper(glm::vec2 centre);
	~Helper();

	Mesh* CreateCircle(glm::vec3 color, string name);
    Mesh* CreateItem(glm::vec3 color, string name);
	Mesh* CreateRectangle(float width, float height, glm::vec3 color, string name);
	
    glm::vec2 centre;
    GLfloat eyeOffsetX;
    GLfloat eyeOffsetY;
    GLfloat headOffsetX;
    GLfloat headOffsetY;
};
