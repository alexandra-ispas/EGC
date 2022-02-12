#pragma once

#include "components/simple_scene.h"
#include "components/transform.h"

#include <Windows.h>
#include <mmsystem.h>
#include <playsoundapi.h>
#include <thread>
#pragma comment(lib, "winmm.lib")

#define Z_FAR 200.f
#define Z_NEAR .01f
#define FLOOR_SIZE FLOOR_SQUARE * 8
#define FLOOR_SQUARE 2
#define DANCERS_NO 14
#define CONE_HEIGHT FLOOR_SIZE / 3

#define NUM_TRIANGLES 50
#define TWO_PI 6.28318530718f

namespace m1
{
    class Cone {
    public:
        Cone(glm::vec3 color, glm::vec3 position) {
            this->color = color;
            this->position = position;
        }
        glm::vec3 color;
        glm::vec3 position;
    };
    class Dancer {
    public:
        Dancer(glm::vec3 position, glm::vec3 light_positions[9], glm::vec3 colors[9]) {
            this->position = position;
            this->color = glm::vec3(rand() % (UINT8_MAX + 1) / 255.f, rand() % (UINT8_MAX + 1) / 255.f, rand() % (UINT8_MAX + 1) / 255.f);
            memcpy(this->light_colors, colors, 9 * sizeof(glm::vec3));
            memcpy(this->light_positions, light_positions, 9 * sizeof(glm::vec3));
            this->angle = RADIANS(0);
        }
        glm::vec3 light_colors[9];
        glm::vec3 light_positions[9];
        glm::vec3 position;
        glm::vec3 color;
        GLfloat angle;
    };
    class Wall {
    public:
        Wall(glm::vec3 position, glm::vec3 color, GLfloat angle, glm::vec3 light_position) {
            this->position = position;
            this->color = color;
            this->angle = angle;
            this->light_position = light_position;
        }
        glm::vec3 position;
        glm::vec3 light_direction;
        glm::vec3 light_position;
        glm::vec3 color;
        GLfloat angle;
    };

    class Floor {
    public:
        Floor(glm::vec3 position, glm::vec3 color) {
            this->position = position;
            this->color = color;
        }
        glm::vec3 position;
        glm::vec3 color;
    };

    class Tema3 : public gfxc::SimpleScene {
    public:
            Tema3();
            ~Tema3();

    private:
        void Init() override;
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;
        void OnKeyPress(int key, int mods) override;

        Texture2D* CreateRandomTexture(unsigned int width, unsigned int height);
        void RenderMesh(Mesh *mesh, 
            Shader *shader, 
            glm::mat4 modelMatrix, 
            glm::vec3 color,
            glm::vec3 obj_col,
            glm::vec3 lightPosition,  
            glm::vec3 light_positions[9], 
            glm::vec3 light_colors[9], 
            Texture2D *texture, 
            int element);

        Mesh* CreateCone();
    protected:
        glm::vec3 positions[4]; // for spotlights
        glm::vec3 colors[4];

        Floor* lights_floor[8][8];
        std::vector<Wall*> wall_columns;
        std::vector<Dancer*> dancers;
        std::vector<Cone*> spotlights;
        std::unordered_map<std::string, Texture2D*> textures;
        bool are_spotlights;
        GLint mode;
        GLfloat time_elapsed;
        unsigned int materialShininess;
        float materialKd;
        GLfloat cut_off_radians;
    };
}   // namespace m1
