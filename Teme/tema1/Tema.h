#pragma once

#include "components/simple_scene.h"
#include "lab_m1/tema1/Helper.h"

#define OBSTACLE_NO 10
#define SQUARE_SIDE 5
#define PROJECTILE_DISTANCE 700
#define PROJECTILE_SPPED 1500
#define BACKGROUND_SIZE 3000
#define PLAYED_RADIUS 80

#define YELLOW_COLOR glm::vec3(1.f, 1.f, 0)
#define RED_COLOR glm::vec3(1.f, 0, 0)
#define GREEN_COLOR glm::vec3(0.f, 1.f, 0.f)
#define BLUE_COLOR glm::vec3(0.118f, 0.565f, 1.000f)
#define BLACK glm::vec3(0, 0, 0)
#define ORANGE_COLOR glm::vec3(0.957, 0.643, 0.376)
#define BROWN_COLOR glm::vec3(	0.545, 0.271, 0.075)

namespace m1
{
    class SurvivalShooter : public gfxc::SimpleScene
    {
    public:
        struct ViewportSpace
        {
            ViewportSpace() : x(0), y(0), width(1), height(1) {}
            ViewportSpace(int x, int y, int width, int height)
                : x(x), y(y), width(width), height(height) {}
            int x;
            int y;
            int width;
            int height;
        };

        struct LogicSpace
        {
            LogicSpace() : x(0), y(0), width(1), height(1) {}
            LogicSpace(float x, float y, float width, float height)
                : x(x), y(y), width(width), height(height) {}
            float x;
            float y;
            float width;
            float height;
        };

    private:
        typedef struct Enemy
        {
            bool collision;
            GLfloat posX;
            GLfloat posY;
            GLfloat speed;
            GLfloat degrees;
        } Enemy;

        typedef struct Obstacle
        {
            GLfloat posX;
            GLfloat posY;
            GLfloat length;
            GLfloat height;
            Mesh* mesh;
        } Obstacle;

        typedef struct Projectile
        {
            bool collision;
            GLfloat x;
            GLfloat y;
            GLfloat degrees;
            GLfloat initialX;
            GLfloat initialY;
        } Projectile;

        typedef struct Item
        {
            GLfloat x;
            GLfloat y;
            bool render;
            Mesh* mesh;
        } Item;

    public:
        SurvivalShooter();
        ~SurvivalShooter();

        GLvoid Init() override;

    private:
        GLvoid FrameStart() override;
        GLvoid Update(float deltaTimeSeconds) override;
        GLvoid OnInputUpdate(float deltaTime, int mods) override;
        GLvoid OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        GLvoid RenderPlayer();
        GLvoid RenderEnemy(Enemy enemy, float deltaTimeSeconds);
        glm::mat3 VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace);
        GLvoid SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor = glm::vec3(0), bool clear = true);
        GLvoid OnMouseBtnPress(int mouseX, int mouseY, int button, int mods);

    protected:
        int livesNo;
        GLfloat time;
        GLfloat points;
        GLfloat score;
        GLfloat healthScale;
        GLfloat health;
        GLfloat degrees;
        Helper* helper;
        GLenum polygonMode;
        bool shoot;
        bool die;
        GLfloat obstacle_width;
        GLfloat obstacle_height;
        GLfloat square_obstacle;
        GLfloat fireInterval;
        GLfloat yellowStarTimer;
        GLfloat redStarTimer;
        GLfloat greenStarTimer;
        GLfloat player_speed;
        

        std::vector<Enemy*> enemies;
        std::vector<Obstacle> obstacles;
        std::vector<Projectile*> projectiles;
        GLfloat obstacleWidth;
        ViewportSpace viewSpace;
        LogicSpace logicSpace;
        glm::mat3 modelMatrix, visMatrix;
        std::vector<Item*> stars;
    };
}