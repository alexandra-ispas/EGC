#include "lab_m1/Tema2/Tema2.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;

Player::Player(glm::vec3 position) {
    this->position = position;
}

Enemy::Enemy(glm::vec3 position, glm::vec2 cell) {
    this->centre = position;
    this->render = true;
    this->position = position + glm::vec3(-(float)CUBE_SIZE / 2 + ENEMY_OFFSET, 0, -(float)CUBE_SIZE / 2 + ENEMY_OFFSET);
    this->degrees = 0;
    this->left = false;
    this->cell = cell;
    this->is_defformed = false;
    this->defform_time = 0;
}

Projectile::Projectile(glm::vec3 position, GLfloat degrees) {
    this->position = position;
    this->initial_position = position;
    this->render = true;
    this->degrees = degrees;
}

Obstacle::Obstacle(glm::vec3 position) {
    this->position = position;
    this->star = true;
}

Tema2::Tema2()
{
    bottom = -5;
    top = 5;
    right = 5.f;
    left = -5.f;
    camera_position_third_person = glm::vec3(0, 70, 3.5f);
    camera_position = glm::vec3(0, 2, 3.5f);
    camera = new Camera();
    is_third_person = true;
    attack_mode = false;
    mazes_no = 3;
    go_forward = false;
    go_left = true;
    go_right = false;
    go_back = false;
    jump = false;
    time_elapsed = 0;
    shoot = false;
    fire_interval = 0;
    enemy_interval = 0;
    health = MAX_LIFE;
    enemy_speed = 1.8f;
    score = 0.f;
    obstacle_interval = 0.f;
    player_jump_speed = 20.f;
    angle = 0;
    degrees = RADIANS(90);
}

Tema2::~Tema2()
{
    delete camera;
}

void Tema2::Init()
{
    renderCameraTarget = false;
    camera->Set(glm::vec3(0.f, 3.f, 10.f),glm::vec3(0.f, 1.f, 0.f),glm::vec3(0.f, 1.f, 0.f));
    
	{
        meshes["sphere"] = new Mesh("sphere");
        meshes["sphere"]->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");

        meshes["star"] = new Mesh("star");
        meshes["star"]->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "star.obj");

        meshes["box"] = new Mesh("box");
        meshes["box"]->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
    }

    {
		textures["background"] = new Texture2D();
		textures["background"]->Load2D(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "Textures", "death_star.jpg").c_str(), GL_REPEAT);

        textures["crate"] = new Texture2D();
		textures["crate"]->Load2D(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "Textures", "crate.jpg").c_str(), GL_REPEAT);

        textures["fire"] = new Texture2D();
        textures["fire"]->Load2D(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "Textures", "fire.jpg").c_str(), GL_REPEAT);

        textures["beige"] = new Texture2D();
		textures["beige"]->Load2D(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "Textures", "beige.jpg").c_str(), GL_REPEAT);
	
        textures["bluetubes"] = new Texture2D();
		textures["bluetubes"]->Load2D(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "Textures", "bluetubes.jpg").c_str(), GL_REPEAT);
    
        textures["pants"] = new Texture2D();
		textures["pants"]->Load2D(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "Textures", "pants.jpg").c_str(), GL_REPEAT);

        textures["wall"] = new Texture2D();
		textures["wall"]->Load2D(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "Textures", "wall.jpg").c_str(), GL_REPEAT);
    
        textures["indicator"] = new Texture2D();
        textures["indicator"]->Load2D(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "Textures", "indicator.png").c_str(), GL_REPEAT);

        textures["shiny"] = new Texture2D();
        textures["shiny"]->Load2D(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "Textures", "shiny.jpg").c_str(), GL_REPEAT);

        textures["projectile"] = new Texture2D();
        textures["projectile"]->Load2D(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "Textures", "projectile.jpg").c_str(), GL_REPEAT);
    }

	{
		Shader* shader = new Shader("TextureShaderTema2");
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2",  "shaders", "TextureVertexShader.glsl"), GL_VERTEX_SHADER);
		shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "TextureFragmentShader.glsl"), GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

    /* Choose maze */
    maze_idx = rand() % mazes_no;
    cout << "========================================================" << endl;
    cout << "==== Maze " << maze_idx << " was chosen =================" << endl;
    cout << "========================================================" << endl;

    /* Choose player position */   
    int i = 3 + rand() % 9;
    int z = 3 + rand() % 9;
    while(mazes[maze_idx][i][z] != 0) {
        i = 3 + rand() % 9;
        z = 3 + rand() % 9;
    }
    player = new Player(glm::vec3(-HALF_BACKGROUND + (2 * i + 1) * (float)CUBE_SIZE / 2, 6, -HALF_BACKGROUND + (2 * z + 1) * (float)CUBE_SIZE / 2));
    
    camera->Set(camera_position_third_person, glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, Z_NEAR, Z_FAR);
}

void Tema2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(.0f, .0f, .0f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// Sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Tema2::Update(float deltaTimeSeconds)
{
    time_elapsed += deltaTimeSeconds;
    fire_interval += deltaTimeSeconds;
    obstacle_interval += deltaTimeSeconds;
    angle += deltaTimeSeconds * 5;

    /* Check if it's time to stop the game */
    {
        /* Check if the player won = collision between an outer cell and the player */
        bool player_won = false;
        for (int i = 0; i < 15 && !player_won; i++) {
            for (int j = 0; j < 15 && !player_won; j++) {
                if ((i == 0 || i == 14 || j == 0 || j == 14) &&
                    player->position.x - HALF_PLAYER <= -HALF_BACKGROUND + (2 * i + 2) * (float)CUBE_SIZE / 2 &&
                    player->position.x + HALF_PLAYER >= -HALF_BACKGROUND + (2 * i) * (float)CUBE_SIZE / 2 &&
                    /*player->position.y - 6 <= CUBE_SIZE &&
                    player->position.y + HALF_PLAYER >= 0 &&*/
                    player->position.z - HALF_PLAYER <= (2 * j + 2) * (float)CUBE_SIZE / 2 - HALF_BACKGROUND &&
                    player->position.z + HALF_PLAYER >= (2 * j) * (float)CUBE_SIZE / 2 - HALF_BACKGROUND) {
                    player_won = true;
                }
            }
        }

        if (time_elapsed >= MAX_TIME || health <= 0 || player_won) {
            if (time_elapsed >= MAX_TIME || health <= 0) {
                cout << "=================================================" << endl;
                cout << "=======GAME OVER=================================" << endl;
                cout << "=================================================" << endl;
            }
            if (player_won) {
                cout << "=================================================" << endl;
                cout << "=======YOU WON=================================" << endl;
                cout << "=================================================" << endl;
            }

            for (Enemy* e : enemies) {
                delete e;
            }

            for (Projectile* p : projectiles) {
                delete p;
            }
            for (Obstacle* o : obstacles) {
                delete o;
            }
            exit(0);
        }
    }

    {
        /* Deciding the color of the indicator */
        glm::vec3 color;
        GLfloat time_percent = (float)(MAX_TIME - time_elapsed) / MAX_TIME;
        if (time_percent >= 0.66f)
            color = glm::vec3(.0f, 1.f, .0f);
        else if (time_percent >= 0.33f)
            color = glm::vec3(.9f, .9f, .0f);
        else
            color = glm::vec3(.9f, .0f, .0f);

        /* Outer rectangle for time */
        glm::mat4 model_matrix = glm::mat4(1);
        model_matrix = glm::translate(model_matrix, glm::vec3(0 + 4.5, 2, -3.01));
        model_matrix = glm::scale(model_matrix, glm::vec3(.5f, 5.f, .01f));
        RenderSimpleMesh(meshes["box"], shaders["TextureShaderTema2"], model_matrix, glm::vec3(1, 1, 1), textures["indicator"]);

        /* Inner rectangle for time */
        model_matrix = glm::mat4(1);
        model_matrix = glm::translate(model_matrix, glm::vec3(0 + 4.5, 2 - 2.5f * (1 - time_percent), -3));
        model_matrix = glm::scale(model_matrix, glm::vec3(.5f, (float)5.f * time_percent, .01f));
        RenderSimpleMesh(meshes["box"], shaders["TextureShaderTema2"], model_matrix, color, textures["indicator"]);

        /* Deciding the color of the indicator */
        GLfloat life_percent = (float)(health) / MAX_LIFE;
        if (life_percent >= 0.66f)
            color = glm::vec3(.0f, 1.f, .4f);
        else if (life_percent >= 0.33f)
            color = glm::vec3(.9f, .9f, .0f);
        else
            color = glm::vec3(.9f, .0f, .0f);

        /* Outer rectangle for health */
        model_matrix = glm::mat4(1);
        model_matrix = glm::translate(model_matrix, glm::vec3(0 - 4.5, 2, -3.01));
        model_matrix = glm::scale(model_matrix, glm::vec3(.5f, 5.f, .01f));
        RenderSimpleMesh(meshes["box"], shaders["TextureShaderTema2"], model_matrix, glm::vec3(1, 1, 1), textures["indicator"]);

        /* Inner rectangle for life */
        model_matrix = glm::mat4(1);
        model_matrix = glm::translate(model_matrix, glm::vec3(0 - 4.5, 2.f - 2.5f * (1 - life_percent), -3));
        model_matrix = glm::scale(model_matrix, glm::vec3(.5f, (float)5.f * life_percent, .01f));
        RenderSimpleMesh(meshes["box"], shaders["TextureShaderTema2"], model_matrix, color, textures["indicator"]);

        /* Clear color from shader */
        GLint obj_color = glGetUniformLocation(shaders["TextureShaderTema2"]->program, "object_color");
        glUniform3fv(obj_color, 1, glm::value_ptr(glm::vec3(0, 0, 0)));
    }

    /* Create projectile */
    if(shoot && fire_interval >= 0.5 && attack_mode) {
        Projectile *p = new Projectile( player->position, degrees + M_PI - M_PI / 2);
        projectiles.push_back(p);
        fire_interval = 0;
        shoot = false;
    }

    /* Add a new crate with a star over it */
    if(obstacle_interval >= 7) {
        /* Choose obstacle position */
        int i = rand() % 15;
        int z = rand() % 15;
        while (mazes[maze_idx][i][z] != 0) {
            i = rand() % 15;
            z = rand() % 15;
        }
        mazes[maze_idx][i][z] = 3;
        obstacles.push_back(new Obstacle(glm::vec3(-HALF_BACKGROUND + (2 * i + 1) * (float)CUBE_SIZE / 2, 1.5, 
                                        -HALF_BACKGROUND + (2 * z + 1) * (float)CUBE_SIZE / 2)));
        obstacle_interval = 0.f;
    }
    for (Obstacle *o : obstacles) {
        /* Render crate */
        model_matrix = glm::mat4(1);
        model_matrix = glm::translate(model_matrix, o->position);
        model_matrix = glm::scale(model_matrix, glm::vec3(CUBE_SIZE, 3, CUBE_SIZE));
        RenderSimpleMesh(meshes["box"], shaders["TextureShaderTema2"], model_matrix, glm::vec3(0, 0, 0), false, textures["crate"]);

        /* If there is no star above the crate */
        if (!o->star) continue;

        /*Check for collision between star and the player. Sphere vs AABB */
        GLfloat x = max(player->position.x - HALF_PLAYER, min(o->position.x, player->position.x + HALF_PLAYER));
        GLfloat y = max(player->position.y - HALF_PLAYER, min(o->position.y + 4, player->position.y + HALF_PLAYER / 2));
        GLfloat z = max(player->position.z - HALF_PLAYER, min(o->position.z, player->position.z + HALF_PLAYER));
        GLfloat distance = sqrt((x - o->position.x) * (x - o->position.x) + (y - o->position.y) * (y - o->position.y) + (z - o->position.z) * (z - o->position.z));
        if (distance < 5) {
            o->star = false;
            time_elapsed = max(time_elapsed - deltaTimeSeconds * 3, 0.f);
        }
        if (!o->star) continue;
        model_matrix = glm::mat4(1);
        model_matrix = glm::translate(model_matrix, o->position + glm::vec3(0, 4, 0));
        model_matrix = glm::rotate(model_matrix, angle, glm::vec3(0, 1, 0));
        model_matrix = glm::rotate(model_matrix, RADIANS(90), glm::vec3(0, 0, 1));
        model_matrix = glm::scale(model_matrix, glm::vec3(0.3, 0.3, 0.3));
        RenderSimpleMesh(meshes["star"], shaders["TextureShaderTema2"], model_matrix, glm::vec3(1.000, 0.843, 0.000), false, NULL);
    }
    /* The player jumps over an obstacle */
    if (jump) {
        if (time_elapsed - jump_time <= .5f)
            player->position.y += player_jump_speed * deltaTimeSeconds;
        else {
            /* Do not get down on the ground if the player is above an obstacle */
            if (!CheckCollisionPlayerObstacle(player->position.x, 
                player->position.y - player_jump_speed * deltaTimeSeconds, player->position.z))

                player->position.y -= player_jump_speed * deltaTimeSeconds;
        }
        if (time_elapsed - jump_time >= 1.f) {
            jump = false;
            player->position.y = 6;
            /* If the player is above an obstacle, it will stand on the crate */
            if (CheckCollisionPlayerObstacle(player->position.x, player->position.y, player->position.z))
                player->position.y += 3;
        }
        /*camera->TranslateUpward(player->position.y - camera_position_third_person.y + 7.f);
        camera_position_third_person.y = player->position.y + 7.f;*/
    }
    RenderPlayer();
    RenderMaze();
    {
        /* Add a new enemy */
        enemy_interval += deltaTimeSeconds;
        if (enemy_interval >= 3) {
            /* Choose enemy position */
            int i = rand() % 15;
            int z = rand() % 15;
            while (mazes[maze_idx][i][z] != 0) {
                i = rand() % 15;
                z = rand() % 15;
            }
            mazes[maze_idx][i][z] = 2;
            Enemy* e = new Enemy(glm::vec3(-HALF_BACKGROUND + (2 * i + 1) * (float)CUBE_SIZE / 2, 7,
                                           -HALF_BACKGROUND + (2 * z + 1) * (float)CUBE_SIZE / 2), glm::vec2(i, z));
            enemies.push_back(e);
            enemy_interval = 0;
        }
    }

    for(Enemy *e : enemies) {
        if (!e->render) continue;
        if (e->is_defformed) {
            e->defform_time += deltaTimeSeconds;
            if (e->defform_time >= ENEMY_DEFFORM_TIME) {
                e->render = false;
                continue;
            }
        }
        /* Make the enemy move on the edges of its cell */
        if (!e->left) {
            float x = e->position.x + deltaTimeSeconds * enemy_speed;
            if (x >= (float)e->centre.x + (float)CUBE_SIZE / 2 - ENEMY_OFFSET) {
                float z = e->position.z + deltaTimeSeconds * enemy_speed;
                if (z >= (float)e->centre.z + (float)CUBE_SIZE / 2 - ENEMY_OFFSET) 
                    e->left = true;
                else 
                    e->position.z += deltaTimeSeconds * enemy_speed;
            }
            else 
                e->position.x = x;
        }
        else {
            float x = e->position.x - deltaTimeSeconds * enemy_speed;
            if (x <= (float)e->centre.x - (float)CUBE_SIZE / 2 + ENEMY_OFFSET) {
                float z = e->position.z - deltaTimeSeconds * enemy_speed;
                if (z <= (float)e->centre.z - (float)CUBE_SIZE / 2 + ENEMY_OFFSET) 
                    e->left = false;
                else 
                    e->position.z -= deltaTimeSeconds * enemy_speed;
            }
            else 
                e->position.x -= deltaTimeSeconds * enemy_speed;
        }

        /* Check for collision between player and enemy: AABB vs AABB*/
        if (player->position.x - HALF_PLAYER <= e->position.x + ENEMY_OFFSET &&
            player->position.x + HALF_PLAYER >= e->position.x - ENEMY_OFFSET &&
            player->position.y - HALF_PLAYER <= e->position.y + ENEMY_OFFSET &&
            player->position.y + HALF_PLAYER >= e->position.y - ENEMY_OFFSET &&
            player->position.z - HALF_PLAYER <= e->position.z + ENEMY_OFFSET &&
            player->position.z + HALF_PLAYER >= e->position.z - ENEMY_OFFSET) {

            e->render = false;
            mazes[maze_idx][(int)e->cell.x][(int)e->cell.y] = 0;
            health -= 20;
            score -= 10;

            cout << "=================================================" << endl;
            cout << "======= SCORE: " << score << "============================ " << endl;
            cout << "=================================================" << endl;
        }
        if (!e->render) continue;
        RenderEnemy(e);
    }

    for (Projectile* p : projectiles) {
        if (!p->render) continue;
        p->position.x += deltaTimeSeconds * cos(p->degrees) * PROJECTILE_SPEED;
        p->position.z += deltaTimeSeconds * sin(p->degrees) * PROJECTILE_SPEED;

        /* Check the distance travelled by the projectile using the Euclidian distance */
        if (sqrt((p->initial_position.x - p->position.x) * (p->initial_position.x - p->position.x) +
            (p->initial_position.y - p->position.y) * (p->initial_position.y - p->position.y) +
            (p->initial_position.z - p->position.z) * (p->initial_position.z - p->position.z)) > PROJECTILE_DISTANCE) {
            p->render = false;
            continue;
        }
        if (!p->render) continue;

        /* Projectile gets out of the map */
        if (p->position.x > HALF_BACKGROUND || 
            p->position.x < -HALF_BACKGROUND || 
            p->position.z > HALF_BACKGROUND || 
            p->position.z < -HALF_BACKGROUND) {
            p->render = false;
        }
        if (!p->render) continue;

        /* Check collision between projectile and a wall: Sphere vs AABB*/
        for (int i = 0; i < 15 && p->render; i++) {
            for (int j = 0; j < 15 && p->render; j++) {
                if (mazes[maze_idx][i][j] == 1) {
                    float x = max(-HALF_BACKGROUND + (2 * i) * (float)CUBE_SIZE / 2, 
                                min(p->position.x, -HALF_BACKGROUND + (2 * i + 2) * (float)CUBE_SIZE / 2));
                    float y = max(0.f, min(p->position.y, (float)CUBE_SIZE));
                    float z = max((2 * j) * (float)CUBE_SIZE / 2 - HALF_BACKGROUND, 
                                min(p->position.z, (2 * j + 2) * (float)CUBE_SIZE / 2 - HALF_BACKGROUND));

                    float distance = sqrt((x - p->position.x) * (x - p->position.x) +
                                        (y - p->position.y) * (y - p->position.y) +
                                        (z - p->position.z) * (z - p->position.z));

                    if (distance < 1) {
                        p->render = false;
                        break;
                    }
                }
            }
        }
        if (!p->render) continue;

        /* Check collision between projectile and enemy: Sphere vs AABB*/
        for (Enemy* e : enemies) {
            if (!e->render) continue;
            GLfloat x = max(e->position.x - ENEMY_SIZE, min(p->position.x, e->position.x + ENEMY_SIZE));
            GLfloat y = max(e->position.y - ENEMY_SIZE, min(p->position.y, e->position.y + ENEMY_SIZE / 4));
            GLfloat z = max(e->position.z - ENEMY_SIZE / 4, min(p->position.z, e->position.z + ENEMY_SIZE / 4));

            GLfloat distance = sqrt((x - p->position.x) * (x - p->position.x) +
                    (y - p->position.y) * (y - p->position.y) +
                    (z - p->position.z) * (z - p->position.z));

            if (distance < 1) {
                p->render = false;
                e->is_defformed = true;
                fire_interval = 0.5;
                mazes[maze_idx][(int)e->cell.x][(int)e->cell.y] = 0;
                score += 20;
                break;
            }
        }
        if (!p->render) continue;
        model_matrix = glm::mat4(1);
        model_matrix = glm::translate(model_matrix, p->position);
        model_matrix = glm::scale(model_matrix, glm::vec3(2, 2, 2));
        RenderSimpleMesh(meshes["sphere"], shaders["TextureShaderTema2"], model_matrix, glm::vec3(0, 0, 0), false, textures["projectile"]);
    }

    /* Render background */
    model_matrix = glm::mat4(1);
    model_matrix = glm::translate(model_matrix, glm::vec3(0, HALF_BACKGROUND, 0));
    model_matrix = glm::scale(model_matrix, BACKGROUND);
    RenderSimpleMesh(meshes["box"], shaders["TextureShaderTema2"], model_matrix, glm::vec3(0, 0, 0), false, textures["background"]);
}

void Tema2::RenderMaze() {
    for(int i = 0; i < 15; i++) {
        for(int j = 0; j < 15; j++) {
            if(mazes[maze_idx][i][j] == 1) {
                model_matrix = glm::mat4(1);
                model_matrix = glm::translate(model_matrix, glm::vec3(-HALF_BACKGROUND + (2 * i + 1) * (float)CUBE_SIZE / 2, 1.5 * CUBE_SIZE / 2,
                    (2 * j + 1) * (float)CUBE_SIZE / 2 - HALF_BACKGROUND));
                model_matrix = glm::scale(model_matrix, glm::vec3(CUBE_SIZE, 1.5 * CUBE_SIZE, CUBE_SIZE));
                RenderSimpleMesh(meshes["box"], shaders["TextureShaderTema2"], model_matrix, glm::vec3(0, 0, 0), false, textures["wall"]);
            }
        }
    }
}

void Tema2::RenderEnemy(Enemy* e) {
    /* head */
    model_matrix = glm::mat4(1);
    model_matrix = glm::translate(model_matrix, e->position);
    model_matrix = glm::scale(model_matrix, PLAYER_SIZE - glm::vec3(7, 7, 7));
    RenderSimpleMesh(meshes["sphere"], shaders["TextureShaderTema2"], model_matrix, glm::vec3(0.957, 0.643, 0.376), e->is_defformed, NULL);

    /* body */
    model_matrix = glm::mat4(1);
    model_matrix = glm::translate(model_matrix, glm::vec3(e->position.x, e->position.y - 3.5, e->position.z));
    model_matrix = glm::scale(model_matrix, PLAYER_SIZE - glm::vec3(6, 5, 6.5));
    RenderSimpleMesh(meshes["sphere"], shaders["TextureShaderTema2"], model_matrix, glm::vec3(0, 0, 0), e->is_defformed, textures["shiny"]);

    /* left hand */
    model_matrix = glm::mat4(1);
    model_matrix = glm::translate(model_matrix, glm::vec3(e->position.x - 2.2, e->position.y - 3.3, e->position.z));
    model_matrix = glm::rotate(model_matrix, RADIANS(-20), glm::vec3(0, 0, 1));
    model_matrix = glm::scale(model_matrix, PLAYER_SIZE - glm::vec3(9, 7.5, 9));
    RenderSimpleMesh(meshes["box"], shaders["TextureShaderTema2"], model_matrix, glm::vec3(0.957, 0.643, 0.376), e->is_defformed, NULL);

    /* right hand */
    model_matrix = glm::mat4(1);
    model_matrix = glm::translate(model_matrix, glm::vec3(e->position.x + 2.2, e->position.y - 3.3, e->position.z));
    model_matrix = glm::rotate(model_matrix, RADIANS(20), glm::vec3(0, 0, 1));
    model_matrix = glm::scale(model_matrix, PLAYER_SIZE - glm::vec3(9, 7.5, 9));
    RenderSimpleMesh(meshes["box"], shaders["TextureShaderTema2"], model_matrix, glm::vec3(0.957, 0.643, 0.376), e->is_defformed, NULL);
}

void Tema2::RenderPlayer() {
    /* head */
    model_matrix = glm::mat4(1);
    model_matrix = glm::translate(model_matrix, player->position);
    model_matrix = glm::rotate(model_matrix, degrees, glm::vec3(0, 1, 0));
    model_matrix = glm::scale(model_matrix, PLAYER_SIZE - glm::vec3(9.1, 9, 9.1));
    RenderSimpleMesh(meshes["box"], shaders["TextureShaderTema2"], model_matrix, glm::vec3(0, 0, 0), false, textures["beige"]);

    /* body*/
    model_matrix = glm::mat4(1);
    model_matrix = glm::translate(model_matrix, glm::vec3(player->position.x, player->position.y - 2 , player->position.z));
    model_matrix = glm::rotate(model_matrix, degrees, glm::vec3(0, 1, 0));
    model_matrix = glm::scale(model_matrix, PLAYER_SIZE - glm::vec3(8, 7, 8.5));
    RenderSimpleMesh(meshes["box"], shaders["TextureShaderTema2"], model_matrix, glm::vec3(0, 0, 0), false, textures["bluetubes"]);

    /* left hand */
    model_matrix = glm::mat4(1);
    model_matrix = glm::translate(model_matrix, glm::vec3(player->position.x, player->position.y, player->position.z));
    model_matrix = glm::rotate(model_matrix, degrees, glm::vec3(0, 1, 0));
    model_matrix = glm::translate(model_matrix, glm::vec3(-1.3, -1.1, 0));
    model_matrix = glm::scale(model_matrix, PLAYER_SIZE - glm::vec3(9.3, 8.5, 9.3));
    RenderSimpleMesh(meshes["box"], shaders["TextureShaderTema2"], model_matrix, glm::vec3(0, 0, 0), false, textures["bluetubes"]);

    /* right hand */
    model_matrix = glm::mat4(1);
    model_matrix = glm::translate(model_matrix, glm::vec3(player->position.x, player->position.y, player->position.z));
    model_matrix = glm::rotate(model_matrix, degrees, glm::vec3(0, 1, 0));
    model_matrix = glm::translate(model_matrix, glm::vec3(1.3, -1.1, 0));
    model_matrix = glm::scale(model_matrix, PLAYER_SIZE - glm::vec3(9.3, 8.5, 9.3));
    RenderSimpleMesh(meshes["box"], shaders["TextureShaderTema2"], model_matrix, glm::vec3(0, 0, 0), false, textures["bluetubes"]);

    model_matrix = glm::mat4(1);
    model_matrix = glm::translate(model_matrix, glm::vec3(player->position.x, player->position.y, player->position.z));
    model_matrix = glm::rotate(model_matrix, degrees, glm::vec3(0, 1, 0));
    model_matrix = glm::translate(model_matrix, glm::vec3(-1.3, -2.2, 0));
    model_matrix = glm::scale(model_matrix, PLAYER_SIZE - glm::vec3(9.3, 9.5, 9.3));
    RenderSimpleMesh(meshes["box"], shaders["TextureShaderTema2"], model_matrix, glm::vec3(0, 0, 0), false, textures["beige"]);

    model_matrix = glm::mat4(1);
    model_matrix = glm::translate(model_matrix, glm::vec3(player->position.x, player->position.y, player->position.z));
    model_matrix = glm::rotate(model_matrix, degrees, glm::vec3(0, 1, 0));
    model_matrix = glm::translate(model_matrix, glm::vec3(1.3, -2.2, 0));
    model_matrix = glm::scale(model_matrix, PLAYER_SIZE - glm::vec3(9.3, 9.5, 9.3));
    RenderSimpleMesh(meshes["box"], shaders["TextureShaderTema2"], model_matrix, glm::vec3(0, 0, 0), false, textures["beige"]);

    /* Legs */
    model_matrix = glm::mat4(1);
    model_matrix = glm::translate(model_matrix, glm::vec3(player->position.x, player->position.y, player->position.z));
    model_matrix = glm::rotate(model_matrix, degrees, glm::vec3(0, 1, 0));
    model_matrix = glm::translate(model_matrix, glm::vec3(0.5, -4.4, 0));
    model_matrix = glm::scale(model_matrix, PLAYER_SIZE - glm::vec3(9.1, 8.2, 9));
    RenderSimpleMesh(meshes["box"], shaders["TextureShaderTema2"], model_matrix, glm::vec3(0, 0, 0), false, textures["pants"]);

    model_matrix = glm::mat4(1);
    model_matrix = glm::translate(model_matrix, glm::vec3(player->position.x, player->position.y, player->position.z));
    model_matrix = glm::rotate(model_matrix, degrees, glm::vec3(0, 1, 0));
    model_matrix = glm::translate(model_matrix, glm::vec3(-0.5, -4.4, 0));
    model_matrix = glm::scale(model_matrix, PLAYER_SIZE - glm::vec3(9.1, 8.2, 9));
    RenderSimpleMesh(meshes["box"], shaders["TextureShaderTema2"], model_matrix, glm::vec3(0, 0, 0), false, textures["pants"]);

    /* Fire ball to follow the player */
    model_matrix = glm::mat4(1);
    model_matrix = glm::translate(model_matrix, player->position + glm::vec3(0, 10, 0));
    model_matrix = glm::scale(model_matrix, glm::vec3(3, 3, 3));
    RenderSimpleMesh(meshes["sphere"], shaders["TextureShaderTema2"], model_matrix, glm::vec3(0, 0, 0), false, textures["fire"]);
}

void Tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& model_matrix,
    const glm::vec3& color, Texture2D* texture)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    glUseProgram(shader->program);

    GLint obj_color = glGetUniformLocation(shader->program, "object_color");
    glUniform3fv(obj_color, 1, glm::value_ptr(color));

    GLint model_location = glGetUniformLocation(shader->GetProgramID(), "Model");
    glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model_matrix));

    GLint view_location = glGetUniformLocation(shader->GetProgramID(), "View");
    glm::mat4 view_matrix = glm::lookAt(camera_position, glm::vec3(camera_position.x, camera_position.y, 0), glm::vec3(0, 1, 0));
    glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(view_matrix));

    GLint projection_location = glGetUniformLocation(shader->GetProgramID(), "Projection");
    glm::mat4 proj_matrix = glm::ortho(left, right, bottom, top, Z_NEAR, Z_FAR);
    glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(proj_matrix));

    glUniform1i(glGetUniformLocation(shader->GetProgramID(), "defform"), 0);

    glUniform1i(glGetUniformLocation(shader->GetProgramID(), "has_texture"), (texture) ? 1 : 0);

    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());

    glUniform1i(glGetUniformLocation(shader->program, "texture"), 0);

    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}


void Tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader,
    const glm::mat4& model_matrix, const glm::vec3& color, bool is_defformed, Texture2D* texture)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    glUseProgram(shader->program);

    GLint obj_color = glGetUniformLocation(shader->program, "object_color");
    glUniform3fv(obj_color, 1, glm::value_ptr(color));

    GLint model_location = glGetUniformLocation(shader->GetProgramID(), "Model");
    glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model_matrix));

    GLint view_location = glGetUniformLocation(shader->GetProgramID(), "View");
    glm::mat4 view_matrix = camera->GetViewMatrix();
    glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(view_matrix));

    GLint projection_location = glGetUniformLocation(shader->GetProgramID(), "Projection");
    glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    GLint defform_location = glGetUniformLocation(shader->GetProgramID(), "defform");
    GLint defform = (is_defformed) ? 1 : 0;
    glUniform1i(defform_location, defform);

    GLint time_location = glGetUniformLocation(shader->GetProgramID(), "time");
    glUniform1f(time_location, time_elapsed * INT16_MAX);

    glUniform1i(glGetUniformLocation(shader->GetProgramID(), "has_texture"), (texture) ? 1 : 0);

    if (texture)
    {
        glActiveTexture(GL_TEXTURE0);

        glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());

        glUniform1i(glGetUniformLocation(shader->program, "texture"), 0);
    }

    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

bool Tema2::CheckCollisionPlayerWall(GLfloat x, GLfloat y, GLfloat z) {
    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 15; j++) {
            if (mazes[maze_idx][i][j] == 1 &&
                x - HALF_PLAYER <= -HALF_BACKGROUND + (2 * i + 2) * (float)CUBE_SIZE / 2 &&
                x + HALF_PLAYER >= -HALF_BACKGROUND + (2 * i) * (float)CUBE_SIZE / 2 &&
                y - 6 <= CUBE_SIZE &&
                y + HALF_PLAYER >= 0 &&
                z - HALF_PLAYER <= (2 * j + 2) * (float)CUBE_SIZE / 2 - HALF_BACKGROUND &&
                z + HALF_PLAYER >= (2 * j) * (float)CUBE_SIZE / 2 - HALF_BACKGROUND) {
                return true;
            }
        }
    }
    return false;
}

bool Tema2::CheckCollisionPlayerObstacle(GLfloat x, GLfloat y, GLfloat z) {
    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 15; j++) {
            if (mazes[maze_idx][i][j] == 3 &&
                x - HALF_PLAYER <= -HALF_BACKGROUND + (2 * i + 2) * (float)CUBE_SIZE / 2 &&
                x + HALF_PLAYER >= -HALF_BACKGROUND + (2 * i) * (float)CUBE_SIZE / 2 &&
                y - 6 + HALF_PLAYER <= 3 &&
                y + HALF_PLAYER >= 0 &&
                z - HALF_PLAYER <= (2 * j + 2) * (float)CUBE_SIZE / 2 - HALF_BACKGROUND &&
                z + HALF_PLAYER >= (2 * j) * (float)CUBE_SIZE / 2 - HALF_BACKGROUND) {
                return true;
            }
        }
    }
    return false;
}

void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    float cameraSpeed = 20.0f;
    if (window->KeyHold(GLFW_KEY_W)) {
        GLfloat z = player->position.z - deltaTime * cameraSpeed * 0.8;
        /* Check if the player gets out of the map */
        if (z + PLAYER_OFFSET >= HALF_BACKGROUND || z - PLAYER_OFFSET <= -HALF_BACKGROUND) return;
        /* Rotate the player if it changes direction */
        if (!attack_mode) {
            if (go_left || go_right)
                degrees += RADIANS(90);
            go_left = false;
            go_right = false;
            go_back = false;
            go_forward = true;
        }
        /* Check collision between player and a wall: AABB vs AABB*/
        if (CheckCollisionPlayerWall(player->position.x, player->position.y, z)) return;
        /* Check collision between player and a obstacles: AABB vs AABB*/
        if (CheckCollisionPlayerObstacle(player->position.x, player->position.y, z)) return;
        /* Get down from an obstacle */
        if (!CheckCollisionPlayerObstacle(player->position.x, 7 - HALF_PLAYER, z))
            player->position.y = 7.f;

        player->position -= glm::vec3(0.f, 0, 0.8) * deltaTime * cameraSpeed;
        if (camera_position_third_person.z - deltaTime * cameraSpeed * 0.8 <= HALF_BACKGROUND &&
            camera_position_third_person.z - deltaTime * cameraSpeed * 0.8 >= -HALF_BACKGROUND) {
            camera->TranslateForward(deltaTime * cameraSpeed * 0.8);
            camera_position_third_person.z -= deltaTime * cameraSpeed * 0.8;
        }
    }
        
    if (window->KeyHold(GLFW_KEY_A)) {
        GLfloat x = player->position.x - deltaTime * cameraSpeed * 0.8;
        /* Check if the player gets out of the map */
        if (x + PLAYER_OFFSET >= HALF_BACKGROUND || x - PLAYER_OFFSET <= -HALF_BACKGROUND) return;
        /* Rotate the player if it changes direction */
        if (!attack_mode) {
            if (go_forward || go_back)
                degrees += RADIANS(90);
            go_left = true;
            go_right = false;
            go_forward = false;
            go_back = false;
        }
        /* Check collision between player and a wall: AABB vs AABB*/
        if (CheckCollisionPlayerWall(x, player->position.y, player->position.z)) return;
        /* Check collision between player and a obstacles: AABB vs AABB*/
        if (CheckCollisionPlayerObstacle(x, player->position.y, player->position.z)) return;
        /* Get down from an obstacle */
        if (!CheckCollisionPlayerObstacle(x, 7 - HALF_PLAYER, player->position.z))
            player->position.y = 7.f;

        player->position -= glm::vec3(0.8, 0, 0) * deltaTime * cameraSpeed;
        if (camera_position_third_person.x - deltaTime * cameraSpeed * 0.8 <= HALF_BACKGROUND &&
            camera_position_third_person.x - deltaTime * cameraSpeed * 0.8 >= -HALF_BACKGROUND) {
            camera->TranslateRight(-deltaTime * cameraSpeed * 0.8);
            camera_position_third_person.x -= deltaTime * cameraSpeed * 0.8;
        }
    }

    if (window->KeyHold(GLFW_KEY_S)) {
        GLfloat z = player->position.z + deltaTime * cameraSpeed * 0.8;
        /* Check if the player gets out of the map */
        if (z + PLAYER_OFFSET >= HALF_BACKGROUND || z - PLAYER_OFFSET <= -HALF_BACKGROUND) return;

        /* Rotate the player if it changes direction */
        if (!attack_mode) {
            if (go_left || go_right)
                degrees += RADIANS(90);
            go_left = false;
            go_right = false;
            go_forward = false;
            go_back = true;
        }
        /* Check collision between player and a wall: AABB vs AABB*/
        if (CheckCollisionPlayerWall(player->position.x, player->position.y, z)) return;
        /* Check collision between player and a obstacles: AABB vs AABB*/
        if (CheckCollisionPlayerObstacle(player->position.x, player->position.y, z)) return;
        /* Get down from an obstacle */
        if (!CheckCollisionPlayerObstacle(player->position.x, 7 - HALF_PLAYER, z))
            player->position.y = 7.f;
        player->position += glm::vec3(0.f, 0, 0.8) * deltaTime * cameraSpeed;
        if (camera_position_third_person.z + deltaTime * cameraSpeed * 0.8 <= HALF_BACKGROUND &&
            camera_position_third_person.z + deltaTime * cameraSpeed * 0.8 >= -HALF_BACKGROUND) {
            camera->TranslateForward(-deltaTime * cameraSpeed * 0.8);
            camera_position_third_person.z += deltaTime * cameraSpeed * 0.8;
        }
    }

    if (window->KeyHold(GLFW_KEY_D)) {
        GLfloat x = player->position.x + deltaTime * cameraSpeed * 0.8;
        /* Check if the player gets out of the map */
        if (x + PLAYER_OFFSET >= HALF_BACKGROUND || x - PLAYER_OFFSET <= -HALF_BACKGROUND) return;
        /* Rotate the player if it changes direction */
        if (!attack_mode) {
            if(go_forward || go_back)
                degrees += RADIANS(90);
            go_left = false;
            go_right = true;
            go_forward = false;
            go_back = false;
        }
        /* Check collision between player and a wall: AABB vs AABB*/
        if (CheckCollisionPlayerWall(x, player->position.y, player->position.z)) return;
        /* Check collision between player and a obstacles: AABB vs AABB*/
        if (CheckCollisionPlayerObstacle(x, player->position.y, player->position.z)) return;
        /* Get down from an obstacle */
        if (!CheckCollisionPlayerObstacle(x, 7 - HALF_PLAYER, player->position.z))
            player->position.y = 7.f;

        player->position += glm::vec3(0.8, 0, 0) * deltaTime * cameraSpeed;
        if (camera_position_third_person.x + deltaTime * cameraSpeed * 0.8 <= HALF_BACKGROUND &&
            camera_position_third_person.x + deltaTime * cameraSpeed * 0.8 >= -HALF_BACKGROUND) {
            camera->TranslateRight(deltaTime * cameraSpeed * 0.8);
            camera_position_third_person.x += deltaTime * cameraSpeed * 0.8;
        }
    }

    if (window->KeyHold(GLFW_KEY_Q)) {
        camera->TranslateUpward(-deltaTime * cameraSpeed);
        camera_position_third_person.y -= deltaTime * cameraSpeed * 0.8;
    }

    if (window->KeyHold(GLFW_KEY_E)) {
        camera->TranslateUpward(deltaTime * cameraSpeed);
        camera_position_third_person.y += deltaTime * cameraSpeed * 0.8;
    }

    if (window->KeyHold(GLFW_KEY_SPACE) && !jump) {
        jump = true;
        jump_time = time_elapsed;
    }
}

void Tema2::OnKeyPress(int key, int mods)
{
    if (key == GLFW_KEY_LEFT_CONTROL) {
        attack_mode = !attack_mode;
        degrees = M_PI/2;
        if(attack_mode) {
            glm::vec3 new_camera_position = player->position;
            glm::vec3 new_player_position = player->position;
            if (go_forward) {
                new_camera_position.z -= 13;
                new_player_position.z -= 1;
                degrees = 0;
            }
            else if (go_back) {
                new_camera_position.z += 13;
                new_player_position.z += 1;
                degrees = 0;
            }
            else if (go_left) {
                new_camera_position.x -= 13;
                new_player_position.z += 1;
            }
            else if (go_right) {
                new_camera_position.x += 13;
                new_player_position.z -= 1;
            }
            new_player_position.y += 1;
			camera->Set(new_player_position, new_camera_position, glm::vec3(0, 1, 0));
            new_camera_position.z = -10;
            camera_position_third_person = new_camera_position;
        } else {
            camera_position_third_person = player->position + glm::vec3(0, 60, 0);
            glm::vec3 new_camera_position = glm::vec3(camera_position_third_person.x, 1, 0);
			camera->Set(camera_position_third_person, new_camera_position, glm::vec3(0, 1, 0));
        }
    }
}

void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT) ) {
        float sensivityOX = 0.001f;
        float sensivityOY = 0.001f;

        if (attack_mode) {
            renderCameraTarget = false;
            camera->RotateFirstPerson_OX(-sensivityOX * deltaY);
            camera->RotateFirstPerson_OY(-sensivityOY * deltaX);
            degrees += sensivityOY * deltaX;
        } else {
            renderCameraTarget = true;
            camera->RotateThirdPerson_OX(-sensivityOX * deltaY);
            camera->RotateThirdPerson_OY(-sensivityOY * deltaX);
        }
    }
}

void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    if (window->MouseHold(GLFW_MOUSE_BUTTON_LEFT))
	    shoot = true;
}