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

#define MOVE_SPEED 0.3f
#define RADIUS 3.0f

const GLuint WIDTH = 800, HEIGHT = 600;
//GLfloat posX = 0.0f, posY = 5.0f, posZ = -10.0f, lookX = posX, lookY = posY-sin(glm::radians(70.0)), lookZ = posZ + 3.0f, lookAngleH = 90.0f, lookAngleV = 0.0f;

static GLfloat camera_angle_Horizontal = 0.0f;
static GLfloat camera_angle_Vertical = 0.0f;


static bool directionDown = true;

static GLfloat startAngle = 360.0 / 16.0;
static GLfloat level = 2;


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		 glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_RIGHT)
		camera_angle_Horizontal += 2.5;
	if (key == GLFW_KEY_LEFT)
		 camera_angle_Horizontal -= 2.5;
	if (key == GLFW_KEY_UP && camera_angle_Vertical < 85.0)
		 camera_angle_Vertical += 2.5;
	if (key == GLFW_KEY_DOWN)
		 camera_angle_Vertical -= 2.5;
	if (key == GLFW_KEY_MINUS)
		level /= 2;
	if (key == GLFW_KEY_EQUAL)
		level *= 2;
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
		trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0)); //drugi parametr - kat w radianach, trzeci - os obrotu przechodzacej przez srodek uklady wsp. (tutaj - obrot o 90 stopnia wokol osi Z)
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
		GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "GKOM - OpenGL 05", nullptr, nullptr);
		if (window == nullptr)
			throw exception("GLFW window not created");
		glfwMakeContextCurrent(window);
		glfwSetKeyCallback(window, key_callback);

		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
			throw exception("GLEW Initialization failed");

		glViewport(0, 0, WIDTH, HEIGHT);

		glEnable(GL_DEPTH_TEST); //wlacza Z-bufor, ktory sprawdza ktore piksele rysujemy najpierw, zeby tekstury sie nie nakladaly

		// Let's check what are maximum parameters counts
		GLint nrAttributes;
		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
		cout << "Max vertex attributes allowed: " << nrAttributes << std::endl;
		glGetIntegerv(GL_MAX_TEXTURE_COORDS, &nrAttributes);
		cout << "Max texture coords allowed: " << nrAttributes << std::endl;

		// Build, compile and link shader program
		ShaderProgram theProgram("gl_05.vert", "gl_05.frag");


		GLfloat centerX = -2.7f;
		GLfloat centerY = 0.0f;
		GLfloat centerZ = 0.0f;
		GLfloat sizeGear = 1.5f;
		GLfloat radius = 1.0f;
		GLfloat teethSize = 0.2f;
		GLfloat vertices[11 * (8 * 4 + 1) * 2 * 2 +			34*11 +		 20*11];

		for (int i = 0; i < 11 * (8 * 4 + 1) * 2 * 2 +		 34 * 11		+20*11 ; ++i)
			vertices[i] = 0;

		vertices[352] = centerX;
		vertices[353] = centerY;
		vertices[354] = centerZ;
		vertices[355] = 1;

		// pierwszy wolny to 363


		vertices[352 + 363] = centerX;
		vertices[353 + 363] = centerY;
		vertices[354 + 363] = centerZ + sizeGear;
		vertices[356 + 363] = 1;

		static GLfloat angle = startAngle;

		for (int i = 0; i < 16; ++i)
		{

			vertices[22 * i] = vertices[22 * i + 363] = cos(glm::radians(angle))*radius + centerX;
			vertices[22 * i + 1] = vertices[363 + 22 * i + 1] = sin(glm::radians(angle))*radius + centerY;
			vertices[22 * i + 2] = 0 + centerZ;
			vertices[22 * i + 3] = 1;

			vertices[22 * i + 11] = vertices[363 + 22 * i + 11] = cos(glm::radians(angle))*(radius + teethSize) + centerX;
			vertices[22 * i + 12] = vertices[363 + 22 * i + 12] = sin(glm::radians(angle))*(radius + teethSize) + centerY;
			vertices[22 * i + 13] = 0 + centerZ;
			vertices[22 * i + 14] = 1;


			vertices[363 + 22 * i + 2] = sizeGear + centerZ;
			vertices[363 + 22 * i + 13] = sizeGear + centerZ;

			vertices[363 + 22 * i + 4] = 1;
			vertices[363 + 22 * i + 15] = 1;

			angle += 360.0 / 16.0;
		}


		GLuint indices[(3 * 8 * 8 + 3 * 4 * 16)		* 2 +		3 * 16		 + 3 * 16 +		 3 * 16 * 2		+	3*10];
		int number = -1;

		for (int i = 0; i < 16; ++i)
		{
			indices[++number] = 2 * i;
			indices[++number] = (2 * (i + 1)) % 32;
			indices[++number] = 32;
		}
		for (int i = 0; i < 8; ++i)
		{
			indices[++number] = 4 * i;
			indices[++number] = 4 * i + 1;
			indices[++number] = 4 * i + 2;
		}
		for (int i = 0; i < 8; ++i)
		{
			indices[++number] = 4 * i + 1;
			indices[++number] = 4 * i + 2;
			indices[++number] = 4 * i + 3;
		}




		for (int i = 0; i < 16; ++i)
		{
			indices[++number] = 33 + 2 * i;
			indices[++number] = 33 + (2 * (i + 1)) % 32;
			indices[++number] = 33 + 32;
		}
		for (int i = 0; i < 8; ++i)
		{
			indices[++number] = 33 + 4 * i;
			indices[++number] = 33 + 4 * i + 1;
			indices[++number] = 33 + 4 * i + 2;
		}
		for (int i = 0; i < 8; ++i)
		{
			indices[++number] = 33 + 4 * i + 1;
			indices[++number] = 33 + 4 * i + 2;
			indices[++number] = 33 + 4 * i + 3;
		}


		for (int i = 0; i < 8; ++i)
		{
			indices[++number] = 4 * i;
			indices[++number] = 4 * i + 1;
			indices[++number] = 4 * i + 33;

			indices[++number] = 4 * i + 1;
			indices[++number] = 4 * i + 33;
			indices[++number] = 4 * i + 34;

			indices[++number] = 4 * i + 1;
			indices[++number] = 4 * i + 3;
			indices[++number] = 4 * i + 34;

			indices[++number] = 4 * i + 3;
			indices[++number] = 4 * i + 34;
			indices[++number] = 4 * i + 36;

			indices[++number] = 4 * i + 2;
			indices[++number] = 4 * i + 3;
			indices[++number] = 4 * i + 35;

			indices[++number] = 4 * i + 3;
			indices[++number] = 4 * i + 35;
			indices[++number] = 4 * i + 36;

			indices[++number] = 4 * i + 2;
			indices[++number] = (4 * i + 4) % 32;
			indices[++number] = 4 * i + 35;

			indices[++number] = (4 * i + 4) % 32;
			indices[++number] = 4 * i + 35;
			indices[++number] = 33 + (4 * i + 4) % 32;
		}


		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




		GLint offset = 11 * (8 * 4 + 1) * 2;

		centerX = 2.7f;


		vertices[offset + 352] = centerX;
		vertices[offset + 353] = centerY;
		vertices[offset + 354] = centerZ;
		vertices[offset + 355] = 1;

		vertices[offset + 352 + 363] = centerX;
		vertices[offset + 353 + 363] = centerY;
		vertices[offset + 354 + 363] = centerZ + sizeGear;
		vertices[offset + 356 + 363] = 1;

		angle = startAngle;

		for (int i = 0; i < 16; ++i)
		{

			vertices[offset + 22 * i] = vertices[offset + 22 * i + 363] = cos(glm::radians(angle))*radius + centerX;
			vertices[offset + 22 * i + 1] = vertices[offset + 363 + 22 * i + 1] = sin(glm::radians(angle))*radius + centerY;
			vertices[offset + 22 * i + 2] = 0 + centerZ;
			vertices[offset + 22 * i + 3] = 1;

			vertices[offset + 22 * i + 11] = vertices[offset + 363 + 22 * i + 11] = cos(glm::radians(angle))*(radius + teethSize) + centerX;
			vertices[offset + 22 * i + 12] = vertices[offset + 363 + 22 * i + 12] = sin(glm::radians(angle))*(radius + teethSize) + centerY;
			vertices[offset + 22 * i + 13] = 0 + centerZ;
			vertices[offset + 22 * i + 14] = 1;


			vertices[offset + 363 + 22 * i + 2] = sizeGear + centerZ;
			vertices[offset + 363 + 22 * i + 13] = sizeGear + centerZ;

			vertices[offset + 363 + 22 * i + 4] = 1;
			vertices[offset + 363 + 22 * i + 15] = 1;

			angle += 360.0 / 16.0;
		}


		//GLuint offsetIndices = 3 * 8 * 8 + 3 * 4 * 16;
		//int number = -1;

		int offset2 = 66;

		for (int i = 0; i < 16; ++i)
		{
			indices[++number] = offset2 + 2 * i;
			indices[++number] = offset2 + (2 * (i + 1)) % 32;
			indices[++number] = offset2 + 32;
		}
		for (int i = 0; i < 8; ++i)
		{
			indices[++number] = offset2 + 4 * i;
			indices[++number] = offset2 + 4 * i + 1;
			indices[++number] = offset2 + 4 * i + 2;
		}
		for (int i = 0; i < 8; ++i)
		{
			indices[++number] = offset2 + 4 * i + 1;
			indices[++number] = offset2 + 4 * i + 2;
			indices[++number] = offset2 + 4 * i + 3;
		}




		for (int i = 0; i < 16; ++i)
		{
			indices[++number] = offset2 + 33 + 2 * i;
			indices[++number] = offset2 + 33 + (2 * (i + 1)) % 32;
			indices[++number] = offset2 + 33 + 32;
		}
		for (int i = 0; i < 8; ++i)
		{
			indices[++number] = offset2 + 33 + 4 * i;
			indices[++number] = offset2 + 33 + 4 * i + 1;
			indices[++number] = offset2 + 33 + 4 * i + 2;
		}
		for (int i = 0; i < 8; ++i)
		{
			indices[++number] = offset2 + 33 + 4 * i + 1;
			indices[++number] = offset2 + 33 + 4 * i + 2;
			indices[++number] = offset2 + 33 + 4 * i + 3;
		}


		for (int i = 0; i < 8; ++i)
		{
			indices[++number] = offset2 + 4 * i;
			indices[++number] = offset2 + 4 * i + 1;
			indices[++number] = offset2 + 4 * i + 33;

			indices[++number] = offset2 + 4 * i + 1;
			indices[++number] = offset2 + 4 * i + 33;
			indices[++number] = offset2 + 4 * i + 34;

			indices[++number] = offset2 + 4 * i + 1;
			indices[++number] = offset2 + 4 * i + 3;
			indices[++number] = offset2 + 4 * i + 34;

			indices[++number] = offset2 + 4 * i + 3;
			indices[++number] = offset2 + 4 * i + 34;
			indices[++number] = offset2 + 4 * i + 36;

			indices[++number] = offset2 + 4 * i + 2;
			indices[++number] = offset2 + 4 * i + 3;
			indices[++number] = offset2 + 4 * i + 35;

			indices[++number] = offset2 + 4 * i + 3;
			indices[++number] = offset2 + 4 * i + 35;
			indices[++number] = offset2 + 4 * i + 36;

			indices[++number] = offset2 + 4 * i + 2;
			indices[++number] = offset2 + (4 * i + 4) % 32;
			indices[++number] = offset2 + 4 * i + 35;

			indices[++number] = offset2 + (4 * i + 4) % 32;
			indices[++number] = offset2 + 4 * i + 35;
			indices[++number] = offset2 + 33 + (4 * i + 4) % 32;


		}


		///////////////////////////////////////



		offset = 1452;



		GLfloat centerXcylinder = 0.0f;
		GLfloat centerYcylinder = 0.0f;
		GLfloat centerZcylinder = 0.0f;
		GLfloat sizeCylinder = 10.0f;
		GLfloat radiusCylinder = 1.5f;

		//132 to start cylindra

		// 66 to start drugiego ko³a


		for (int i = 0; i < 16; ++i)
		{

			vertices[offset + 11 * i] = cos(glm::radians(angle))*radiusCylinder + centerXcylinder;
			vertices[offset + 11 * i + 1] = -(0 + centerZcylinder);
			vertices[offset + 11 * i + 2] = sin(glm::radians(angle))*radiusCylinder+ centerYcylinder;
			vertices[offset + 11 * i + 5] = 1;

			angle += 360.0 / 16.0;
		}

		vertices[offset + 11 * 16] = centerXcylinder;
		vertices[offset + 11 * 16 + 1] = -(centerZcylinder);
		vertices[offset + 11 * 16 + 2] = centerYcylinder;
		vertices[offset + 11 * 16 + 5] = 1;




		offset2 = 132;

		for (int i = 0; i < 16; ++i)
		{
			indices[++number] = offset2 + 16;
			indices[++number] = offset2 + i;
			indices[++number] = offset2 + (i + 1) % 16;
		}





		offset += 11 * 17;

		for (int i = 0; i < 16; ++i)
		{
			vertices[offset + 11 * i] = cos(glm::radians(angle))*radiusCylinder + centerXcylinder;
			vertices[offset + 11 * i + 1] = (0 + centerZcylinder + sizeCylinder);
			vertices[offset + 11 * i + 2] = sin(glm::radians(angle))*radiusCylinder + centerYcylinder;
			vertices[offset + 11 * i + 5] = 1;

			angle += 360.0 / 16.0;
		}
		vertices[offset + 11 * 16] = centerXcylinder;
		vertices[offset + 11 * 16 + 1] = (centerZcylinder + sizeCylinder);
		vertices[offset + 11 * 16 + 2] = centerYcylinder;
		vertices[offset + 11 * 16 + 5] = 1;




		offset2 = 149;

		for (int i = 0; i < 16; ++i)
		{
			indices[++number] = offset2 + 16;
			indices[++number] = offset2 + i;
			indices[++number] = offset2 + (i + 1) % 16;
		}


		offset2 = 132;


		for (int i = 0; i < 16; ++i)
		{
			indices[++number] = offset2 + i;
			indices[++number] = offset2 + (1+i)%16;
			indices[++number] = offset2 + 17 + i;

			indices[++number] = offset2 + 17 + i;
			indices[++number] = offset2 + 17 + (i + 1)%32;
			indices[++number] = offset2 + (1+i)%16;
		}



		offset += 17 * 11;

		GLfloat x_basis = 4.0f;
		GLfloat y_basis = 10.0f;
		GLfloat z_basis = 1.0f;

		GLfloat basis[20 * 11] =
		{
			x_basis, y_basis, z_basis, 0.4f, 0.4f, 0.4f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,	//0
			x_basis, y_basis, -z_basis, 0.4f, 0.4f, 0.4f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,	//1
			-x_basis, y_basis, -z_basis, 0.4f, 0.4f, 0.4f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,	//2
			-x_basis, y_basis, z_basis, 0.4f, 0.4f, 0.4f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,	//3

			x_basis, y_basis, -z_basis, 0.4f, 0.4f, 0.4f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,	//4
			x_basis, -y_basis, -z_basis, 0.4f, 0.4f, 0.4f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,	//5
			-x_basis, -y_basis, -z_basis, 0.4f, 0.4f, 0.4f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,	//6
			-x_basis, y_basis, -z_basis, 0.4f, 0.4f, 0.4f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,	//7

			-x_basis, y_basis, -z_basis, 0.4f, 0.4f, 0.4f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,	//8
			-x_basis, -y_basis, -z_basis, 0.4f, 0.4f, 0.4f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,	//9
			-x_basis, -y_basis, z_basis, 0.4f, 0.4f, 0.4f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,	//10
			-x_basis, y_basis, z_basis, 0.4f, 0.4f, 0.4f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,	//11

			-x_basis, -y_basis, -z_basis, 0.4f, 0.4f, 0.4f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,	//12
			-x_basis, -y_basis, z_basis, 0.4f, 0.4f, 0.4f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,	//13
			x_basis, -y_basis, -z_basis, 0.4f, 0.4f, 0.4f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,	//14
			x_basis, -y_basis, z_basis, 0.4f, 0.4f, 0.4f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,	//15

			x_basis, y_basis, z_basis, 0.4f, 0.4f, 0.4f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,	//16
			x_basis, -y_basis, z_basis, 0.4f, 0.4f, 0.4f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,	//17
			x_basis, -y_basis, -z_basis, 0.4f, 0.4f, 0.4f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,	//18
			x_basis, y_basis, -z_basis, 0.4f, 0.4f, 0.4f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,	//19

		};

		for (int i = 0; i < 20 * 11; ++i)
			vertices[offset+i] = basis[i];

		for (int i = 0; i < offset/11 + 20; ++i)
			cout << i << " )  " << vertices[11 * i] << vertices[11 * i+1] << vertices[11 * i+2] << endl;

		_sleep(1000);

		GLuint basis_indices[10 * 3] = {
			0, 1, 2,
			0, 2, 3,	//Podstawa-top
			4, 5, 6,
			4, 6, 7,	//Podstawa-right
			8, 9, 10,
			8, 10, 11,	//Podstawa-back
			12, 13, 14,
			13, 14, 15,	//
			16, 17, 18,
			16, 18, 19 //Podstawa-front
		};

		for (int i = 0; i < 30; ++i)
			basis_indices[i] += offset / 11;

		for (int i = 0; i < 10 * 3; ++i)
			indices[++number] = basis_indices[i];


//
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
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);

		// vertex color data
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		// vertex texture coordinates
		// TEKSTURY glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
		// TEKSTURY glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

		glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)

		// Set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// Set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// prepare textures
		GLuint texture0 = LoadMipmapTexture(GL_TEXTURE0, "iipw.png");
		GLuint texture1 = LoadMipmapTexture(GL_TEXTURE1, "weiti.png");

		// main event loop
		while (!glfwWindowShouldClose(window))
		{
			// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
			glfwPollEvents();

			// Clear the colorbuffer
			glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //czyszczenie bufora koloru i bufora glebokosci

			// Bind Textures using texture units
			/* TEKSTURY
			glActiveTexture(GL_TEXTURE0);

			glBindTexture(GL_TEXTURE_2D, texture0);
			glUniform1i(glGetUniformLocation(theProgram.get_programID(), "Texture0"), 0);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, texture1);
			glUniform1i(glGetUniformLocation(theProgram.get_programID(), "Texture1"), 1);
			*/



			glm::mat4 trans;
			static GLfloat rot_angle = 0.0f;
			trans = glm::rotate(trans, -glm::radians(rot_angle), glm::vec3(0.0f, 0.0f, 1.0f)); //obrot wokol osi X
			if (directionDown)
				rot_angle += 0.1f * level; //predkosc obrotu
			else
				rot_angle -= 0.1f * level;

			if (rot_angle >= 360.0f)
				rot_angle -= 360.0f;

			if (rot_angle <= 360.0f)
				rot_angle += 360.0f;

			GLuint transformLoc = glGetUniformLocation(theProgram.get_programID(), "transform");
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));


			glm::mat4 trans2;
			static GLfloat rot_angle2 = 0.0f;
			trans2 = glm::rotate(trans2, glm::radians(rot_angle2), glm::vec3(0.0f, 0.0f, 1.0f)); //obrot wokol osi X
			if (directionDown)
				rot_angle2 += 0.1f * level; //predkosc obrotu
			else
				rot_angle2 -= 0.1f * level;

			if (rot_angle2 >= 360.0f)
				rot_angle2 -= 360.0f;

			if (rot_angle2 <= 360.0f)
				rot_angle2 += 360.0f;
			GLuint transform2Loc = glGetUniformLocation(theProgram.get_programID(), "transform2");
			glUniformMatrix4fv(transform2Loc, 1, GL_FALSE, glm::value_ptr(trans2));

			glm::mat4 trans3;
			//static GLfloat rot_angle2 = 0.0f;
			trans3 = glm::translate(trans3, glm::vec3(centerX, centerY, centerZ));
			//rot_angle2 -= 0.1f; //predkosc obrotu
			//if (rot_angle2 <= 0.0f)
			//	rot_angle2 += 360.0f;
			GLuint transform3Loc = glGetUniformLocation(theProgram.get_programID(), "move1");
			glUniformMatrix4fv(transform3Loc, 1, GL_FALSE, glm::value_ptr(trans3));

			glm::mat4 trans4;
			//static GLfloat rot_angle2 = 0.0f;
			trans4 = glm::translate(trans4, glm::vec3(-centerX, centerY, centerZ));
			//rot_angle2 -= 0.1f; //predkosc obrotu
			//if (rot_angle2 <= 0.0f)
			//	rot_angle2 += 360.0f;
			GLuint transform4Loc = glGetUniformLocation(theProgram.get_programID(), "move2");
			glUniformMatrix4fv(transform4Loc, 1, GL_FALSE, glm::value_ptr(trans4));



			glm::mat4 trans5;
			static GLfloat speed = 0.0f;
			trans5 = glm::translate(trans5, glm::vec3(0.0f, speed, 0.0f));

			if (directionDown)
				speed -= 0.00209440f * level; //predkosc obrotu
			else
				speed += 0.00209440f * level;





			//speed -= 0.00209440; //predkosc obrotu

			//cout << speed << endl;

			//cout << directionDown << endl;

			if (speed < -10.0f){
				directionDown = false;
				cout << "daje na fols" << endl;
				_sleep(100);
			}
			if (speed > 0.0f)
			{
				directionDown = true;
				cout << "daje na tru" << endl;
				_sleep(100);
			}

			GLuint transform5Loc = glGetUniformLocation(theProgram.get_programID(), "move3");
			glUniformMatrix4fv(transform5Loc, 1, GL_FALSE, glm::value_ptr(trans5));







			glm::mat4 trans6;
			//static GLfloat rot_angle2 = 0.0f;
			trans6 = glm::translate(trans6, glm::vec3(0.0f, 0.0f, 2.5f));
			//rot_angle2 -= 0.1f; //predkosc obrotu
			//if (rot_angle2 <= 0.0f)
			//	rot_angle2 += 360.0f;
			GLuint transform6Loc = glGetUniformLocation(theProgram.get_programID(), "move4");
			glUniformMatrix4fv(transform6Loc, 1, GL_FALSE, glm::value_ptr(trans6));


			glm::mat4 trans7;
			static GLfloat rot_angle7 = 90.0f;
			trans7 = glm::rotate(trans7, glm::radians(rot_angle7), glm::vec3(1.0f, 0.0f, 0.0f));
			//rot_angle2 -= 0.1f; //predkosc obrotu
			//if (rot_angle2 <= 0.0f)
			//	rot_angle2 += 360.0f;
			GLuint transform7Loc = glGetUniformLocation(theProgram.get_programID(), "change");
			glUniformMatrix4fv(transform7Loc, 1, GL_FALSE, glm::value_ptr(trans7));












			GLfloat camera_rot_angle = 0.0f;

			glm::mat4 camRot;
			camRot = glm::rotate(camRot, glm::radians(camera_rot_angle), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::vec3 cameraPos = glm::vec3(camRot * glm::vec4(0.0f, 0.0f, -3.0f, 1.0f));
			
				glUniform3fv(glGetUniformLocation(theProgram.get_programID(), "viewPos"), 1, glm::value_ptr(cameraPos)); //chyba niepotrzebne
						//----------------
				
				glm::mat4 view;
			glm::mat4 projection;



			if (camera_angle_Horizontal >= 360.0f || camera_angle_Horizontal <= -360.0f)
				 camera_angle_Horizontal = 0.0f;
			if (camera_angle_Vertical >= 360.0f || camera_angle_Vertical <= -360.0f)
				 camera_angle_Vertical = 0.0f;

			camRot = glm::rotate(camRot, glm::radians(camera_angle_Horizontal), glm::vec3(0.0, 1.0, 0.0));
			camRot = glm::rotate(camRot, glm::radians(camera_angle_Vertical), glm::vec3(1.0, 0.0, 0.0));
			cameraPos = glm::vec3(camRot * glm::vec4(0.0f, 0.0f, -20.0f, 1.0f));
						//glm::mat4 view;
							//glm::mat4 projection;
				view = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
			
				
				
							////
				GLint viewLoc = glGetUniformLocation(theProgram.get_programID(), "view");
			GLint projectionLoc = glGetUniformLocation(theProgram.get_programID(), "projection");
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));






			////----------------
			////glm::mat4 camRot;
			////camRot = glm::rotate(camRot, glm::radians(rot_angle), glm::vec3(0.0f, 1.0f, 0.0f));
			////glm::vec3 cameraPos = glm::vec3(camRot * glm::vec4(0.0f, 0.0f, -3.0f, 1.0f));
			//glm::vec3 cameraPos = glm::vec3(posX, posY, posZ);

			//glUniform3fv(glGetUniformLocation(theProgram.get_programID(), "viewPos"), 1, glm::value_ptr(cameraPos)); //chyba niepotrzebne
			////----------------

			//glm::mat4 view;
			//glm::mat4 projection;

			////view = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f)); //to zamiast glm::lookAt i linijek miedzy kreskami
			////view = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			//view = glm::lookAt(cameraPos, glm::vec3(lookX, lookY, lookZ), glm::vec3(0.0f, 1.0f, 0.0f));
			//projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

			//GLint viewLoc = glGetUniformLocation(theProgram.get_programID(), "view");
			//GLint projectionLoc = glGetUniformLocation(theProgram.get_programID(), "projection");
			//glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			//glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));




			// Draw our first triangle
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