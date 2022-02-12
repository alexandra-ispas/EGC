#pragma once

#include "components/simple_scene.h"
#include "components/transform.h"


namespace m1
{
    class Lab8 : public gfxc::SimpleScene
    {
     public:
        Lab8();
        ~Lab8();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, const glm::vec3 &color = glm::vec3(1));

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;

        glm::vec3 lightPosition;
        glm::vec3 lightDirection;
        unsigned int materialShininess;
        float materialKd;
        float materialKs;

        // TODO(student): If you need any other class variables, define them here.
        bool spotlight;
        GLfloat cut_off_radians;
    };
}   // namespace m1
