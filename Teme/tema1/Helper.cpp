#include "lab_m1/tema1/Helper.h"

using namespace std;

Helper::Helper(glm::vec2 centre1){
	centre.x = centre1.x;
	centre.y = centre1.y;
	eyeOffsetX = 90;
	eyeOffsetY = 90;
	headOffsetX = 40;
	headOffsetY = 40;
}

Helper::~Helper(){}

Mesh* Helper::CreateCircle(glm::vec3 color, string name) {
	
	vector<VertexFormat> vertices;
	vector<unsigned int> indices;
	GLfloat arg;
	vertices.emplace_back(glm::vec3(0, 0, 0), color);
	for (GLushort i = 0; i < NUM_TRIANGLES; i++)
	{
		arg = TWO_PI * i / NUM_TRIANGLES;
		vertices.emplace_back(glm::vec3(cos(arg), sin(arg), 0), color);
		indices.push_back(i);
	}
	indices.push_back(NUM_TRIANGLES);
	indices.push_back(1);

	Mesh* circle = new Mesh(name);
	circle->InitFromData(vertices, indices);
	circle->SetDrawMode(GL_TRIANGLE_FAN);

	return circle;
}

Mesh* Helper::CreateRectangle(float width, float height, glm::vec3 color, string name) {
	vector<VertexFormat> vertices
	{
		VertexFormat(glm::vec3(0.f,		0.f,	0.f), color),
		VertexFormat(glm::vec3(width,	0.f,	0.f), color),
		VertexFormat(glm::vec3(width,	height, 0.f), color),
		VertexFormat(glm::vec3(0.f,		height, 0.f), color)
	};

	vector<unsigned int> indices =
	{
		0, 1, 2,
		0, 2, 3
	};

	Mesh* rectangle = new Mesh(name);
	rectangle->InitFromData(vertices, indices);
	return rectangle;
}

Mesh* Helper::CreateItem(glm::vec3 color, string name) {
	vector<VertexFormat> vertices;
	vector<unsigned int> indices;
	for (unsigned i = 0; i < 2 * 5; ++i) {

		float const angle(TWO_PI * ((float)i / (float)5));

		if (!(i & 1)) { 
			vertices.emplace_back(glm::vec3(cosf(angle), sinf(angle), 0), color);
			indices.push_back(i);
		}

		else { 
			vertices.emplace_back(glm::vec3(RADIANS(30) * cosf(angle), RADIANS(30) * sinf(angle), 0), color);
			indices.push_back(i);
		}
	}
	Mesh* star = new Mesh(name);
	star->InitFromData(vertices, indices);
	return star;
}