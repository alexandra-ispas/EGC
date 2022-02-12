#include "lab_m1/Tema3/Tema3.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;

Tema3::Tema3() {
    time_elapsed = 0.f;
    materialShininess = 20;
    materialKd = 0.8f;
    are_spotlights = false;
    cut_off_radians = RADIANS(25);
    mode = 0;
}


Tema3::~Tema3() {
}

Mesh* Tema3::CreateCone() {

    vector<VertexFormat> vertices;
    vector<unsigned int> indices;
    GLfloat arg;
    vertices.emplace_back(glm::vec3(0, 0, 0));
    for (GLushort i = 0; i < NUM_TRIANGLES; i++) {
        arg = TWO_PI * i / NUM_TRIANGLES;
        vertices.emplace_back(glm::vec3(cos(arg), -1, sin(arg)));
        indices.push_back(i);
    }
    indices.push_back(NUM_TRIANGLES);
    indices.push_back(1);

    Mesh* cone = new Mesh("cone");
    cone->InitFromData(vertices, indices);
    cone->SetDrawMode(GL_TRIANGLE_FAN);
    return cone;
}

void Tema3::Init() {

    meshes["box"] = new Mesh("box");
    meshes["box"]->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");

    meshes["quad"] = new Mesh("quad");
    meshes["quad"]->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "quad.obj");

    meshes["sphere"] = new Mesh("sphere");
    meshes["sphere"]->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");

    meshes["cone"] = CreateCone();

    textures["random"] = CreateRandomTexture(16, 16);

    shaders["Shader"] = new Shader("Shader");
	shaders["Shader"]->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3",  "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
	shaders["Shader"]->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema3", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
	shaders["Shader"]->CreateAndLink();

    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {

            // create the floor
            glm::vec3 color = glm::vec3(rand() % (UINT8_MAX + 1) / 255.f, rand() % (UINT8_MAX + 1) / 255.f, rand() % (UINT8_MAX + 1) / 255.f);
            glm::vec3 position = glm::vec3( -FLOOR_SIZE / 2 + i * FLOOR_SQUARE + FLOOR_SQUARE / 2, 0.5, 
                                            -FLOOR_SIZE / 2 + j * FLOOR_SQUARE + FLOOR_SQUARE / 2);
            lights_floor[i][j] = new Floor(position, color);
            
            // create walls from columns for the first part 
            if(i == 0) {
                wall_columns.push_back(new Wall(position - glm::vec3(FLOOR_SQUARE / 2, -FLOOR_SIZE / 2, 0), color, M_PI / 2, position));
            } 
            if(i == 7) {
                wall_columns.push_back(new Wall(position + glm::vec3(FLOOR_SQUARE / 2, FLOOR_SIZE / 2, 0), color, M_PI / 2, position));
            } 
            if(j == 0) {
                wall_columns.push_back(new Wall(position - glm::vec3(0, -FLOOR_SIZE / 2, FLOOR_SQUARE / 2), color, 0, position));
            } 
            if(j == 7) {
                wall_columns.push_back(new Wall(position + glm::vec3(0, FLOOR_SIZE / 2, FLOOR_SQUARE / 2), color, 0, position));
            }
        }
    }

    for(int i = 0; i < DANCERS_NO; i++) {
        int x = 1 + rand() % 6;
        int y = 1 + rand() % 6;

        // the 9 lights associated with each dancer

        glm::vec3 colors[9], positions[9];
        colors[0] = lights_floor[x - 1][y - 1]->color;
        colors[1] = lights_floor[x - 1][y]->color;
        colors[2] = lights_floor[x - 1][y + 1]->color;
        colors[3] = lights_floor[x][y - 1]->color;
        colors[4] = lights_floor[x][y]->color;
        colors[5] = lights_floor[x][y + 1]->color;
        colors[6] = lights_floor[x + 1][y - 1]->color;
        colors[7] = lights_floor[x + 1][y]->color;
        colors[8] = lights_floor[x + 1][y + 1]->color;

        positions[0] = lights_floor[x - 1][y-1]->position;
        positions[1] = lights_floor[x - 1][y]->position;
        positions[2] = lights_floor[x - 1][y + 1]->position;
        positions[3] = lights_floor[x][y - 1]->position;
        positions[4] = lights_floor[x][y]->position;
        positions[5] = lights_floor[x][y + 1]->position;
        positions[6] = lights_floor[x + 1][y - 1]->position;
        positions[7] = lights_floor[x + 1][y]->position;
        positions[8] = lights_floor[x + 1][y + 1]->position;

        dancers.push_back(new Dancer(lights_floor[x][y]->position + glm::vec3(0, 0.5, 0) , positions, colors));
    }
    
    // create spotlights
    {
        glm::vec3 color = glm::vec3(rand() % (UINT8_MAX + 1) / 255.f, rand() % (UINT8_MAX + 1) / 255.f, rand() % (UINT8_MAX + 1) / 255.f);
        Cone* s = new Cone(color, glm::vec3(-FLOOR_SIZE / 2 + 1.3 * FLOOR_SQUARE, CONE_HEIGHT + 0.5, -FLOOR_SIZE / 2 + 1.3 * FLOOR_SQUARE));
        spotlights.push_back(s);
        positions[0] = s->position;
        colors[0] = color;

        color = glm::vec3(rand() % (UINT8_MAX + 1) / 255.f, rand() % (UINT8_MAX + 1) / 255.f, rand() % (UINT8_MAX + 1) / 255.f);
        s = new Cone(color, glm::vec3(FLOOR_SIZE / 2 - 1.3 * FLOOR_SQUARE, CONE_HEIGHT + 0.5, -FLOOR_SIZE / 2 + 1.3 * FLOOR_SQUARE));
        spotlights.push_back(s);
        positions[1] = s->position;
        colors[1] = color;

        color = glm::vec3(rand() % (UINT8_MAX + 1) / 255.f, rand() % (UINT8_MAX + 1) / 255.f, rand() % (UINT8_MAX + 1) / 255.f);
        s = new Cone(color, glm::vec3(-FLOOR_SIZE / 2 + 1.3 * FLOOR_SQUARE, CONE_HEIGHT + 0.5, FLOOR_SIZE / 2 - 1.3 * FLOOR_SQUARE));
        spotlights.push_back(s);
        positions[2] = s->position;
        colors[2] = color;

        color = glm::vec3(rand() % (UINT8_MAX + 1) / 255.f, rand() % (UINT8_MAX + 1) / 255.f, rand() % (UINT8_MAX + 1) / 255.f);
        s = new Cone(color, glm::vec3(FLOOR_SIZE / 2 - 1.3 * FLOOR_SQUARE, CONE_HEIGHT + 0.5, FLOOR_SIZE / 2 - 1.3 * FLOOR_SQUARE));
        spotlights.push_back(s);
        positions[3] = s->position;
        colors[3] = color;
    }

    // set camera position
    GetSceneCamera()->SetPosition(glm::vec3(FLOOR_SIZE / 2, CONE_HEIGHT, FLOOR_SIZE / 2));
}

void Tema3::FrameStart() {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    glViewport(0, 0, resolution.x, resolution.y);
}

void Tema3::Update(float deltaTimeSeconds) {

    time_elapsed += deltaTimeSeconds;
    
    if (mode == 0) {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                Floor* f = lights_floor[i][j];
                glm::mat4 modelMatrix = glm::mat4(1);
                modelMatrix = glm::translate(modelMatrix, f->position);
                modelMatrix = glm::rotate(modelMatrix, RADIANS(90), glm::vec3(1, 0, 0));
                RenderMesh(meshes["quad"], shaders["Shader"], modelMatrix, f->color, f->color, f->position + glm::vec3(0, 20, 0), NULL, NULL, NULL, 1);
            }
        }

        for (Wall* w : wall_columns) {
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, w->position);
            modelMatrix = glm::rotate(modelMatrix, w->angle, glm::vec3(0, 1, 0));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(1, FLOOR_SIZE, 1));
            RenderMesh(meshes["quad"], shaders["Shader"], modelMatrix, w->color, glm::vec3(1, 1, 1), w->light_position, NULL, NULL, NULL, 2);
        }
    } else {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-FLOOR_SIZE / 2, FLOOR_SIZE / 2, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(FLOOR_SIZE/2));
        modelMatrix = glm::rotate(modelMatrix, (float)M_PI / 2, glm::vec3(0, 1, 0));
        RenderMesh(meshes["quad"], shaders["Shader"], modelMatrix, glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), glm::vec3(0, 2 * FLOOR_SIZE / 3 - 1.5, 0), NULL, NULL, textures["random"], 7);

        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(FLOOR_SIZE / 2, FLOOR_SIZE / 2, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(FLOOR_SIZE / 2));
        modelMatrix = glm::rotate(modelMatrix, (float)M_PI / 2, glm::vec3(0, 1, 0));
        RenderMesh(meshes["quad"], shaders["Shader"], modelMatrix, glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), glm::vec3(0, 2 * FLOOR_SIZE / 3 - 1.5, 0), NULL, NULL, textures["random"], 8);

        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, FLOOR_SIZE / 2, -FLOOR_SIZE / 2));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(FLOOR_SIZE / 2));
        RenderMesh(meshes["quad"], shaders["Shader"], modelMatrix, glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), glm::vec3(0, 2 * FLOOR_SIZE / 3 - 1.5, 0), NULL, NULL, textures["random"], 9);

        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, FLOOR_SIZE / 2, FLOOR_SIZE / 2));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(FLOOR_SIZE / 2));
        RenderMesh(meshes["quad"], shaders["Shader"], modelMatrix, glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), glm::vec3(0, 2 * FLOOR_SIZE / 3 - 1.5, 0), NULL, NULL, textures["random"], 10);

        modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 2 * FLOOR_SIZE / 3, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(FLOOR_SIZE/2));
        modelMatrix = glm::rotate(modelMatrix, (float)M_PI / 2, glm::vec3(1, 0, 0));
        RenderMesh(meshes["quad"], shaders["Shader"], modelMatrix, glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), glm::vec3(0, FLOOR_SIZE / 4 - 1, 0), NULL, NULL, textures["random"], 6);
   
        modelMatrix = glm::mat4(1);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(FLOOR_SIZE / 2));
        modelMatrix = glm::rotate(modelMatrix, (float)M_PI / 2, glm::vec3(1, 0, 0));
        RenderMesh(meshes["quad"], shaders["Shader"], modelMatrix, glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), glm::vec3(0, 2 * FLOOR_SIZE / 3 - 1.5, 0), NULL, NULL, textures["random"], 1);
    }

    for(Dancer* d : dancers) {
        d->angle += deltaTimeSeconds;

        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, d->position - glm::vec3(1, 0, 1));
        modelMatrix = glm::rotate(modelMatrix, d->angle, glm::vec3(0, 1, 0));
        modelMatrix = glm::translate(modelMatrix, glm::vec3(1, 0, 1));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 2, 1));
        RenderMesh(meshes["box"], shaders["Shader"], modelMatrix, glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), 
            glm::vec3(0, 2 * FLOOR_SIZE / 3 - 1.5, 0), d->light_positions, d->light_colors, textures["random"], 3);
    }

    if (are_spotlights) {
        glLineWidth(3);
        glPointSize(5);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        glDepthMask(GL_FALSE);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        for (int i = 0; i < spotlights.size(); i++) {
            Cone* s = spotlights.at(i);
            positions[i] = s->position + glm::vec3(sin(time_elapsed) * 0.5, 0, cos(time_elapsed) * 0.5);
            
            glm::mat4 modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, positions[i]);
            modelMatrix = glm::scale(modelMatrix, glm::vec3(tan(cut_off_radians) * CONE_HEIGHT, 1 * CONE_HEIGHT, tan(cut_off_radians) * CONE_HEIGHT));
            RenderMesh(meshes["cone"], shaders["Shader"], modelMatrix, s->color, s->color, s->position, NULL, NULL, NULL, 4);
        }
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glDisable(GL_CULL_FACE);
    }

    if (mode == 1) {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 2 * FLOOR_SIZE / 3 - 1.5, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(3));
        RenderMesh(meshes["sphere"], shaders["Shader"], modelMatrix, glm::vec3(0, 0, 0), glm::vec3(1, 1, 1), glm::vec3(0, 2 * FLOOR_SIZE / 3 - 1.5, 0), NULL, NULL, textures["random"], 5);
    }
}

void Tema3::FrameEnd() {}

Texture2D* Tema3::CreateRandomTexture(unsigned int width, unsigned int height) {
	GLuint textureID = 0;
	unsigned int channels = 3;
	unsigned int size = width * height * channels;
	unsigned char* data = new unsigned char[size];

	for (int i = 0; i < size; ++i) {
		data[i] = rand() % (UINT8_MAX + 1);
	}

	// Generate and bind the new texture ID
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	CheckOpenGLError();

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glGenerateMipmap(GL_TEXTURE_2D);
	CheckOpenGLError();

	Texture2D* texture = new Texture2D();
	texture->Init(textureID, width, height, channels);

	SAFE_FREE_ARRAY(data);
	return texture;
}

void Tema3::RenderMesh(Mesh* mesh,
    Shader* shader,
    glm::mat4 modelMatrix,
    glm::vec3 light_color,
    glm::vec3 obj_col,
    glm::vec3 lightPosition,
    glm::vec3 light_positions[9],
    glm::vec3 light_colors[9],
    Texture2D* texture,
    int element)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    glUseProgram(shader->program);

    int location;
    location = glGetUniformLocation(shader->program, "light_position");
    glUniform3f(location, lightPosition.x, lightPosition.y, lightPosition.z);

    location = glGetUniformLocation(shader->program, "light_color");
    glUniform3fv(location, 1, glm::value_ptr(light_color));
        
    if (are_spotlights) {

        location = glGetUniformLocation(shader->program, "spotlight_radians");
        glUniform1f(location, cut_off_radians);
    
        location = glGetUniformLocation(shader->program, "light_colors_spotlights");
        glUniform3fv(location, 4, glm::value_ptr(colors[0]));

        location = glGetUniformLocation(shader->program, "light_positions_spotlights");
        glUniform3fv(location, 4, glm::value_ptr(positions[0]));
    }

    if (element == 3 && mode == 0) {
        location = glGetUniformLocation(shader->program, "light_positions_floor");
        glUniform3fv(location, 9, glm::value_ptr(light_positions[0]));

        location = glGetUniformLocation(shader->program, "light_colors_floor");
        glUniform3fv(location, 9, glm::value_ptr(light_colors[0]));
    }
    
    if (mode == 1) {
        if (element == 7) {
            location = glGetUniformLocation(shader->program, "light_direction");
            glUniform3f(location, 1, 0, 0);
        } else if (element == 6) {
            location = glGetUniformLocation(shader->program, "light_direction");
            glUniform3f(location, 0, 1, 0);
        } else if (element == 1 || element == 3) {
            location = glGetUniformLocation(shader->program, "light_direction");
            glUniform3f(location, 0, -1, 0);
        } else if (element == 8) {
            location = glGetUniformLocation(shader->program, "light_direction");
            glUniform3f(location, 1, 0, 0);
        } else if (element == 9) {
            location = glGetUniformLocation(shader->program, "light_direction");
            glUniform3f(location, 0, 0, -1);
        } else if (element == 10) {
            location = glGetUniformLocation(shader->program, "light_direction");
            glUniform3f(location, 0, 0, 1);
        } else if (element == 4) {
            location = glGetUniformLocation(shader->program, "light_direction");
            glUniform3f(location, 0, -1, 0);
        }

        location = glGetUniformLocation(shader->program, "cut_off_radians");
        if (element == 3) {
            glUniform1f(location, RADIANS(30));
        }
        else {
            glUniform1f(location, RADIANS(60));
        }
        location = glGetUniformLocation(shader->program, "time");
        glUniform1f(location, time_elapsed);

        if (element != 4) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
            glUniform1i(glGetUniformLocation(shader->program, "texture"), 0);
        }
    }

    glm::vec3 eyePosition = GetSceneCamera()->m_transform->GetWorldPosition();
    location = glGetUniformLocation(shader->program, "eye_position");
    glUniform3f(location, eyePosition.x, eyePosition.y, eyePosition.z);

    location = glGetUniformLocation(shader->program, "material_shininess");
    glUniform1f(location, materialShininess);

    location = glGetUniformLocation(shader->program, "material_kd");
    glUniform3f(location, materialKd, materialKd, materialKd);

    location = glGetUniformLocation(shader->program, "material_ks");
    glUniform3f(location, materialKd / 2.f, materialKd / 2.f, materialKd / 2.f);

    location = glGetUniformLocation(shader->program, "elem");
    glUniform1i(location, element);

    location = glGetUniformLocation(shader->program, "mode");
    glUniform1i(location, mode);

    location = glGetUniformLocation(shader->program, "spotlight");
    glUniform1f(location, are_spotlights);

    location = glGetUniformLocation(shader->program, "object_color");
    glUniform3fv(location, 1, glm::value_ptr(obj_col));

    location = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    location = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    location = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}


void Tema3::OnKeyPress(int key, int mods) {
    // daca apasa pe P, se pornesc spotlight-urile
    if (window->KeyHold(GLFW_KEY_P)) {
        are_spotlights = !are_spotlights;
    }

    if (window->KeyHold(GLFW_KEY_O)) {
        mode++;
        mode %= 3;
        if (mode == 2) {
            are_spotlights = true;
        }
    }
    if (window->KeyHold(GLFW_KEY_L)) {
        exit(0);
    }
}

