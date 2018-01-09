#define GLEW_STATIC
#include <GL/glew.h>
#include "shprogram.h"
#include <GLFW/glfw3.h>
#include <SOIL.h>
#include <iostream>
using namespace std;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>

#define GREY1 0.3f
#define GREY2 0.6f
#define BOX 50.0f
#define PULSE 1.0f/16.0f
#define ANGLE_NUMBER 16
#define LIGHT_DOWN_LIMIT 0.0f
#define LIGHT_UP_LIMIT 2.0f

const GLuint WIDTH = 1200, HEIGHT = 700;
static GLfloat camera_angle_Horizontal = 0.0f;
static GLfloat camera_angle_Vertical = 0.0f;
static GLfloat startAngle = 360.0f / ANGLE_NUMBER;
static GLfloat speed = 2.0f;
static GLfloat red = 1.0f;
static GLfloat green = 1.0f;
static GLfloat blue = 1.0f;
static GLfloat zoom = 20.0f;
static bool directionDown = true;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_RIGHT)
		camera_angle_Horizontal += 2.5f;
	if (key == GLFW_KEY_LEFT)
		camera_angle_Horizontal -= 2.5f;
	if (key == GLFW_KEY_UP && camera_angle_Vertical < 85.0f)
		camera_angle_Vertical += 2.5f;
	if (key == GLFW_KEY_DOWN && camera_angle_Vertical >-5.0f)
		camera_angle_Vertical -= 2.5f;
	if (key == GLFW_KEY_MINUS && speed > 0.0f)
		speed -= 0.5f;
	if (key == GLFW_KEY_EQUAL)
		speed += 0.5f;
	if (key == GLFW_KEY_R && red > LIGHT_DOWN_LIMIT)
		red -= PULSE;
	if (key == GLFW_KEY_G && green > LIGHT_DOWN_LIMIT)
		green -= PULSE;
	if (key == GLFW_KEY_B && blue > LIGHT_DOWN_LIMIT)
		blue -= PULSE;
	if (key == GLFW_KEY_T && red < LIGHT_UP_LIMIT)
		red += PULSE;
	if (key == GLFW_KEY_H && green < LIGHT_UP_LIMIT)
		green += PULSE;
	if (key == GLFW_KEY_N && blue < LIGHT_UP_LIMIT)
		blue += PULSE;
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		if (red == LIGHT_UP_LIMIT && green == LIGHT_UP_LIMIT && blue == LIGHT_UP_LIMIT)
		{
			red = LIGHT_DOWN_LIMIT;
			green = LIGHT_DOWN_LIMIT;
			blue = LIGHT_DOWN_LIMIT;
		}
		else
		{
			red = LIGHT_UP_LIMIT;
			green = LIGHT_UP_LIMIT;
			blue = LIGHT_UP_LIMIT;
		}
	if (key == GLFW_KEY_Z)
	{
		if (red > LIGHT_DOWN_LIMIT)
			red -= PULSE;
		if (green > LIGHT_DOWN_LIMIT)
			green -= PULSE;
		if (blue > LIGHT_DOWN_LIMIT)
			blue -= PULSE;
	}
	if (key == GLFW_KEY_X)
	{
		if (red < LIGHT_UP_LIMIT)
			red += PULSE;
		if (green < LIGHT_UP_LIMIT)
			green += PULSE;
		if (blue < LIGHT_UP_LIMIT)
			blue += PULSE;
	}
	if (key == GLFW_KEY_S && zoom < 21)
		zoom += 1;
	if (key == GLFW_KEY_W && zoom > 7)
		zoom -= 1;

}

GLuint LoadMipmapTexture(GLuint texId, const char* fname)
{
	int width, height;
	unsigned char* image = SOIL_load_image(fname, &width, &height, 0, SOIL_LOAD_RGB);
	if (image == nullptr)
		throw exception("Failed to load texture file");

	GLuint texture;
	glGenTextures(1, &texture);

	glActiveTexture(texId);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

ostream& operator<<(ostream& os, const glm::mat4& mx)
{
	for (int row = 0; row < 4; ++row)
	{
		for (int col = 0; col < 4; ++col)
			cout << mx[row][col] << ' ';
		cout << endl;
	}
	return os;
}

int main()
{
	{
		glm::mat4 trans;
		cout << trans << endl;
		trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
		cout << trans << endl;
	}
	if (glfwInit() != GL_TRUE)
	{
		cout << "GLFW initialization failed" << endl;
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	try
	{
		GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "GKOM - gear wheel", nullptr, nullptr);
		if (window == nullptr)
			throw exception("GLFW window not created");
		glfwMakeContextCurrent(window);
		glfwSetKeyCallback(window, key_callback);

		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
			throw exception("GLEW Initialization failed");

		glViewport(0, 0, WIDTH, HEIGHT);

		glEnable(GL_DEPTH_TEST);

		GLint nrAttributes;
		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
		cout << "Max vertex attributes allowed: " << nrAttributes << std::endl;
		glGetIntegerv(GL_MAX_TEXTURE_COORDS, &nrAttributes);
		cout << "Max texture coords allowed: " << nrAttributes << std::endl;

		ShaderProgram theProgram("gearWheels.vert", "gearWheels.frag");

		GLfloat centerX = -2.7f;
		GLfloat centerY = 0.0f;
		GLfloat centerZ = 0.0f;
		GLfloat sizeGear = 1.5f;
		GLfloat radius = 1.0f;
		GLfloat teethSize = 0.2f;
		GLfloat vertices[12 * ( (ANGLE_NUMBER * 2 + 1) * 2 * 2 +	 34  +		20 +	 4 +	 20)];
		//dwa kola
		//walec
		//podstawa
		//podloga
		//skybox

		for (int i = 0; i < 12 * ((ANGLE_NUMBER * 2 + 1) * 2 * 2 + 34 + 20 + 4 + 20); ++i)
			vertices[i] = 0;
		//srodki kola zebatego
		vertices[ANGLE_NUMBER * 2 * 12] = centerX;
		vertices[ANGLE_NUMBER * 2 * 12 + 1] = centerY;
		vertices[ANGLE_NUMBER * 2 * 12 + 2] = centerZ;
		vertices[ANGLE_NUMBER * 2 * 12 + 3] = GREY1;
		vertices[ANGLE_NUMBER * 2 * 12 + 4] = GREY1;
		vertices[ANGLE_NUMBER * 2 * 12 + 5] = GREY1;

		vertices[ANGLE_NUMBER * 2 * 12 + (ANGLE_NUMBER * 2 + 1) * 12] = centerX;
		vertices[ANGLE_NUMBER * 2 * 12 + 1 + (ANGLE_NUMBER * 2 + 1) * 12] = centerY;
		vertices[ANGLE_NUMBER * 2 * 12 + 2 + (ANGLE_NUMBER * 2 + 1) * 12] = centerZ + sizeGear;
		vertices[ANGLE_NUMBER * 2 * 12 + 3 + (ANGLE_NUMBER * 2 + 1) * 12] = GREY2;
		vertices[ANGLE_NUMBER * 2 * 12 + 4 + (ANGLE_NUMBER * 2 + 1) * 12] = GREY2;
		vertices[ANGLE_NUMBER * 2 * 12 + 5 + (ANGLE_NUMBER * 2 + 1) * 12] = GREY2;

		static GLfloat angle = startAngle;
		//punkty brzegowe pierwszego kola zebatego
		for (int i = 0; i < ANGLE_NUMBER; ++i)
		{
			vertices[24 * i] = vertices[(ANGLE_NUMBER * 2 + 1) * 12 + 24 * i] = cos(glm::radians(angle))*radius + centerX;
			vertices[24 * i + 1] = vertices[(ANGLE_NUMBER * 2 + 1) * 12 + 24 * i + 1] = sin(glm::radians(angle))*radius + centerY;
			vertices[24 * i + 2] = 0 + centerZ;
			vertices[24 * i + 3] = GREY1;		//R
			vertices[24 * i + 4] = GREY1;		//G
			vertices[24 * i + 5] = GREY1;		//B

			vertices[24 * i + 12] = vertices[(ANGLE_NUMBER * 2 + 1) * 12 + 24 * i + 12] = cos(glm::radians(angle))*(radius + teethSize) + centerX;
			vertices[24 * i + 13] = vertices[(ANGLE_NUMBER * 2 + 1) * 12 + 24 * i + 13] = sin(glm::radians(angle))*(radius + teethSize) + centerY;
			vertices[24 * i + 14] = 0 + centerZ;
			vertices[24 * i + 15] = GREY1;
			vertices[24 * i + 16] = GREY1;
			vertices[24 * i + 17] = GREY1;

			vertices[(ANGLE_NUMBER * 2 + 1) * 12 + 24 * i + 2] = sizeGear + centerZ;
			vertices[(ANGLE_NUMBER * 2 + 1) * 12 + 24 * i + 14] = sizeGear + centerZ;

			vertices[(ANGLE_NUMBER * 2 + 1) * 12 + 24 * i + 3] = GREY2;
			vertices[(ANGLE_NUMBER * 2 + 1) * 12 + 24 * i + 4] = GREY2;
			vertices[(ANGLE_NUMBER * 2 + 1) * 12 + 24 * i + 5] = GREY2;

			vertices[(ANGLE_NUMBER * 2 + 1) * 12 + 24 * i + 15] = GREY2;
			vertices[(ANGLE_NUMBER * 2 + 1) * 12 + 24 * i + 16] = GREY2;
			vertices[(ANGLE_NUMBER * 2 + 1) * 12 + 24 * i + 17] = GREY2;

			angle += 360.0 / ANGLE_NUMBER;
		}

		GLuint indices[3 * ((4 * ANGLE_NUMBER + 4 * ANGLE_NUMBER) * 2 +	 16 +	16 +	16 * 2 +	10 +	2 +		10)];
		int number = -1;

		for (int i = 0; i < 3 * ((4 * ANGLE_NUMBER + 4 * ANGLE_NUMBER) * 2 + 16 + 16 + 16 * 2 + 10 + 2 + 10); ++i)
			indices[i] = 0;

		//oba kola zebate
		//16 - podstawa walca
		//16 - podstawa walca
		//32 - boki walca
		//3 - podstawa
		//2 - podloga
		//10 - skybox

		//krawedzie kola zebatego
		for (int i = 0; i < ANGLE_NUMBER; ++i)
		{
			indices[++number] = 2 * i;
			indices[++number] = (2 * (i + 1)) % (2 * ANGLE_NUMBER);
			indices[++number] = 2 * ANGLE_NUMBER;
		}
		for (int i = 0; i < ANGLE_NUMBER / 2; ++i)
		{
			indices[++number] = 4 * i;
			indices[++number] = 4 * i + 1;
			indices[++number] = 4 * i + 2;
		}
		for (int i = 0; i < ANGLE_NUMBER / 2; ++i)
		{
			indices[++number] = 4 * i + 1;
			indices[++number] = 4 * i + 2;
			indices[++number] = 4 * i + 3;
		}
		for (int i = 0; i < ANGLE_NUMBER; ++i)
		{
			indices[++number] = (2 * ANGLE_NUMBER + 1) + 2 * i;
			indices[++number] = (2 * ANGLE_NUMBER + 1) + (2 * (i + 1)) % (2 * ANGLE_NUMBER);
			indices[++number] = (2 * ANGLE_NUMBER + 1) + 2 * ANGLE_NUMBER;
		}
		for (int i = 0; i < ANGLE_NUMBER / 2; ++i)
		{
			indices[++number] = (2 * ANGLE_NUMBER + 1) + 4 * i;
			indices[++number] = (2 * ANGLE_NUMBER + 1) + 4 * i + 1;
			indices[++number] = (2 * ANGLE_NUMBER + 1) + 4 * i + 2;
		}
		for (int i = 0; i < ANGLE_NUMBER / 2; ++i)
		{
			indices[++number] = (2 * ANGLE_NUMBER + 1) + 4 * i + 1;
			indices[++number] = (2 * ANGLE_NUMBER + 1) + 4 * i + 2;
			indices[++number] = (2 * ANGLE_NUMBER + 1) + 4 * i + 3;
		}

		for (int i = 0; i < ANGLE_NUMBER / 2; ++i)
		{
			indices[++number] = 4 * i;
			indices[++number] = 4 * i + 1;
			indices[++number] = 4 * i + (2 * ANGLE_NUMBER + 1);

			indices[++number] = 4 * i + 1;
			indices[++number] = 4 * i + (2 * ANGLE_NUMBER + 1);
			indices[++number] = 4 * i + (2 * ANGLE_NUMBER + 2);

			indices[++number] = 4 * i + 1;
			indices[++number] = 4 * i + 3;
			indices[++number] = 4 * i + (2 * ANGLE_NUMBER + 2);

			indices[++number] = 4 * i + 3;
			indices[++number] = 4 * i + (2 * ANGLE_NUMBER + 2);
			indices[++number] = 4 * i + (2 * ANGLE_NUMBER + 4);

			indices[++number] = 4 * i + 2;
			indices[++number] = 4 * i + 3;
			indices[++number] = 4 * i + (2 * ANGLE_NUMBER + 3);

			indices[++number] = 4 * i + 3;
			indices[++number] = 4 * i + (2 * ANGLE_NUMBER + 3);
			indices[++number] = 4 * i + (2 * ANGLE_NUMBER + 4);

			indices[++number] = 4 * i + 2;
			indices[++number] = (4 * i + 4) % (2 * ANGLE_NUMBER);
			indices[++number] = 4 * i + (2 * ANGLE_NUMBER + 3);

			indices[++number] = (4 * i + 4) % (2 * ANGLE_NUMBER);
			indices[++number] = 4 * i + (2 * ANGLE_NUMBER + 3);
			indices[++number] = (2 * ANGLE_NUMBER + 1) + (4 * i + 4) % (2 * ANGLE_NUMBER);
		}

		//teraz drugie kolo zebate

		GLint offset = 12 * (2 * ANGLE_NUMBER + 1) * 2;

		centerX = 2.7f;

		//wierzcholki w srodku kola z obu stron
		vertices[offset + (2 * ANGLE_NUMBER) * 12] = centerX;
		vertices[offset + (2 * ANGLE_NUMBER) * 12 + 1] = centerY;
		vertices[offset + (2 * ANGLE_NUMBER) * 12 + 2] = centerZ;
		vertices[offset + (2 * ANGLE_NUMBER) * 12 + 3] = GREY1;
		vertices[offset + (2 * ANGLE_NUMBER) * 12 + 4] = GREY1;
		vertices[offset + (2 * ANGLE_NUMBER) * 12 + 5] = GREY1;

		vertices[offset + (2 * ANGLE_NUMBER) * 12 + (2 * ANGLE_NUMBER + 1) * 12] = centerX;
		vertices[offset + (2 * ANGLE_NUMBER) * 12 + (2 * ANGLE_NUMBER + 1) * 12 + 1] = centerY;
		vertices[offset + (2 * ANGLE_NUMBER) * 12 + (2 * ANGLE_NUMBER + 1) * 12 + 2] = centerZ + sizeGear;
		vertices[offset + (2 * ANGLE_NUMBER) * 12 + (2 * ANGLE_NUMBER + 1) * 12 + 3] = GREY2;
		vertices[offset + (2 * ANGLE_NUMBER) * 12 + (2 * ANGLE_NUMBER + 1) * 12 + 4] = GREY2;
		vertices[offset + (2 * ANGLE_NUMBER) * 12 + (2 * ANGLE_NUMBER + 1) * 12 + 5] = GREY2;

		angle = startAngle;

		//wierzcholki na brzegu kola z obu stron
		for (int i = 0; i < ANGLE_NUMBER; ++i)
		{
			vertices[offset + 24 * i] = vertices[offset + (2 * ANGLE_NUMBER + 1) * 12 + 24 * i] = cos(glm::radians(angle))*radius + centerX;
			vertices[offset + 24 * i + 1] = vertices[offset + (2 * ANGLE_NUMBER + 1) * 12 + 24 * i + 1] = sin(glm::radians(angle))*radius + centerY;
			vertices[offset + 24 * i + 2] = 0 + centerZ;
			vertices[offset + 24 * i + 3] = GREY1;
			vertices[offset + 24 * i + 4] = GREY1;
			vertices[offset + 24 * i + 5] = GREY1;

			vertices[offset + 24 * i + 12] = vertices[offset + (2 * ANGLE_NUMBER + 1) * 12 + 24 * i + 12] = cos(glm::radians(angle))*(radius + teethSize) + centerX;
			vertices[offset + 24 * i + 13] = vertices[offset + (2 * ANGLE_NUMBER + 1) * 12 + 24 * i + 13] = sin(glm::radians(angle))*(radius + teethSize) + centerY;
			vertices[offset + 24 * i + 14] = 0 + centerZ;
			vertices[offset + 24 * i + 15] = GREY1;
			vertices[offset + 24 * i + 16] = GREY1;
			vertices[offset + 24 * i + 17] = GREY1;

			vertices[offset + (2 * ANGLE_NUMBER + 1) * 12 + 24 * i + 2] = sizeGear + centerZ;
			vertices[offset + (2 * ANGLE_NUMBER + 1) * 12 + 24 * i + 14] = sizeGear + centerZ;

			vertices[offset + (2 * ANGLE_NUMBER + 1) * 12 + 24 * i + 3] = GREY2;
			vertices[offset + (2 * ANGLE_NUMBER + 1) * 12 + 24 * i + 4] = GREY2;
			vertices[offset + (2 * ANGLE_NUMBER + 1) * 12 + 24 * i + 5] = GREY2;
			vertices[offset + (2 * ANGLE_NUMBER + 1) * 12 + 24 * i + 15] = GREY2;
			vertices[offset + (2 * ANGLE_NUMBER + 1) * 12 + 24 * i + 16] = GREY2;
			vertices[offset + (2 * ANGLE_NUMBER + 1) * 12 + 24 * i + 17] = GREY2;

			angle += 360.0 / ANGLE_NUMBER;
		}

		int offset2 = (ANGLE_NUMBER * 2 + 1) * 2;

		for (int i = 0; i < ANGLE_NUMBER; ++i)
		{
			indices[++number] = offset2 + 2 * i;
			indices[++number] = offset2 + (2 * (i + 1)) % (2 * ANGLE_NUMBER);
			indices[++number] = offset2 + (2 * ANGLE_NUMBER);
		}
		for (int i = 0; i < ANGLE_NUMBER / 2; ++i)
		{
			indices[++number] = offset2 + 4 * i;
			indices[++number] = offset2 + 4 * i + 1;
			indices[++number] = offset2 + 4 * i + 2;
		}
		for (int i = 0; i < ANGLE_NUMBER / 2; ++i)
		{
			indices[++number] = offset2 + 4 * i + 1;
			indices[++number] = offset2 + 4 * i + 2;
			indices[++number] = offset2 + 4 * i + 3;
		}
		for (int i = 0; i < ANGLE_NUMBER; ++i)
		{
			indices[++number] = offset2 + (2 * ANGLE_NUMBER + 1) + 2 * i;
			indices[++number] = offset2 + (2 * ANGLE_NUMBER + 1) + (2 * (i + 1)) % (2 * ANGLE_NUMBER);
			indices[++number] = offset2 + (2 * ANGLE_NUMBER + 1) + (2 * ANGLE_NUMBER);
		}
		for (int i = 0; i < ANGLE_NUMBER / 2; ++i)
		{
			indices[++number] = offset2 + (2 * ANGLE_NUMBER + 1) + 4 * i;
			indices[++number] = offset2 + (2 * ANGLE_NUMBER + 1) + 4 * i + 1;
			indices[++number] = offset2 + (2 * ANGLE_NUMBER + 1) + 4 * i + 2;
		}
		for (int i = 0; i < ANGLE_NUMBER / 2; ++i)
		{
			indices[++number] = offset2 + (2 * ANGLE_NUMBER + 1) + 4 * i + 1;
			indices[++number] = offset2 + (2 * ANGLE_NUMBER + 1) + 4 * i + 2;
			indices[++number] = offset2 + (2 * ANGLE_NUMBER + 1) + 4 * i + 3;
		}
		for (int i = 0; i < ANGLE_NUMBER / 2; ++i)
		{
			indices[++number] = offset2 + 4 * i;
			indices[++number] = offset2 + 4 * i + 1;
			indices[++number] = offset2 + 4 * i + (2 * ANGLE_NUMBER + 1);

			indices[++number] = offset2 + 4 * i + 1;
			indices[++number] = offset2 + 4 * i + (2 * ANGLE_NUMBER + 1);
			indices[++number] = offset2 + 4 * i + (2 * ANGLE_NUMBER + 2);

			indices[++number] = offset2 + 4 * i + 1;
			indices[++number] = offset2 + 4 * i + 3;
			indices[++number] = offset2 + 4 * i + (2 * ANGLE_NUMBER + 2);

			indices[++number] = offset2 + 4 * i + 3;
			indices[++number] = offset2 + 4 * i + (2 * ANGLE_NUMBER + 2);
			indices[++number] = offset2 + 4 * i + (2 * ANGLE_NUMBER + 4);

			indices[++number] = offset2 + 4 * i + 2;
			indices[++number] = offset2 + 4 * i + 3;
			indices[++number] = offset2 + 4 * i + (2 * ANGLE_NUMBER + 3);

			indices[++number] = offset2 + 4 * i + 3;
			indices[++number] = offset2 + 4 * i + (2 * ANGLE_NUMBER + 3);
			indices[++number] = offset2 + 4 * i + (2 * ANGLE_NUMBER + 4);

			indices[++number] = offset2 + 4 * i + 2;
			indices[++number] = offset2 + (4 * i + 4) % (2 * ANGLE_NUMBER);
			indices[++number] = offset2 + 4 * i + (2 * ANGLE_NUMBER + 3);

			indices[++number] = offset2 + (4 * i + 4) % (2 * ANGLE_NUMBER);
			indices[++number] = offset2 + 4 * i + (2 * ANGLE_NUMBER + 3);
			indices[++number] = offset2 + (2 * ANGLE_NUMBER + 1) + (4 * i + 4) % (2 * ANGLE_NUMBER);
		}

		// koniec kol zebatych

		offset = (2 * ANGLE_NUMBER + 1) * 2 * 2 * 12;

		GLfloat centerXcylinder = 0.0f;
		GLfloat centerYcylinder = 0.0f;
		GLfloat centerZcylinder = 0.0f;
		GLfloat sizeCylinder = 10.0f;
		GLfloat radiusCylinder = 1.5f;

		//cylinder teraz - kloda

		GLfloat fragment = 0.0f;

		for (int i = 0; i < 16; ++i)
		{
			vertices[offset + 12 * i] = cos(glm::radians(angle))*radiusCylinder + centerXcylinder;
			vertices[offset + 12 * i + 1] = -centerZcylinder;
			vertices[offset + 12 * i + 2] = sin(glm::radians(angle))*radiusCylinder + centerYcylinder;

			vertices[offset + 12 * i + 3] = 0.4f;
			vertices[offset + 12 * i + 4] = 0.4f;
			vertices[offset + 12 * i + 5] = 0.4f;

			vertices[offset + 12 * i + 7] = fragment;
			fragment += 1.0f / 16.0f;
			vertices[offset + 12 * i + 6] = 0.0f;

			vertices[offset + 12 * i + 11] = 1.0f;

			angle += 360.0 / 16.0;
		}

		vertices[offset + 12 * 16] = centerXcylinder;
		vertices[offset + 12 * 16 + 1] = -centerZcylinder;
		vertices[offset + 12 * 16 + 2] = centerYcylinder;
		vertices[offset + 12 * 16 + 3] = 50.0f / 256.0f;
		vertices[offset + 12 * 16 + 4] = 35.0f / 256.0f;
		vertices[offset + 12 * 16 + 5] = 10.0f / 256.0f;

		fragment = 0.0f;

		offset2 = (2 * ANGLE_NUMBER + 1) * 2 * 2;

		for (int i = 0; i < 16; ++i)
		{
			indices[++number] = offset2 + 16;
			indices[++number] = offset2 + i;
			indices[++number] = offset2 + (i + 1) % 16;
		}

		offset += 12 * 17;

		for (int i = 0; i < 16; ++i)
		{
			vertices[offset + 12 * i] = cos(glm::radians(angle))*radiusCylinder + centerXcylinder;
			vertices[offset + 12 * i + 1] = (0 + centerZcylinder + sizeCylinder);
			vertices[offset + 12 * i + 2] = sin(glm::radians(angle))*radiusCylinder + centerYcylinder;

			vertices[offset + 12 * i + 3] = 0.4f;
			vertices[offset + 12 * i + 4] = 0.4f;
			vertices[offset + 12 * i + 5] = 0.4f;

			vertices[offset + 12 * i + 7] = fragment;
			fragment += 1.0f / 16.0f;
			vertices[offset + 12 * i + 6] = 1.0f;

			vertices[offset + 12 * i + 11] = 1.0f;

			angle += 360.0f / 16.0f;
		}
		vertices[offset + 12 * 16] = centerXcylinder;
		vertices[offset + 12 * 16 + 1] = (centerZcylinder + sizeCylinder);
		vertices[offset + 12 * 16 + 2] = centerYcylinder;
		vertices[offset + 12 * 16 + 3] = 50.0f / 256.0f;
		vertices[offset + 12 * 16 + 4] = 35.0f / 256.0f;
		vertices[offset + 12 * 16 + 5] = 10.0f / 256.0f;

		offset2 += 17;

		for (int i = 0; i < 16; ++i)
		{
			indices[++number] = offset2 + 16;
			indices[++number] = offset2 + i;
			indices[++number] = offset2 + (i + 1) % 16;
		}

		offset2 -= 17;

		for (int i = 0; i < 16; ++i)
		{
			indices[++number] = offset2 + i;
			indices[++number] = offset2 + (1 + i) % 16;
			indices[++number] = offset2 + 17 + i;

			indices[++number] = offset2 + 17 + i;
			indices[++number] = offset2 + 17 + (i + 1) % 16;
			indices[++number] = offset2 + (1 + i) % 16;
		}

		offset += 17 * 12;

		GLfloat x_basis = 4.0f;
		GLfloat y_basis = 10.0f;
		GLfloat z_basis = 1.0f;

		GLfloat basis[20 * 12] =
		{
			x_basis, y_basis, z_basis, 0.4f, 0.4f, 0.4f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f,	//0
			x_basis, y_basis, -z_basis, 0.4f, 0.4f, 0.4f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f,	//1
			-x_basis, y_basis, -z_basis, 0.4f, 0.4f, 0.4f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f,	//2
			-x_basis, y_basis, z_basis, 0.4f, 0.4f, 0.4f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f,	//3

			x_basis, y_basis, -z_basis, 0.4f, 0.4f, 0.4f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, -1.0f,	//4
			x_basis, -y_basis, -z_basis, 0.4f, 0.4f, 0.4f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f,	//5
			-x_basis, -y_basis, -z_basis, 0.4f, 0.4f, 0.4f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, -1.0f,	//6
			-x_basis, y_basis, -z_basis, 0.4f, 0.4f, 0.4f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, -1.0f,	//7

			-x_basis, y_basis, -z_basis, 0.4f, 0.4f, 0.4f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f,	//8
			-x_basis, -y_basis, -z_basis, 0.4f, 0.4f, 0.4f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f,	//9
			-x_basis, -y_basis, z_basis, 0.4f, 0.4f, 0.4f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f,	//10
			-x_basis, y_basis, z_basis, 0.4f, 0.4f, 0.4f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f,	//11

			-x_basis, -y_basis, -z_basis, 0.4f, 0.4f, 0.4f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, -1.0f,	//12
			-x_basis, -y_basis, z_basis, 0.4f, 0.4f, 0.4f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, -1.0f,	//13
			x_basis, -y_basis, -z_basis, 0.4f, 0.4f, 0.4f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, -1.0f,	//14
			x_basis, -y_basis, z_basis, 0.4f, 0.4f, 0.4f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, -1.0f,	//15

			x_basis, y_basis, z_basis, 0.4f, 0.4f, 0.4f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,	//16
			x_basis, -y_basis, z_basis, 0.4f, 0.4f, 0.4f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,	//17
			x_basis, -y_basis, -z_basis, 0.4f, 0.4f, 0.4f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,	//18
			x_basis, y_basis, -z_basis, 0.4f, 0.4f, 0.4f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f	//19
		};

		for (int i = 0; i < 20 * 12; ++i)
			vertices[offset + i] = basis[i];

		GLuint basis_indices[10 * 3] =
		{
			0, 1, 2,
			0, 2, 3,	//tyl
			4, 5, 6,
			4, 6, 7,	//gora
			8, 9, 10,
			8, 10, 11,	//prawy
			12, 13, 14,
			13, 14, 15,	//przod
			16, 17, 18,
			16, 18, 19 //lewy
		};

		for (int i = 0; i < 30; ++i)
			basis_indices[i] += offset / 12;

		for (int i = 0; i < 10 * 3; ++i)
			indices[++number] = basis_indices[i];

		offset += 20 * 12;		//pierwszy wolny po platformie, teraz czas na podloge

		for (int i = 0; i < 4; ++i)
		{
			vertices[offset + i * 12 + 2] = 3.5f;
			vertices[offset + i * 12 + 3] = 0.2f;
			vertices[offset + i * 12 + 4] = 0.2f;
			vertices[offset + i * 12 + 5] = 0.2f;
			vertices[offset + i * 12 + 6] = i < 2 ? 1 : 0;
			vertices[offset + i * 12 + 7] = 0;
			vertices[offset + i * 12 + 11] = 0.5f;
		}

		GLfloat floor = 20.0f;

		vertices[offset] = floor;
		vertices[offset + 1] = floor;
		vertices[offset + 1 * 12] = floor;
		vertices[offset + 1 * 12 + 1] = -floor;
		vertices[offset + 2 * 12] = -floor;
		vertices[offset + 2 * 12 + 1] = -floor;
		vertices[offset + 3 * 12] = -floor;
		vertices[offset + 3 * 12 + 1] = floor;

		vertices[offset + 7] = 1;
		vertices[offset + 3 * 12 + 7] = 1;

		offset2 = offset / 12;

		offset += 4 * 12;

		indices[++number] = offset2;
		indices[++number] = offset2 + 1;
		indices[++number] = offset2 + 2;

		indices[++number] = offset2;
		indices[++number] = offset2 + 2;
		indices[++number] = offset2 + 3;

		GLfloat box[20 * 12] =
		{
			BOX, BOX, BOX, 0.4f, 0.4f, 0.4f, 0.25f, 0.66666f, 0.0f, -1.0f, 0.0f, -0.5f,	//0
			BOX, BOX, -BOX, 0.4f, 0.4f, 0.4f, 0.25f, 0.33333f, 0.0f, -1.0f, 0.0f, -0.5f,	//1
			-BOX, BOX, -BOX, 0.4f, 0.4f, 0.4f, 0.5f, 0.33333f, 0.0f, -1.0f, 0.0f, -0.5f,	//2
			-BOX, BOX, BOX, 0.4f, 0.4f, 0.4f, 0.5f, 0.66666f, 0.0f, -1.0f, 0.0f, -0.5f,	//3

			BOX, BOX, -BOX, 0.4f, 0.4f, 0.4f, 0.25f, 0.33333f, 0.0f, 0.0f, 1.0f, -0.5f,	//4
			BOX, -BOX, -BOX, 0.4f, 0.4f, 0.4f, 0.25f, 0.0f, 0.0f, 0.0f, 1.0f, -0.5f,	//5
			-BOX, -BOX, -BOX, 0.4f, 0.4f, 0.4f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, -0.5f,	//6
			-BOX, BOX, -BOX, 0.4f, 0.4f, 0.4f, 0.5f, 0.33333f, 0.0f, 0.0f, 1.0f, -0.5f,	//7

			-BOX, BOX, -BOX, 0.4f, 0.4f, 0.4f, 0.5f, 0.33333f, -1.0f, 0.0f, 0.0f, -0.5f,	//8
			-BOX, -BOX, -BOX, 0.4f, 0.4f, 0.4f, 0.75f, 0.33333f, -1.0f, 0.0f, 0.0f, -0.5f,	//9
			-BOX, -BOX, BOX, 0.4f, 0.4f, 0.4f, 0.75f, 0.66666f, -1.0f, 0.0f, 0.0f, -0.5f,	//10
			-BOX, BOX, BOX, 0.4f, 0.4f, 0.4f, 0.5f, 0.66666f, -1.0f, 0.0f, 0.0f, -0.5f,	//11

			-BOX, -BOX, -BOX, 0.4f, 0.4f, 0.4f, 0.75f, 0.33333f, 0.0f, 1.0f, 0.0f, -0.5f,	//12
			-BOX, -BOX, BOX, 0.4f, 0.4f, 0.4f, 0.75f, 0.66666f, 0.0f, 1.0f, 0.0f, -0.5f,	//13
			BOX, -BOX, -BOX, 0.4f, 0.4f, 0.4f, 1.0f, 0.33333f, 0.0f, 1.0f, 0.0f, -0.5f,	//14
			BOX, -BOX, BOX, 0.4f, 0.4f, 0.4f, 1.0f, 0.66666f, 0.0f, 1.0f, 0.0f, -0.5f,	//15

			BOX, BOX, BOX, 0.4f, 0.4f, 0.4f, 0.25f, 0.66666f, 1.0f, 0.0f, 0.0f, -0.5f,	//16
			BOX, -BOX, BOX, 0.4f, 0.4f, 0.4f, 0.0f, 0.66666f, 1.0f, 0.0f, 0.0f, -0.5f,	//17
			BOX, -BOX, -BOX, 0.4f, 0.4f, 0.4f, 0.0f, 0.33333f, 1.0f, 0.0f, 0.0f, -0.5f,	//18
			BOX, BOX, -BOX, 0.4f, 0.4f, 0.4f, 0.25f, 0.33333f, 1.0f, 0.0f, 0.0f, -0.5f	//19
		};

		for (int i = 0; i < 20 * 12; ++i)
			vertices[offset + i] = box[i];

		GLuint box_indices[10 * 3] =
		{
			0, 1, 2,
			0, 2, 3,	//tyl
			4, 5, 6,
			4, 6, 7,	//gora
			8, 9, 10,
			8, 10, 11,	//prawy
			12, 13, 14,
			13, 14, 15,	//przod
			16, 17, 18,
			16, 18, 19 //lewy
		};

		for (int i = 0; i < 30; ++i)
			box_indices[i] += offset / 12;

		for (int i = 0; i < 10 * 3; ++i)
			indices[++number] = box_indices[i];

		GLuint VBO, EBO, VAO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// vertex geometry data
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		// vertex color data
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		//vertex texture coordinates
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		//vertex texture coordinates
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 12 * sizeof(GLfloat), (GLvoid*)(11 * sizeof(GLfloat)));
		glEnableVertexAttribArray(3);

		glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

		glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)

		// Set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// Set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// prepare textures
		GLuint texture0 = LoadMipmapTexture(GL_TEXTURE0, "metal.jpg");
		GLuint texture1 = LoadMipmapTexture(GL_TEXTURE1, "drewno.jpg");
		GLuint texture2 = LoadMipmapTexture(GL_TEXTURE2, "bialy.jpg");
		GLuint texture3 = LoadMipmapTexture(GL_TEXTURE3, "podloga.jpg");
		GLuint texture4 = LoadMipmapTexture(GL_TEXTURE4, "otoczenie.jpg");

		// main event loop
		while (!glfwWindowShouldClose(window))
		{
			// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
			glfwPollEvents();

			// Clear the colorbuffer
			glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //czyszczenie bufora koloru i bufora glebokosci

			// Bind Textures using texture units

			glActiveTexture(GL_TEXTURE0);

			glBindTexture(GL_TEXTURE_2D, texture0);
			glUniform1i(glGetUniformLocation(theProgram.get_programID(), "Texture0"), 0);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, texture1);
			glUniform1i(glGetUniformLocation(theProgram.get_programID(), "Texture1"), 1);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, texture2);
			glUniform1i(glGetUniformLocation(theProgram.get_programID(), "Texture2"), 2);

			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, texture3);
			glUniform1i(glGetUniformLocation(theProgram.get_programID(), "Texture3"), 3);

			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, texture4);
			glUniform1i(glGetUniformLocation(theProgram.get_programID(), "Texture4"), 4);

			glUniform3f(glGetUniformLocation(theProgram.get_programID(), "lightColor"), red, green, blue);

			glm::mat4 trans;		// obrot kola zebatego
			static GLfloat rot_angle = 0.0f;
			trans = glm::rotate(trans, -glm::radians(rot_angle), glm::vec3(0.0f, 0.0f, 1.0f));
			if (directionDown)
				rot_angle += 0.1f * speed;
			else
				rot_angle -= 0.1f * speed;

			if (rot_angle >= 360.0f)
				rot_angle -= 360.0f;

			if (rot_angle <= 360.0f)
				rot_angle += 360.0f;

			GLuint transformLoc = glGetUniformLocation(theProgram.get_programID(), "transform");
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

			glm::mat4 trans2;		//obrot drugiego kola zebatego
			static GLfloat rot_angle2 = 0.0f;
			trans2 = glm::rotate(trans2, glm::radians(rot_angle), glm::vec3(0.0f, 0.0f, 1.0f)); //obrot wokol osi X

			GLuint transform2Loc = glGetUniformLocation(theProgram.get_programID(), "transform2");
			glUniformMatrix4fv(transform2Loc, 1, GL_FALSE, glm::value_ptr(trans2));

			glm::mat4 trans3;		//przerzucanie (powrot)
			trans3 = glm::translate(trans3, glm::vec3(centerX, centerY, centerZ));
			GLuint transform3Loc = glGetUniformLocation(theProgram.get_programID(), "move1");
			glUniformMatrix4fv(transform3Loc, 1, GL_FALSE, glm::value_ptr(trans3));

			glm::mat4 trans4;		//przerzucanie (powrot)
			trans4 = glm::translate(trans4, glm::vec3(-centerX, centerY, centerZ));
			GLuint transform4Loc = glGetUniformLocation(theProgram.get_programID(), "move2");
			glUniformMatrix4fv(transform4Loc, 1, GL_FALSE, glm::value_ptr(trans4));

			glm::mat4 trans5;		//poruszanie walcem
			static GLfloat position = 0.0f;
			trans5 = glm::translate(trans5, glm::vec3(0.0f, position, 0.0f));

			if (directionDown)
				position -= 0.00209440f * speed; //predkosc obrotu
			else
				position += 0.00209440f * speed;

			if (position < -10.0f)
				directionDown = false;
			if (position > 0.0f)
				directionDown = true;

			GLuint transform5Loc = glGetUniformLocation(theProgram.get_programID(), "move3");
			glUniformMatrix4fv(transform5Loc, 1, GL_FALSE, glm::value_ptr(trans5));

			glm::mat4 trans6;		//umiejscowienie podstawy
			trans6 = glm::translate(trans6, glm::vec3(0.0f, 0.0f, 2.5f));
			GLuint transform6Loc = glGetUniformLocation(theProgram.get_programID(), "move4");
			glUniformMatrix4fv(transform6Loc, 1, GL_FALSE, glm::value_ptr(trans6));

			glm::mat4 trans7;		//rotacja po osi
			static GLfloat rot_angle7 = 90.0f;
			trans7 = glm::rotate(trans7, glm::radians(rot_angle7), glm::vec3(1.0f, 0.0f, 0.0f));
			GLuint transform7Loc = glGetUniformLocation(theProgram.get_programID(), "change");
			glUniformMatrix4fv(transform7Loc, 1, GL_FALSE, glm::value_ptr(trans7));

			GLfloat camera_rot_angle = 0.0f;

			glm::mat4 camRot;
			camRot = glm::rotate(camRot, glm::radians(camera_rot_angle), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::vec3 cameraPos = glm::vec3(camRot * glm::vec4(0.0f, 0.0f, -3.0f, 1.0f));

			glUniform3fv(glGetUniformLocation(theProgram.get_programID(), "viewPos"), 1, glm::value_ptr(cameraPos));

			glm::mat4 view;
			glm::mat4 projection;

			if (camera_angle_Horizontal >= 360.0f || camera_angle_Horizontal <= -360.0f)
				camera_angle_Horizontal = 0.0f;
			if (camera_angle_Vertical >= 360.0f || camera_angle_Vertical <= -360.0f)
				camera_angle_Vertical = 0.0f;

			camRot = glm::rotate(camRot, glm::radians(camera_angle_Horizontal), glm::vec3(0.0, 1.0, 0.0));
			camRot = glm::rotate(camRot, glm::radians(camera_angle_Vertical), glm::vec3(1.0, 0.0, 0.0));
			cameraPos = glm::vec3(camRot * glm::vec4(0.0f, 0.0f, -zoom, 1.0f));

			view = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

			GLint viewLoc = glGetUniformLocation(theProgram.get_programID(), "view");
			GLint projectionLoc = glGetUniformLocation(theProgram.get_programID(), "projection");
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

			theProgram.Use();

			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, _countof(indices), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			// Swap the screen buffers
			glfwSwapBuffers(window);
		}
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}
	catch (exception ex)
	{
		cout << ex.what() << endl;
	}
	glfwTerminate();

	return 0;
}
