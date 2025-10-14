#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <iostream>
#include <random>
#include <vector>

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
uniform_real_distribution<float> ranPos(-0.9f, 0.9f);

GLint width = 800, height = 800;
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;

GLuint VAO, VBO;

struct SHAPE
{
	int vertexNum; // 꼭짓점 개수
	GLuint VAO, VBO;
	bool isPlus; // 합쳐졌는지 여부
	vector<float> vertices; // 꼭짓점 좌표
	float r, g, b;
	float vx, vy; // 중심 좌표
}s[12];

void reset()
{
	for (int i = 0; i < 12; ++i)
	{
		s[i].isPlus = false;
		s[i].r = color(rd);
		s[i].g = color(rd);
		s[i].b = color(rd);
		s[i].vx = ranPos(rd);
		s[i].vy = ranPos(rd);

		if (i < 3)
		{
			s[i].vertices =
			{
				s[i].vx - 0.01f, s[i].vy + 0.01f, 0.0f,
				s[i].vx - 0.01f, s[i].vy - 0.01f, 0.0f,
				s[i].vx + 0.01f, s[i].vy - 0.01f, 0.0f,

				s[i].vx - 0.01f, s[i].vy + 0.01f, 0.0f,
				s[i].vx + 0.01f, s[i].vy - 0.01f, 0.0f,
				s[i].vx + 0.01f, s[i].vy + 0.01f, 0.0f
			};
			s[i].vertexNum = 1;
		}
		else if (i >= 3 && i < 6)
		{
			s[i].vertices =
			{
				s[i].vx, s[i].vy + 0.05f, 0.0f,
				s[i].vx - 0.05f, s[i].vy - 0.03f, 0.0f,
				s[i].vx + 0.05f, s[i].vy - 0.03f, 0.0f,
			};
			s[i].vertexNum = 3;
		}
		else if (i >= 6 && i < 9)
		{
			s[i].vertices =
			{
				s[i].vx - 0.05f, s[i].vy + 0.05f, 0.0f,
				s[i].vx - 0.05f, s[i].vy - 0.05f, 0.0f,
				s[i].vx + 0.05f, s[i].vy - 0.05f, 0.0f,

				s[i].vx - 0.05f, s[i].vy + 0.05f, 0.0f,
				s[i].vx + 0.05f, s[i].vy - 0.05f, 0.0f,
				s[i].vx + 0.05f, s[i].vy + 0.05f, 0.0f
			};
			s[i].vertexNum = 4;
		}
		else
		{
			s[i].vertices =
			{
				s[i].vx, s[i].vy + 0.05f, 0.0f,
				s[i].vx - 0.06f, s[i].vy, 0.0f,
				s[i].vx + 0.06f, s[i].vy, 0.0f,

				s[i].vx - 0.06f, s[i].vy, 0.0f,
				s[i].vx - 0.04f, s[i].vy - 0.065f, 0.0f,
				s[i].vx + 0.04f, s[i].vy - 0.065f, 0.0f,

				s[i].vx - 0.06f, s[i].vy, 0.0f,
				s[i].vx + 0.04f, s[i].vy - 0.065f, 0.0f,
				s[i].vx + 0.06f, s[i].vy, 0.0f
			};
			s[i].vertexNum = 5;
		}
		s[i].VAO = VAO;
		s[i].VBO = VBO;

		glGenVertexArrays(1, &s[i].VAO);
		glGenBuffers(1, &s[i].VBO);

		glBindVertexArray(s[i].VAO);
		glBindBuffer(GL_ARRAY_BUFFER, s[i].VBO);
		glBufferData(GL_ARRAY_BUFFER, s[i].vertices.size() * sizeof(float), s[i].vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);
	}
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
	reset();
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
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	glPointSize(2.0);
	GLint loc = glGetUniformLocation(shaderProgramID, "u_color");
	for (int i = 0; i < 12; ++i)
	{
		if (!s[i].isPlus)
		{
			glUniform3f(loc, s[i].r, s[i].g, s[i].b);
			glBindVertexArray(s[i].VAO);
			glDrawArrays(GL_TRIANGLES, 0, s[i].vertices.size());
		}
	}
	glBindVertexArray(0);
	glutSwapBuffers();
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
	case 'c':
		break;
	case 's':
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