#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <iostream>
#include <random>
#define PI 3.14

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
uniform_real_distribution<float> pos(-1.0f, 1.0f);
uniform_int_distribution<int> ran_dic(0, 1);

GLint width = 1200, height = 1200;
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;

GLuint VAO, VBO, EBO;

int spiral_num = 1;
bool line = false;

float startX = 0.0f, startY = 0.0f;
bool moveStart = false;
float r;
float rad = 0.5f;

GLfloat colorR;
GLfloat colorG;
GLfloat colorB;

struct Spiral {
	float startX, startY;
	float r, rad;
	int dic;
	GLuint VAO, VBO;
	bool active;
	bool outspiral = true;
	vector<float> points;
	float endX, endY;
	float newX, newY;
};

vector<Spiral> spirals;

void reset()
{
	spirals.clear();
	colorR = 0.0f;
	colorG = 0.0f;
	colorB = 0.0f;
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
	glClearColor(colorR, colorG, colorB, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	glPointSize(2.0);
	GLint loc = glGetUniformLocation(shaderProgramID, "u_color");
	glUniform3f(loc, 1.0f, 1.0f, 1.0f);
	for (auto& s : spirals)
	{
		glBindVertexArray(s.VAO);
		if (line)
			glDrawArrays(GL_LINE_STRIP, 0, s.points.size() / 2);
		else
			glDrawArrays(GL_POINTS, 0, s.points.size() / 2);
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
	case 'p':
		line = false;
		break;
	case 'l':
		line = true;
		break;
	case '1':
		spiral_num = 1;
		break;
	case '2':
		spiral_num = 2;
		break;
	case '3':
		spiral_num = 3;
		break;
	case '4':
		spiral_num = 4;
		break;
	case '5':
		spiral_num = 5;
		break;
	case 'c':
		reset();
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
		for (auto& s : spirals)
		{
			if (!s.active) continue;

			if (s.outspiral)
			{
				if (s.dic == 0) s.rad -= 0.1f;
				else s.rad += 0.1f;

				s.r += 0.0005f;

				if (s.rad > 2 * PI) s.rad -= 2 * PI;
				else if (s.rad < 0) s.rad += 2 * PI;

				float x = s.startX + s.r * cos(s.rad);
				float y = s.startY + s.r * sin(s.rad);

				s.points.push_back(x);
				s.points.push_back(y);

				s.endX = x;
				s.endY = y;

				if (s.r >= 0.1f)
				{
					s.outspiral = false;

					float vx = s.endX - s.startX;
					float vy = s.endY - s.startY;
					float v = sqrt(vx * vx + vy * vy);
					vx /= v;
					vy /= v;

					s.newX = s.endX + vx * 0.1f;
					s.newY = s.endY + vy * 0.1f;

					s.r = 0.1f;
				}
			}
			else
			{
				if (s.r <= 0.0f)
				{
					continue;
				}
				if (s.dic == 0) s.rad += 0.1f;
				else s.rad -= 0.1f;

				s.r -= 0.0005f;

				if (s.rad > 2 * PI) s.rad -= 2 * PI;
				else if (s.rad < 0) s.rad += 2 * PI;

				float x = s.newX + s.r * -cos(s.rad);
				float y = s.newY + s.r * -sin(s.rad);

				s.points.push_back(x);
				s.points.push_back(y);
			}

			glBindVertexArray(s.VAO);
			glBindBuffer(GL_ARRAY_BUFFER, s.VBO);

			glBufferData(GL_ARRAY_BUFFER, s.points.size() * sizeof(float), s.points.data(), GL_DYNAMIC_DRAW);

			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
		glutPostRedisplay();
		glutTimerFunc(16, TimerFunction, 1);
		break;
	}
}

void Mouse(int button, int state, int x, int y)
{
	float ox = (float)x / width * 2.0f - 1.0f;
	float oy = 1.0f - (float)y / height * 2.0f;
	if ((button == GLUT_LEFT_BUTTON && state == GLUT_DOWN))
	{
		colorR = color(rd);
		colorG = color(rd);
		colorB = color(rd);

		Spiral s;
		s.startX = ox;
		s.startY = oy;
		s.r = 0.0f;
		s.rad = 0.5f;
		s.active = true;
		s.dic = ran_dic(rd);

		glGenVertexArrays(1, &s.VAO);
		glGenBuffers(1, &s.VBO);

		spirals.push_back(s);

		for (int i = 1; i < spiral_num; ++i)
		{
			Spiral r;
			r.startX = pos(rd);
			r.startY = pos(rd);
			r.r = 0;
			r.rad = 0;
			r.active = true;
			r.dic = ran_dic(rd);
			glGenVertexArrays(1, &r.VAO);
			glGenBuffers(1, &r.VBO);
			spirals.push_back(r);
		}

		if (spirals.size() >  0)
			glutTimerFunc(16, TimerFunction, 1);
	}
	glutPostRedisplay();
}