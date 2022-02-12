#pragma once

#include <string>
#include <unordered_map>

#include "components/simple_scene.h"
#include "components/transform.h"


namespace m1
{
    class Lab9 : public gfxc::SimpleScene
    {
     public:
        Lab9();
        ~Lab9();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, bool use_time,
            Texture2D *texture1 = NULL, Texture2D *texture2 = NULL);
        Texture2D *CreateRandomTexture(unsigned int width, unsigned int height);

        void OnInputUpdate(float deltaTime, int mods) override;
       
        std::unordered_map<std::string, Texture2D *> mapTextures;
    };
}   // namespace m1
