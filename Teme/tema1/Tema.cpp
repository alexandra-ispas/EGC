#include "lab_m1/tema1/Tema.h"

#include <vector>
#include <iostream>


using namespace std;
using namespace m1;


SurvivalShooter::SurvivalShooter()
{
    livesNo = 4;
    time = 0.f;
    polygonMode = GL_FILL;
    score = 0.f;
    degrees = 0;
    points = 0;
    obstacleWidth = 80.f;
    health = 3000.f;
    healthScale = 1.f;
	shoot = false;
	die = false;
	obstacle_height = obstacleWidth * 6 / 4.f;
	obstacle_width = obstacleWidth * 9;
	square_obstacle = 600.f;
	fireInterval = 0;
	greenStarTimer = 0.f;
	yellowStarTimer = 0.f;
	redStarTimer = 0.f;
	player_speed = 300;
}


SurvivalShooter::~SurvivalShooter()
{
}

void SurvivalShooter::Init()
{
    glm::ivec2 resolution = window->GetResolution();
	auto camera = GetSceneCamera();
	camera->SetPosition(glm::vec3(0, 0, 50));
	camera->SetRotation(glm::vec3(0, 0, 0));
	camera->Update();
	GetCameraInput()->SetActive(false);

	modelMatrix = glm::mat3(1);
    helper = new Helper(glm::vec2(resolution.x / 2, resolution.y / 2));

	logicSpace.x = 0;      
	logicSpace.y = 0;       
	logicSpace.width = (float)resolution.x;   
	logicSpace.height = (float)resolution.y;  

	meshes["orange_circle"] = helper->CreateCircle(ORANGE_COLOR, "circle");
	meshes["brown_circle"] = helper->CreateCircle(BROWN_COLOR, "circle");
	meshes["obstacle"] = helper->CreateRectangle(obstacle_width, obstacle_height, BLUE_COLOR, "obstacle");
	meshes["indicator"] = helper->CreateRectangle(100.f, 20.f, GREEN_COLOR, "indicator");
	meshes["indicator_back"] = helper->CreateRectangle(100.f, 20.f, BLACK, "indicator");
	meshes["projectile"] = helper->CreateRectangle(SQUARE_SIDE, SQUARE_SIDE, glm::vec3(1.000, 0.078, 0.576), "projectile");
	meshes["cube_obstacle"] = helper->CreateRectangle(square_obstacle, square_obstacle, BLUE_COLOR, "cube_obstacle");
	meshes["yellow_square"] = helper->CreateRectangle(SQUARE_SIDE, SQUARE_SIDE, YELLOW_COLOR, "yellow_square");
	meshes["red_square"] = helper->CreateRectangle(SQUARE_SIDE, SQUARE_SIDE, RED_COLOR, "red_square");
	meshes["background"] = helper->CreateRectangle(500.f, 500.f, glm::vec3(0.678, 0.847, 0.902), "background");

	meshes["green_star"] = helper->CreateItem(GREEN_COLOR, "green_star");
	meshes["red_star"] = helper->CreateItem(RED_COLOR, "red_star");
	meshes["yellow_star"] = helper->CreateItem(YELLOW_COLOR, "yellow_star");

	{
		Obstacle aux;
		aux.height = obstacle_height;
		aux.length = obstacle_width;
		aux.mesh = meshes["obstacle"];
		aux.posY = 80;
		aux.posX = 120.f;
		obstacles.push_back(aux);

		aux.posX = 120.f + obstacle_height;
		aux.height = obstacle_height;
		aux.length = obstacle_width;
		obstacles.push_back(aux);

		aux.posX = 120.f;
		aux.posY = resolution.y + 200.F;
		aux.height = obstacle_height;
		aux.length = obstacle_width;
		obstacles.push_back(aux);

		aux.posX = resolution.x - 200.f;
		aux.mesh = meshes["cube_obstacle"];
		aux.posY = resolution.y - 120.F;
		aux.height = square_obstacle;
		aux.length = square_obstacle;
		obstacles.push_back(aux);

		aux.mesh = meshes["obstacle"];
		aux.height = obstacle_height;
		aux.length = obstacle_width;
		aux.posY = 20.f;
		aux.posX = resolution.x + 20.f;
		obstacles.push_back(aux);

		aux.posY = 80.f + 2 * resolution.y;
		aux.posX = resolution.x;
		aux.mesh = meshes["cube_obstacle"];
		aux.height = square_obstacle;
		aux.length = square_obstacle;
		obstacles.push_back(aux);

		aux.posY += 800.f + resolution.y;
		aux.posX -= resolution.x / 4;
		obstacles.push_back(aux);

		aux.mesh = meshes["obstacle"];
		aux.height = obstacle_height;
		aux.length = obstacle_width;
		aux.posY = BACKGROUND_SIZE - 500;
		aux.posX = BACKGROUND_SIZE - 1300;
		obstacles.push_back(aux);

		aux.mesh = meshes["cube_obstacle"];
		aux.height = square_obstacle;
		aux.length = square_obstacle;
		aux.posY = BACKGROUND_SIZE - 40;
		aux.posX = BACKGROUND_SIZE - 7000;
		obstacles.push_back(aux);

		aux.mesh = meshes["obstacle"];
		aux.height = obstacle_height;
		aux.length = obstacle_width;
		aux.posY = BACKGROUND_SIZE/2 + 20;
		aux.posX = BACKGROUND_SIZE/2 - 400.f;
		obstacles.push_back(aux);

		aux.mesh = meshes["cube_obstacle"];
		aux.height = square_obstacle;
		aux.length = square_obstacle;
		aux.posY = BACKGROUND_SIZE - 1200;
		aux.posX = 500;
		obstacles.push_back(aux);
	}
}

void SurvivalShooter::FrameStart()
{
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/* Function from lab */
void SurvivalShooter::SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor, bool clear)
{
	glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

	glEnable(GL_SCISSOR_TEST);
	glScissor(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(colorColor.r, colorColor.g, colorColor.b, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);

	GetSceneCamera()->SetOrthographic((float)viewSpace.x, (float)(viewSpace.x + viewSpace.width), (float)viewSpace.y, (float)(viewSpace.y + viewSpace.height), 0.1f, 400);
	GetSceneCamera()->Update();
}

/* Function from lab */
glm::mat3 SurvivalShooter::VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace)
{
	float sx, sy, tx, ty, smin;
	sx = viewSpace.width / logicSpace.width;
	sy = viewSpace.height / logicSpace.height;
	if (sx < sy)
		smin = sx;
	else
		smin = sy;
	tx = viewSpace.x - smin * logicSpace.x + (viewSpace.width - smin * logicSpace.width) / 2;
	ty = viewSpace.y - smin * logicSpace.y + (viewSpace.height - smin * logicSpace.height) / 2;

	return glm::transpose(glm::mat3(
		smin, 0.0f, tx,
		0.0f, smin, ty,
		0.0f, 0.0f, 1.0f));
}

void SurvivalShooter::Update(float deltaTimeSeconds)
{
	glm::ivec2 resolution = window->GetResolution();

	viewSpace = ViewportSpace(0, 0, resolution.x, resolution.y);
	SetViewportArea(viewSpace, glm::vec3(0, 0, 0), true);
	
	visMatrix = glm::mat3(1);
	visMatrix *= VisualizationTransf2DUnif(logicSpace, viewSpace);

	greenStarTimer += deltaTimeSeconds;
	redStarTimer += deltaTimeSeconds;
	yellowStarTimer += deltaTimeSeconds;

    if (livesNo == 0 || die) {
		for (Projectile* p : projectiles) {
			free(p);
		}
		for (Enemy* e : enemies) {
			free(e);
		}
		for (Item* s : stars) {
			free(s);
		}
		cout << "=======================================================" << endl;
		cout << "---------------GAME OVER :(----------------------------" << endl;
		cout << "=======================================================" << endl;

        exit(0);
    }

	if (player_speed > 300.f) {
		player_speed -= deltaTimeSeconds * 100;
	}

	/* Health bar */
	{
		/* The player has 4 lives */
		if (health <= 0) {
			if (livesNo > 0) {
				health = 3000;
				livesNo--;
			}
		}

		healthScale = health / 3000;

		modelMatrix = visMatrix;
		modelMatrix *= Transform2D::Translate(logicSpace.x + logicSpace.width - 453, logicSpace.y + logicSpace.height - 23);
		modelMatrix *= Transform2D::Scale(healthScale * 4.8, 1.f);
		RenderMesh2D(meshes["indicator"], shaders["VertexColor"], modelMatrix);

		modelMatrix = visMatrix;
		modelMatrix *= Transform2D::Translate(logicSpace.x + logicSpace.width - 460, logicSpace.y + logicSpace.height - 27);
		modelMatrix *= Transform2D::Scale(5.f, 2.f);
		RenderMesh2D(meshes["indicator_back"], shaders["VertexColor"], modelMatrix);
	}

	/* Score bar */
	{
		modelMatrix = visMatrix;
		modelMatrix *= Transform2D::Translate(logicSpace.x + 14, logicSpace.y + logicSpace.height - 23);
		modelMatrix *= Transform2D::Scale(4.8 * score / 3000.f, 1.f);
		RenderMesh2D(meshes["indicator"], shaders["VertexColor"], modelMatrix);

		modelMatrix = visMatrix;
		modelMatrix *= Transform2D::Translate(logicSpace.x + 13, logicSpace.y + logicSpace.height - 27);
		modelMatrix *= Transform2D::Scale(5.f, 2.f);
		RenderMesh2D(meshes["indicator_back"], shaders["VertexColor"], modelMatrix);
	}

	/* Once in every 10 seconds, a green item is rendered and 
		it increases the health of the player*/
	if (greenStarTimer > 10) {
		greenStarTimer = 0.f;
		Item *s = (Item*)calloc(1, sizeof(Item));
		s->x = -280 + rand() % (BACKGROUND_SIZE - 320);
		s->y = -280 + rand() % (BACKGROUND_SIZE - 320);
		s->render = true;
		s->mesh = meshes["green_star"];
		stars.push_back(s);
	}

	/* Once in every 15 seconds, a yellow item is rendered and
	it increases the score of the player*/
	if (yellowStarTimer > 15) {
		yellowStarTimer = 0.f;
		Item* s = (Item*)calloc(1, sizeof(Item));
		s->x = -280 + rand() % (BACKGROUND_SIZE - 320);
		s->y = -280 + rand() % (BACKGROUND_SIZE - 320);
		s->render = true;
		s->mesh = meshes["yellow_star"];
		stars.push_back(s);
	}

	/* Once in every 10 seconds, a red item is rendered and
	it increases the speed of the player*/
	if (redStarTimer > 10) {
		redStarTimer = 0.f;
		Item* s = (Item*)calloc(1, sizeof(Item));
		s->x = -280 + rand() % (BACKGROUND_SIZE - 320);
		s->y = -280 + rand() % (BACKGROUND_SIZE - 320);
		s->render = true;
		s->mesh = meshes["red_star"];
		stars.push_back(s);
	}

	RenderPlayer();

	/* Add a new projectile */
	fireInterval += deltaTimeSeconds;

	if (shoot && (fireInterval >= 0.5)) {
		fireInterval = 0;
		Projectile* p = (Projectile*)calloc(1, sizeof(Projectile));
		Projectile p2{};
		p->collision = false;
		p->x = helper->centre.x;
		p->y = helper->centre.y;
		p->degrees = degrees - (float)M_PI + (float)M_PI / 5;
		p->initialX = p->x;
		p->initialY = p->y; 
		projectiles.push_back(p);
		shoot = false;
	}

	/* Update the existing ones */
	Projectile* aux;
	for(Projectile *p : projectiles) {
		if (!p->collision) {

			/* Check the distance travelled by the projectile using the Euclidian distance */
			if (sqrt((p->initialX - p->x) * (p->initialX - p->x) +
				(p->initialY - p->y) * (p->initialY - p->y)) > PROJECTILE_DISTANCE) {
				p->collision = true;
				continue;
			}

			p->x += deltaTimeSeconds * cos(p->degrees) * PROJECTILE_SPPED;
			p->y += deltaTimeSeconds * sin(p->degrees) * PROJECTILE_SPPED;

			/* Check for collision between enemy and projectile AABB vs. AABB*/
			for (Enemy* en : enemies) {
				if (!en->collision) {

					if (en->posX < p->x + 10 * SQUARE_SIDE &&
						en->posX + 28 * SQUARE_SIDE > p->x &&
						en->posY < p->y + 10 * SQUARE_SIDE &&
						en->posY + 28 * SQUARE_SIDE > p->y) {

						fireInterval = 0.5f;
						p->collision = true;
						en->collision = true;
						
						score  = (score + 100 < 3000) ? score + 100 : 3000;

						cout << "=======================================================" << endl;
						cout << "---------------SCORE: " << score << "----------------------------" << endl;
						cout << "=======================================================" << endl;

						break;
					}
				}
			}

			/* Collision between projectile and obstacle AABB vs. AABB */
			for (Obstacle obstacle : obstacles) {
				if (p->x <= obstacle.posX + obstacle.length && 
					p->x + SQUARE_SIDE * 10 >= obstacle.posX &&
					p->y <= obstacle.posY + obstacle.height && 
					p->y + SQUARE_SIDE * 10 >= obstacle.posY) {

					p->collision = true;
					break;
				}
			}
			/* Collision with wall */
			if (p->x > BACKGROUND_SIZE - 300 ||
				p->x < -300 ||
				p->y < -300 ||
				p->y > BACKGROUND_SIZE - 300) {

				p->collision = true;
			}
		}
		if (!p->collision) {
			modelMatrix = visMatrix;
			modelMatrix *= Transform2D::Translate(p->x, p->y);
			modelMatrix *= Transform2D::Rotate(p->degrees);
			modelMatrix *= Transform2D::Scale(10, 10);
			RenderMesh2D(meshes["projectile"], shaders["VertexColor"], modelMatrix);
		}
	}


	/* Render obstacles */
	for (Obstacle obs : obstacles) {
		modelMatrix = visMatrix;
		modelMatrix *= Transform2D::Translate(obs.posX, obs.posY);
		RenderMesh2D(obs.mesh, shaders["VertexColor"], modelMatrix);
	}

	{
		/* Add a new enemy every 3 seconds*/
		time += deltaTimeSeconds;
		if (time >= 3) {
			Enemy* aux = (Enemy*)calloc(1, sizeof(Enemy));
			/* Random position */
			aux->posX = -300 + SQUARE_SIDE * 28.f + rand() % (BACKGROUND_SIZE - 300 - SQUARE_SIDE * 28);
			aux->posY = -300 + SQUARE_SIDE * 28.f + rand() % (BACKGROUND_SIZE - 300 - SQUARE_SIDE * 28);

			/* Random speed */
			aux->speed = rand() % 20;

			int y = -logicSpace.y - logicSpace.height + aux->posY + helper->centre.y;
			int x = -logicSpace.x - aux->posX + helper->centre.x;

			aux->degrees = -2 * atan(y / (x + sqrt(x * x + y * y))) - M_PI / 4;
			enemies.push_back(aux);
			time = 0;
		}
	}

	/* Bonus items */
	{
		for (Item* s : stars) {
			if (s->render) {
				/* Collision between player and an item: Sphere vs Sphere*/
				GLfloat distance = sqrt((helper->centre.x - s->x) * (helper->centre.x - s->x) +
					(helper->centre.y - s->y) * (helper->centre.y - s->y));

				if (distance < (PLAYED_RADIUS + 20.f)) {
					s->render = false;
					if (s->mesh == meshes["green_star"]) {
						health = (health + 50 > 3000) ? 3000 : (health + 50);
						cout << "Health increased" << endl;
					}
					else if (s->mesh == meshes["red_star"]) {
						player_speed *= 2;
						cout << "Speed increased" << endl;
					}
					else {
						score = (score + 50 > 3000) ? 3000 : (score + 50);
						cout << "Score increased" << endl;
					}
				}

				if (s->render) {
					modelMatrix = visMatrix;
					modelMatrix *= Transform2D::Translate(s->x, s->y);
					modelMatrix *= Transform2D::Scale(40.f, 40.f);
					RenderMesh2D(s->mesh, shaders["VertexColor"], modelMatrix);
				}
			}
		}
	}

	{
		/* Update enemies */
		for (Enemy* en : enemies) {
			if (!en->collision) {
				int y = en->posY - helper->centre.y;
				int x = -en->posX + helper->centre.x;
				en->degrees = -2 * atan(y / (x + sqrt(x * x + y * y))) - M_PI / 4;

				/* Used for collision with player: Sphere vs. AABB*/
				GLfloat cx = max(en->posX + cos(en->degrees) * en->speed / 2, 
					min(helper->centre.x, en->posX + cos(en->degrees) * en->speed / 2 + SQUARE_SIDE * 28));
				GLfloat cy = max(en->posY + sin(en->degrees) * en->speed / 2, 
					min(helper->centre.y, en->posY + sin(en->degrees) * en->speed / 2 + SQUARE_SIDE * 28));
				GLfloat distance = sqrt((cx - helper->centre.x) * (cx - helper->centre.x) + 
					(cy - helper->centre.y) * (cy - helper->centre.y));

				if (en->posX + cos(en->degrees) * en->speed / 2 + SQUARE_SIDE * 28 > BACKGROUND_SIZE - 300 ||
					en->posX + cos(en->degrees) * en->speed / 2 < -300 ||
					en->posY + sin(en->degrees) * en->speed / 2 + SQUARE_SIDE * 28 > BACKGROUND_SIZE - 300 ||
					en->posY + sin(en->degrees) * en->speed / 2 < -300) {

					// do nothing. Enemy would hit the wall
				}
				else if (distance < helper->headOffsetX + PLAYED_RADIUS) {
					/* collision with the player */
					health -= 200.f;
					en->collision = true;
				} else {
					/* go farther */
					en->posX += cos(en->degrees) * en->speed / 2;
					en->posY += sin(en->degrees) * en->speed / 2;
				}
				if(!en->collision)
					RenderEnemy(*en, deltaTimeSeconds);
			}
		}
	}

	{
		/*Render background*/
		modelMatrix = visMatrix;
		modelMatrix *= Transform2D::Translate(-300, -300);
		modelMatrix *= Transform2D::Scale(6, 6);
		RenderMesh2D(meshes["background"], shaders["VertexColor"], modelMatrix);
	}
}

void SurvivalShooter::RenderPlayer() {
	modelMatrix = visMatrix;
	modelMatrix *= Transform2D::Translate(helper->centre.x, helper->centre.y);
	modelMatrix *= Transform2D::Rotate(degrees - RADIANS(15));
	modelMatrix *= Transform2D::Translate(-helper->eyeOffsetX, -helper->eyeOffsetY);
	modelMatrix *= Transform2D::Scale(10.f, 10.f);
	RenderMesh2D(meshes["orange_circle"], shaders["VertexColor"], modelMatrix);

	modelMatrix =visMatrix;
	modelMatrix *= Transform2D::Translate(helper->centre.x, helper->centre.y);
	modelMatrix *= Transform2D::Rotate(degrees + RADIANS(15));
	modelMatrix *= Transform2D::Translate(-helper->eyeOffsetX, -helper->eyeOffsetY);
	modelMatrix *= Transform2D::Scale(10.f, 10.f);
	RenderMesh2D(meshes["orange_circle"], shaders["VertexColor"], modelMatrix);

	modelMatrix = visMatrix;
	modelMatrix *= Transform2D::Translate(helper->centre.x, helper->centre.y);
	modelMatrix *= Transform2D::Rotate(degrees - RADIANS(15));
	modelMatrix *= Transform2D::Translate(-helper->eyeOffsetX, -helper->eyeOffsetY);
	modelMatrix *= Transform2D::Scale(15.f, 15.f);
	RenderMesh2D(meshes["brown_circle"], shaders["VertexColor"], modelMatrix);

	modelMatrix = visMatrix;
	modelMatrix *= Transform2D::Translate(helper->centre.x, helper->centre.y);
	modelMatrix *= Transform2D::Rotate(degrees + RADIANS(15));
	modelMatrix *= Transform2D::Translate(-helper->eyeOffsetX, -helper->eyeOffsetY);
	modelMatrix *= Transform2D::Scale(15.f, 15.f);
	RenderMesh2D(meshes["brown_circle"], shaders["VertexColor"], modelMatrix);

	modelMatrix = visMatrix;
	modelMatrix *= Transform2D::Translate(helper->centre.x, helper->centre.y);
	modelMatrix *= Transform2D::Rotate(degrees - RADIANS(15));
	modelMatrix *= Transform2D::Translate(-helper->eyeOffsetX, -helper->eyeOffsetY);
	modelMatrix *= Transform2D::Scale(17.f, 17.f);
	RenderMesh2D(meshes["brown_circle"], shaders["Color"], modelMatrix);

	modelMatrix = visMatrix;
	modelMatrix *= Transform2D::Translate(helper->centre.x, helper->centre.y);
	modelMatrix *= Transform2D::Rotate(degrees + RADIANS(15));
	modelMatrix *= Transform2D::Translate(-helper->eyeOffsetX, -helper->eyeOffsetY);
	modelMatrix *= Transform2D::Scale(17.f, 17.f);
	RenderMesh2D(meshes["brown_circle"], shaders["Color"], modelMatrix);

	modelMatrix = visMatrix;
	modelMatrix *= Transform2D::Translate(helper->centre.x, helper->centre.y);
	modelMatrix *= Transform2D::Rotate(degrees);
	modelMatrix *= Transform2D::Translate(-helper->headOffsetX, -helper->headOffsetY);
	modelMatrix *= Transform2D::Scale(65.f, 65.f);
	RenderMesh2D(meshes["brown_circle"], shaders["Color"], modelMatrix);

	modelMatrix = visMatrix;
	modelMatrix *= Transform2D::Translate(helper->centre.x, helper->centre.y);
	modelMatrix *= Transform2D::Scale(75.f, 75.f);
	RenderMesh2D(meshes["orange_circle"], shaders["VertexColor"], modelMatrix);

	modelMatrix = visMatrix;
	modelMatrix *= Transform2D::Translate(helper->centre.x, helper->centre.y);
	modelMatrix *= Transform2D::Scale(80.8f, 80.8f);
	RenderMesh2D(meshes["brown_circle"], shaders["VertexColor"], modelMatrix);
}

void SurvivalShooter::RenderEnemy(Enemy enemy, float deltaTimeSeconds) {

	modelMatrix = visMatrix;
	modelMatrix *= Transform2D::Translate(enemy.posX, enemy.posY);
	modelMatrix *= Transform2D::Rotate(degrees + RADIANS(15) - M_PI);
	modelMatrix *= Transform2D::Translate(-helper->eyeOffsetX / 3, -helper->eyeOffsetY / 3);
	modelMatrix *= Transform2D::Scale(10.f, 10.f);
	RenderMesh2D(meshes["yellow_square"], shaders["VertexColor"], modelMatrix);

	modelMatrix = visMatrix;
	modelMatrix *= Transform2D::Translate(enemy.posX + 25 * SQUARE_SIDE, enemy.posY);
	modelMatrix *= Transform2D::Rotate(degrees + RADIANS(15) - M_PI);
	modelMatrix *= Transform2D::Translate(-helper->eyeOffsetX / 3, -helper->eyeOffsetY / 3);
	modelMatrix *= Transform2D::Scale(10.f, 10.f);
	RenderMesh2D(meshes["yellow_square"], shaders["VertexColor"], modelMatrix);

	modelMatrix = visMatrix;
	modelMatrix *= Transform2D::Translate(enemy.posX + 5, enemy.posY + 5);
	modelMatrix *= Transform2D::Scale(25.f, 25.f);
	RenderMesh2D(meshes["red_square"], shaders["VertexColor"], modelMatrix);

	modelMatrix = visMatrix;
	modelMatrix *= Transform2D::Translate(enemy.posX, enemy.posY);
	modelMatrix *= Transform2D::Scale(28.f, 28.f);
	RenderMesh2D(meshes["red_square"], shaders["Color"], modelMatrix);
}

void SurvivalShooter::OnInputUpdate(float deltaTime, int mods)
{
	int ok = 0, x, y, distance;
	if (window->KeyHold(GLFW_KEY_W) && helper->centre.y + deltaTime * player_speed + PLAYED_RADIUS < BACKGROUND_SIZE - 300) {

		for(Obstacle obstacle : obstacles) {
			x = max(obstacle.posX, min(helper->centre.x, obstacle.posX + obstacle.length));
			y = max(obstacle.posY, min(helper->centre.y + deltaTime * player_speed, obstacle.posY + obstacle.height));
			distance = sqrt((x - helper->centre.x) * (x - helper->centre.x) + 
				(y - (helper->centre.y + deltaTime * player_speed + 2.f)) * (y - (helper->centre.y + deltaTime * player_speed + 2.f)));

			if (distance >= PLAYED_RADIUS + 2.f){

			} else {
				ok = 1;
				break;
			}
		}
		if (ok == 0) {
			logicSpace.y += deltaTime * player_speed;
			helper->centre.y += deltaTime * player_speed;
		}
	}

	if (window->KeyHold(GLFW_KEY_S) && helper->centre.y - deltaTime * player_speed - PLAYED_RADIUS >  - 300) {
		int ok = 0;
		for (Obstacle obstacle : obstacles) {
			x = max(obstacle.posX, min(helper->centre.x, obstacle.posX + obstacle.length));
			y = max(obstacle.posY, min(helper->centre.y - deltaTime * player_speed, obstacle.posY + obstacle.height));
			distance = sqrt((x - helper->centre.x) * (x - helper->centre.x) + 
				(y - (helper->centre.y - deltaTime * player_speed - 2.f)) * (y - (helper->centre.y - deltaTime * player_speed - 2.f)));

			if(distance >= PLAYED_RADIUS + 2.f){
			}
			else {
				ok = 1;
				break;
			}
		}
		if (ok == 0) {
			logicSpace.y -= deltaTime * player_speed;
			helper->centre.y -= deltaTime * player_speed;
		}

	}
	if (window->KeyHold(GLFW_KEY_A) && helper->centre.x - deltaTime * player_speed - PLAYED_RADIUS > - 300) {
		int ok = 0;
		for (Obstacle obstacle : obstacles) {
			x = max(obstacle.posX, min(helper->centre.x - deltaTime * player_speed, obstacle.posX + obstacle.length));
			y = max(obstacle.posY, min(helper->centre.y, obstacle.posY + obstacle.height));
			distance = sqrt((x - (helper->centre.x - deltaTime * player_speed - 2.f)) * (x - (helper->centre.x - deltaTime * player_speed - 2.f)) + 
				(y - helper->centre.y) * (y - helper->centre.y));

			if (distance >= PLAYED_RADIUS + 2.f) {
			}
			else {
				ok = 1;
				break;
			}
		}
		if (ok == 0) {
			logicSpace.x -= deltaTime * player_speed;
			helper->centre.x -= deltaTime * player_speed;
		}
	}
	if (window->KeyHold(GLFW_KEY_D) && helper->centre.x + deltaTime * player_speed + PLAYED_RADIUS < BACKGROUND_SIZE - 300) {
		int ok = 0;
		for (Obstacle obstacle : obstacles) {
			x = max(obstacle.posX, min(helper->centre.x + deltaTime * player_speed, obstacle.posX + obstacle.length));
			y = max(obstacle.posY, min(helper->centre.y, obstacle.posY + obstacle.height));
			distance = sqrt((x - (helper->centre.x + deltaTime * player_speed + 2.f)) * (x - (helper->centre.x + deltaTime * player_speed + 2.f)) + 
				(y - helper->centre.y) * (y - helper->centre.y));

			if (distance >= PLAYED_RADIUS + 2.f) {
			}
			else {
				ok = 1;
				break;
			}
		}
		if (ok == 0) {
			logicSpace.x += deltaTime * player_speed;
			helper->centre.x += deltaTime * player_speed;
		}

	}

	if (window->KeyHold(GLFW_KEY_L)) {
		die = true;
	}
}

void SurvivalShooter::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	float x = -logicSpace.x - mouseX + helper->centre.x;
	float y = logicSpace.y + logicSpace.height - mouseY - helper->centre.y;
	degrees = -2 * (float)atan(y / (x + sqrt(x * x + y * y))) - (float)M_PI / 4;
}

void SurvivalShooter::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	shoot = true;
}
