#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <iostream>
#include <random>

using namespace std;

void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y);
void TimerFunction(int value);
char* filetobuf(const char* file);

random_device rd;
uniform_real_distribution<float> color(0.0f, 1.0f);

GLint width = 800, height = 800;
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;

GLuint VAO_L, VBO_L;

bool shape3Done = false;
bool shape1Done = false;
bool anyAnimating = false;

struct SHAPE
{
	GLuint VAO, VBO;
	GLfloat vertices[18];
	GLfloat r, g, b;
	bool animating = false;
}shape[4];

void makeLine()
{
	GLfloat lines[] =
	{
		-1.0f, 0.0f,
		1.0f, 0.0f,

		0.0f, 1.0f,
		0.0f, -1.0f
	};

	glGenVertexArrays(1, &VAO_L);
	glGenBuffers(1, &VBO_L);

	glBindVertexArray(VAO_L);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_L);

	glBufferData(GL_ARRAY_BUFFER, sizeof(lines), lines, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void drawLine() // 선->삼각
{
	GLfloat temp[] =
	{
		-0.8f, 0.2f,
		-0.5f, 0.5f,
		-0.2f, 0.8f,

		-0.8f, 0.2f,
		-0.5f, 0.5f,
		-0.2f, 0.8f,

		-0.8f, 0.2f,
		-0.5f, 0.5f,
		-0.2f, 0.8f,
	};

	memcpy(shape[0].vertices, temp, sizeof(temp));

	GLuint VAO, VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(shape[0].vertices), shape[0].vertices, GL_STATIC_DRAW);;

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	shape[0].r = color(rd);
	shape[0].g = color(rd);
	shape[0].b = color(rd);
	shape[0].VAO = VAO;
	shape[0].VBO = VBO;

	cout << "shape[0].VAO: " << shape[0].VAO << ", VBO: " << shape[0].VBO << endl;
}

void drawTri() // 삼각->사각
{
	GLfloat temp[] =
	{
		0.5f, 0.8f,
		0.2f, 0.2f,
		0.8f, 0.2f,

		0.5f, 0.8f,
		0.5f, 0.8f,
		0.2f, 0.2f,

		0.5f, 0.8f,
		0.5f, 0.8f,
		0.8f, 0.2f,
	};
	memcpy(shape[1].vertices, temp, sizeof(temp));

	GLuint VAO, VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(shape[1].vertices), shape[1].vertices, GL_STATIC_DRAW);;

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	shape[1].r = color(rd);
	shape[1].g = color(rd);
	shape[1].b = color(rd);
	shape[1].VAO = VAO;
	shape[1].VBO = VBO;

	cout << "shape[1].VAO: " << shape[1].VAO << ", VBO: " << shape[1].VBO << endl;
}

void drawRect() // 사각->오각
{
	GLfloat temp[] =
	{
		-0.8f, -0.2f,
		-0.8f, -0.8f,
		-0.2f, -0.8f,

		-0.8f, -0.2f,
		-0.2f, -0.2f,
		-0.2f, -0.8f,

		-0.8f, -0.2f,
		-0.5f, -0.2f,
		-0.2f, -0.2f,
	};
	memcpy(shape[2].vertices, temp, sizeof(temp));

	GLuint VAO, VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(shape[2].vertices), shape[2].vertices, GL_STATIC_DRAW);;

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	shape[2].r = color(rd);
	shape[2].g = color(rd);
	shape[2].b = color(rd);
	shape[2].VAO = VAO;
	shape[2].VBO = VBO;

	cout << "shape[2].VAO: " << shape[2].VAO << ", VBO: " << shape[2].VBO << endl;
}

void drawPent() // 오각->선
{
	GLfloat temp[] =
	{
		0.2f, -0.4f,
		0.3f, -0.8f,
		0.7f, -0.8f,

		0.2f, -0.4f,
		0.7f, -0.8f,
		0.8f, -0.4f,

		0.5f, -0.2f,
		0.2f, -0.4f,
		0.8f, -0.4f,
	};
	memcpy(shape[3].vertices, temp, sizeof(temp));

	GLuint VAO, VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(shape[3].vertices), shape[3].vertices, GL_STATIC_DRAW);;

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	shape[3].r = color(rd);
	shape[3].g = color(rd);
	shape[3].b = color(rd);
	shape[3].VAO = VAO;
	shape[3].VBO = VBO;

	cout << "shape[3].VAO: " << shape[3].VAO << ", VBO: " << shape[3].VBO << endl;
}

void main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("Example1");
	glewExperimental = GL_TRUE;
	glewInit();
	makeLine();
	drawLine(); 
	drawTri();
	drawRect();
	drawPent();
	make_vertexShaders();
	make_fragmentShaders();
	shaderProgramID = make_shaderProgram();
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse); 
	glutMainLoop();
}

GLvoid drawScene()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	glPointSize(2.0);

	int centerLoc = glGetUniformLocation(shaderProgramID, "u_center");
	GLint scaleLoc = glGetUniformLocation(shaderProgramID, "u_scale");
	GLint loc = glGetUniformLocation(shaderProgramID, "u_color");
	float centers[4][2] = {
	{-0.85f,  0.85f},
	{ 0.85f,  0.85f},
	{-0.85f, -0.85f},
	{ 0.85f, -0.85f}
	};

	for (int i = 0; i < 4; ++i)
	{
		if (shape[i].animating)
		{
			anyAnimating = true;
			glUniform2f(centerLoc, centers[i][0], centers[i][1]);
			glUniform1f(scaleLoc, shape[i].animating ? 2.3f : 1.0f);

			glUniform3f(loc, shape[i].r, shape[i].g, shape[i].b);
			glBindVertexArray(shape[i].VAO);
			if (i == 0)
			{
				if (shape[i].animating || shape1Done)
					glDrawArrays(GL_TRIANGLES, 0, 9);
				else
					glDrawArrays(GL_LINES, 0, 9);
			}
			else if (i == 3 && shape3Done)
			{
				glDrawArrays(GL_LINES, 0, 3);
			}
			else if (i == 3 && !shape3Done)
			{
				glDrawArrays(GL_TRIANGLES, 0, 3);
			}
			else
				glDrawArrays(GL_TRIANGLES, 0, 9);
			break;
		}
	}
	if (!anyAnimating)
	{
		for (int i = 0; i < 4; ++i)
		{
			glUniform2f(centerLoc, centers[i][0], centers[i][1]);
			glUniform1f(scaleLoc, 1.0f);

			glUniform3f(loc, 0.0f, 0.0f, 0.0f);
			glBindVertexArray(VAO_L);
			glDrawArrays(GL_LINES, 0, 4);

			glUniform3f(loc, shape[i].r, shape[i].g, shape[i].b);
			glBindVertexArray(shape[i].VAO);
			if (i == 0)
			{
				if (shape[i].animating || shape1Done)
					glDrawArrays(GL_TRIANGLES, 0, 9);
				else
					glDrawArrays(GL_LINES, 0, 9);
			}
			else if (i == 3 && shape3Done)
			{
				glDrawArrays(GL_LINES, 0, 9);
			}
			else
				glDrawArrays(GL_TRIANGLES, 0, 9);
		}
	}
	glBindVertexArray(0);
	glutSwapBuffers();
}

void shapeAnimation(char o, float t)
{
	switch (o)
	{
	case 'l':
		if (shape[0].vertices[4] >= -0.5f)
		{
			shape[0].vertices[2] += t;
			shape[0].vertices[3] -= t;
			shape[0].vertices[4] -= t;
		}
		else
		{
			shape[0].animating = false;
			shape1Done = true;
			anyAnimating = false;
		}
		glBindVertexArray(shape[0].VAO);
		glBindBuffer(GL_ARRAY_BUFFER, shape[0].VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(shape[0].vertices), shape[0].vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		break;
	case 't':
		if (shape[1].vertices[6] >= 0.2f)
		{
			shape[1].vertices[6] -= t;
			shape[1].vertices[12] += t;

			glBindVertexArray(shape[1].VAO);
			glBindBuffer(GL_ARRAY_BUFFER, shape[1].VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(shape[1].vertices), shape[1].vertices);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
		else
		{
			shape[1].animating = false;
			anyAnimating = false;
		}
		break;
	case 'r':
		if (shape[2].vertices[13] >= -0.4f)
		{
			shape[2].vertices[1] -= t;
			shape[2].vertices[7] -= t;
			shape[2].vertices[9] -= t;
			shape[2].vertices[13] -= t;
			shape[2].vertices[17] -= t;
		}
		else
		{
			shape[2].animating = false;
			anyAnimating = false;
		}
		if (shape[2].vertices[2] <= -0.7f)
		{
			shape[2].vertices[2] += t;
			shape[2].vertices[4] -= t;
			shape[2].vertices[10] -= t;
		}
		glBindVertexArray(shape[2].VAO);
		glBindBuffer(GL_ARRAY_BUFFER, shape[2].VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(shape[2].vertices), shape[2].vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		break;
	case 'p':
		if (shape[3].vertices[4] < 0.8f)
		{
			shape[3].vertices[4] += t;
			shape[3].vertices[15] -= t;
		}
		if (shape[3].vertices[2] < 0.5f)
		{
			shape[3].vertices[2] += t;
			shape[3].vertices[8] -= t;
			shape[3].vertices[11] += t;
			shape[3].vertices[17] += t;
		}
		if (shape[3].vertices[3] < -0.5f)
		{
			shape[3].vertices[3] += t;
			shape[3].vertices[9] += t;
			shape[3].vertices[12] -= t;
			shape[3].vertices[14] += t;
		}
		if (shape[3].vertices[1] > -0.8f)
		{
			shape[3].vertices[1] -= t;
			shape[3].vertices[7] -= t;
		}
		if (shape[3].vertices[5] < -0.2f)
		{
			shape[3].vertices[5] += t;
			shape[3].vertices[13] -= t;
		}
		else
		{
			shape[3].animating = false;
			shape3Done = true;
			for (int i = 0; i < 18; ++i)
				cout << shape[3].vertices[i] << endl;
			anyAnimating = false;
		}
		glBindVertexArray(shape[3].VAO);
		glBindBuffer(GL_ARRAY_BUFFER, shape[3].VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(shape[3].vertices), shape[3].vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		break;
	}
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

void make_vertexShaders()
{
	GLchar* vertexSource;
	vertexSource = filetobuf("vertex.glsl");
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader\n" << errorLog << std::endl;
		return;
	}
}

void make_fragmentShaders()
{
	GLchar* fragmentSource;
	fragmentSource = filetobuf("fragment.glsl");
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: frag_shader\n" << errorLog << std::endl;
		return;
	}
}

GLuint make_shaderProgram()
{
	GLint result;
	GLchar* errorLog = NULL;
	GLuint shaderID;
	shaderID = glCreateProgram();
	glAttachShader(shaderID, vertexShader);
	glAttachShader(shaderID, fragmentShader);
	glLinkProgram(shaderID);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program\n" << errorLog << std::endl;
		return false;
	}
	glUseProgram(shaderID);
	return shaderID;
}

char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb"); // Open file for reading
	if (!fptr) // Return NULL on failure
		return NULL;
	fseek(fptr, 0, SEEK_END); // Seek to the end of the file
	length = ftell(fptr); // Find out how many bytes into the file we are
	buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator
	fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file
	fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer
	fclose(fptr); // Close the file
	buf[length] = 0; // Null terminator
	return buf; // Return the buffer
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'l':
		shape[0].animating = true;
		glutTimerFunc(16, TimerFunction, 1);
		break;
	case 't':
		shape[1].animating = true;
		glutTimerFunc(16, TimerFunction, 2);
		break;
	case 'r':
		shape[2].animating = true;
		glutTimerFunc(16, TimerFunction, 3);
		break;
	case 'p':
		shape[3].animating = true;
		glutTimerFunc(16, TimerFunction, 4);
		break;
	case 'a':
		makeLine();
		drawLine();
		drawTri();
		drawRect();
		drawPent();
		shape1Done = false;
		shape3Done = false;
		break;
	case 'q':
		exit(0);
	}
	glutPostRedisplay();
}

void TimerFunction(int value)
{
	switch (value)
	{
	case 1:
		if (shape[0].animating)
		{
			shapeAnimation('l', 0.01f);
			glutPostRedisplay();
			glutTimerFunc(16, TimerFunction, 1);
		}
		break;
	case 2:
		if (shape[1].animating)
		{
			shapeAnimation('t', 0.01f);
			glutPostRedisplay();
			glutTimerFunc(16, TimerFunction, 2);
		}
		break;
	case 3:
		if (shape[2].animating)
		{
			shapeAnimation('r', 0.01f);
			glutPostRedisplay();
			glutTimerFunc(16, TimerFunction, 3);
		}
		break;
	case 4:
		if (shape[3].animating)
		{
			shapeAnimation('p', 0.05f);
			glutPostRedisplay();
			glutTimerFunc(16, TimerFunction, 4);
		}
		break;
	}
}

void Mouse(int button, int state, int x, int y)
{
	float ox = (float)x / width * 2.0f - 1.0f;
	float oy = 1.0f - (float)y / height * 2.0f;
	if ((button == GLUT_LEFT_BUTTON && state == GLUT_DOWN))
	{
	}
	glutPostRedisplay();
}