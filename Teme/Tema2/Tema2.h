#pragma once

#include "components/simple_scene.h"
#include "lab_m1/Tema2/lab_camera.h"

#define Z_FAR 200.f
#define Z_NEAR .01f
#define BACKGROUND glm::vec3(120, 120, 120)
#define CUBE_SIZE 8
#define PROJECTILE_SPEED 20
#define PLAYER_SIZE glm::vec3(10, 10, 10)
#define MAX_TIME 180
#define MAX_LIFE 200
#define PROJECTILE_DISTANCE 20
#define HALF_BACKGROUND (float)60
#define ENEMY_OFFSET 2.3
#define ENEMY_SIZE (float)8.3
#define ENEMY_DEFFORM_TIME 3
#define PLAYER_OFFSET 2
#define HALF_PLAYER 1.8f

namespace m1
{
    class Player {
    public:
        Player(glm::vec3 position);
        glm::vec3 position;
    };

    class Enemy {
    public:
        Enemy(glm::vec3 position, glm::vec2 cell);
        bool render;
        bool left;
        bool is_defformed;
        GLfloat defform_time;
        GLfloat degrees;
        glm::vec3 centre;
        glm::vec3 position;
        glm::vec2 cell;
    };

    class Projectile {
    public:
        Projectile(glm::vec3 position, GLfloat degrees);
        bool render;
        GLfloat degrees;
        glm::vec3 position;
        glm::vec3 initial_position;
    };

    class Obstacle {
    public:
        Obstacle(glm::vec3 position);
        glm::vec3 position;
        bool star;
    };

    class Tema2 : public gfxc::SimpleScene {
    public:
        Tema2();
        ~Tema2();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& model_matrix, const glm::vec3& color, Texture2D* texture);
        void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& model_matrix, const glm::vec3& color, bool is_defformed, Texture2D* texture);
        void RenderPlayer();
        void RenderEnemy(Enemy* e);
        void RenderMaze();
        bool CheckCollisionPlayerWall(GLfloat x, GLfloat y, GLfloat z);
        bool CheckCollisionPlayerObstacle(GLfloat x, GLfloat y, GLfloat z);

    protected:
        Camera *camera;
        glm::mat4 projectionMatrix;
        glm::mat4 orthoMatrix;
        bool renderCameraTarget;
        std::unordered_map<std::string, Texture2D*> textures;
        glm::vec3 camera_position_third_person;
        glm::vec3 camera_position;
        bool is_third_person;
        Player *player;
        bool attack_mode;
        glm::mat4 model_matrix;
        GLfloat top;
        GLfloat bottom;
        GLfloat right;
        GLfloat left;
        int mazes_no;
        int maze_idx;
        bool jump;
        bool go_forward;
        bool go_left;
        bool go_right;
        bool go_back;
        GLfloat time_elapsed;
        GLfloat score;
        GLfloat jump_time;
        GLfloat player_jump_speed;
        bool shoot;
        GLfloat fire_interval;
        std::vector<Enemy*> enemies;
        std::vector<Projectile*> projectiles;
        std::vector<Obstacle*> obstacles;
        GLfloat degrees;
        GLfloat angle;
        glm::vec2 mouse_position;
        GLfloat enemy_interval;
        GLfloat health;
        GLfloat enemy_speed;
        GLfloat obstacle_interval;
        int mazes[3][16][16] = {
        {   //Eller
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
            {0 ,0 ,1 ,0 ,0 ,0 ,1 ,0 ,0 ,0 ,0 ,0 ,1 ,0 ,0},
            {0 ,0 ,1 ,0 ,1 ,0 ,1 ,0 ,1 ,0 ,1 ,0 ,1 ,0 ,0},
            {0 ,0 ,1 ,0 ,1 ,0 ,0 ,0 ,1 ,0 ,1 ,0 ,1 ,0 ,0},
            {0 ,0 ,1 ,0 ,1 ,1 ,1 ,1 ,1 ,0 ,1 ,1 ,1 ,0 ,0},
            {0 ,0 ,0 ,0 ,1 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0},
            {0 ,1 ,1 ,0 ,1 ,0 ,1 ,0 ,1 ,0 ,1 ,1 ,1 ,1 ,0},
            {0 ,0 ,1 ,0 ,1 ,0 ,1 ,0 ,1 ,0 ,0 ,0 ,1 ,0 ,0},
            {0 ,0 ,1 ,1 ,1 ,1 ,1 ,0 ,1 ,0 ,1 ,1 ,1 ,0 ,0},
            {0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,1 ,0 ,1 ,0 ,0 ,0 ,0},
            {0 ,0 ,1 ,0 ,1 ,0 ,1 ,0 ,1 ,0 ,1 ,0 ,1 ,1 ,0},
            {0 ,0 ,1 ,0 ,1 ,0 ,1 ,0 ,1 ,0 ,0 ,0 ,1 ,0 ,0},
            {0 ,1 ,1 ,0 ,1 ,1 ,1 ,1 ,1 ,0 ,1 ,0 ,1 ,0 ,0},
            {0 ,0 ,0 ,0 ,0 ,0 ,1 ,0 ,0 ,0 ,1 ,0 ,0 ,0 ,0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
        },
        {   //Growing Tree
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0},
            {0 ,1 ,1 ,1 ,1 ,1 ,1 ,0 ,1 ,0 ,1 ,1 ,1 ,1 ,0},
            {0 ,0 ,0 ,0 ,1 ,0 ,0 ,0 ,1 ,0 ,1 ,0 ,0 ,0 ,0}, 
            {0 ,0 ,1 ,0 ,1 ,0 ,1 ,1 ,1 ,0 ,1 ,0 ,1 ,0 ,0},
            {0 ,0 ,1 ,0 ,0 ,0 ,1 ,0 ,0 ,0 ,1 ,0 ,1 ,0 ,0},
            {0 ,0 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,0 ,1 ,0 ,0},
            {0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,1 ,0 ,0 ,0 ,1 ,0 ,0},
            {0 ,0 ,1 ,1 ,1 ,1 ,1 ,0 ,1 ,1 ,1 ,1 ,1 ,0 ,0},
            {0 ,0 ,0 ,0 ,1 ,0 ,0 ,0 ,0 ,0 ,1 ,0 ,0 ,0 ,0},
            {0 ,1 ,1 ,0 ,1 ,0 ,1 ,1 ,1 ,0 ,1 ,0 ,1 ,1 ,0},
            {0 ,0 ,0 ,0 ,1 ,0 ,0 ,0 ,1 ,0 ,1 ,0 ,1 ,0 ,0},
            {0 ,0 ,1 ,1 ,1 ,1 ,1 ,0 ,1 ,1 ,1 ,0 ,1 ,0 ,0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
        },
        {   //Hunt-and-kill
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,1 ,0 ,0 ,0 ,0 ,0 ,0},
            {0 ,1 ,1 ,0 ,1 ,1 ,1 ,1 ,1 ,0 ,1 ,1 ,1 ,0 ,0},
            {0 ,0 ,0 ,0 ,1 ,0 ,0 ,0 ,0 ,0 ,1 ,0 ,1 ,0 ,0},
            {0 ,0 ,1 ,0 ,1 ,0 ,1 ,0 ,1 ,1 ,1 ,0 ,1 ,0 ,0},
            {0 ,0 ,1 ,0 ,0 ,0 ,1 ,0 ,0 ,0 ,0 ,0 ,1 ,0 ,0},
            {0 ,0 ,1 ,1 ,1 ,0 ,1 ,1 ,1 ,0 ,1 ,1 ,1 ,1 ,0},
            {0 ,0 ,1 ,0 ,1 ,0 ,1 ,0 ,0 ,0 ,1 ,0 ,0 ,0 ,0},
            {0 ,0 ,1 ,0 ,1 ,0 ,1 ,1 ,1 ,1 ,1 ,0 ,1 ,0 ,0},
            {0 ,0 ,0 ,0 ,1 ,0 ,0 ,0 ,1 ,0 ,0 ,0 ,1 ,0 ,0},
            {0 ,1 ,1 ,1 ,1 ,1 ,1 ,0 ,1 ,0 ,1 ,1 ,1 ,0 ,0},
            {0 ,0 ,0 ,0 ,1 ,0 ,0 ,0 ,0 ,0 ,1 ,0 ,0 ,0 ,0},
            {0 ,0 ,1 ,0 ,1 ,0 ,1 ,1 ,1 ,1 ,1 ,0 ,1 ,0 ,0},
            {0 ,0 ,1 ,0 ,0 ,0 ,1 ,0 ,0 ,0 ,0 ,0 ,1 ,0 ,0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
        }
        };
    };
}   // namespace m1
