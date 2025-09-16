#include <iostream>
#include <random>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

using namespace std;

random_device rd;
uniform_real_distribution<float> ran(0.0f, 1.0f);

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y);

bool timer_Stop = false;

float r = 0.0f, g = 0.0f, b = 1.0f;

struct Rectangle
{
	int x1, y1, x2, y2;
	float r, g, b;
	float size;

	void reset()
	{
		rect[0].x1 = -1; rect[0].y1 = 1; rect[0].x2 = 0; rect[0].y2 = 0;
		rect[0].r = 0.0f; rect[0].g = 1.0f; rect[0].b = 1.0f; rect[0].size = 0.01;

		rect[1].x1 = 0; rect[1].y1 = 1; rect[1].x2 = 1; rect[1].y2 = 0;
		rect[1].r = 1.0f; rect[1].g = 0.0f; rect[1].b = 1.0f; rect[1].size = 0.01;

		rect[2].x1 = -1; rect[2].y1 = 0; rect[2].x2 = 0; rect[2].y2 = -1;
		rect[2].r = 1.0f; rect[2].g = 1.0f; rect[2].b = 0.0f; rect[2].size = 0.01;

		rect[3].x1 = 0; rect[3].y1 = 0; rect[3].x2 = 1; rect[3].y2 = -1;
		rect[3].r = 1.0f; rect[3].g = 0.0f; rect[3].b = 0.0f; rect[3].size = 0.01;
	}

	void draw_Rectangle()
	{
		for (int i = 0; i < 4; ++i)
		{
			glColor3f(rect[i].r, rect[i].g, rect[i].b);
			glRectf(rect[i].x1 + rect[i].size, rect[i].y1 - rect[i].size, rect[i].x2 - rect[i].size, rect[i].y2 + rect[i].size);
		}
	}

	void check_Mouse_Inout()
	{

	}
}rect[4];


void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("homework_2");

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		std::cout << "GLEW Initialized\n";
		glutDisplayFunc(drawScene);
		glutReshapeFunc(Reshape);
		glutKeyboardFunc(Keyboard);
		glutMouseFunc(Mouse);
		glutMainLoop();
	}
}

GLvoid drawScene()
{
	glClearColor(r, g, b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	rect[0].reset();
	rect[0].draw_Rectangle();
	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	}
	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		rect[0].check_Mouse_Inout();
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		rect[0].check_Mouse_Inout();
	}
}
