#include <iostream>
#include <random>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

using namespace std;

random_device rd;
uniform_real_distribution<float> ran(0.0f, 1.0f);
uniform_real_distribution<float> ran_Pos(-1.0f, 1.0f);

int width = 800;
int height = 600;

float erase_r = 0, erase_g = 0, erase_b = 0;
float erase_x = 0, erase_y = 0;
bool eraser = false;

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);

struct RECTANGLE
{
	float x1, y1, x2, y2;
	float r, g, b;
	bool draw;
}rect[30];

void reset()
{
	for (int i = 0; i < 30; ++i)
	{
		rect[i].draw = true;
		rect[i].r = ran(rd);
		rect[i].g = ran(rd);
		rect[i].b = ran(rd);
		rect[i].x1 = ran_Pos(rd);
		rect[i].y1 = ran_Pos(rd);
		rect[i].x2 = rect[i].x1 + 0.06f;
		rect[i].y2 = rect[i].y1 - 0.08f;
	}
}

void draw_Rect()
{
	for (int i = 0; i < 30; ++i)
	{
		if (rect[i].draw)
		{
			glColor3f(rect[i].r, rect[i].g, rect[i].b);
			glRectf(rect[i].x1, rect[i].y1, rect[i].x2, rect[i].y2);
		}
	}

	if (eraser)
	{
		glColor3f(erase_r, erase_g, erase_b);
		glRectf(erase_x - 0.06f, erase_y + 0.08f, erase_x + 0.06f, erase_y - 0.08f);
	}
}

void hit_Erase(float ox, float oy)
{
	for (int i = 0; i < 30; ++i)
	{
		if (rect[i].draw)
		{
			if (!(ox + 0.06f < rect[i].x1 || ox - 0.06f > rect[i].x2 || oy + 0.08f < rect[i].y2 || oy - 0.08f > rect[i].y1))
			{
				rect[i].draw = false;
				erase_r = rect[i].r;
				erase_g = rect[i].g;
				erase_b = rect[i].b;
			}
		}
	}
}

void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("homework_5");

	glewExperimental = GL_TRUE;
	reset();

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
		glutMotionFunc(Motion);
		glutMainLoop();
	}
}

GLvoid drawScene()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	draw_Rect();
	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	width = w;
	height = h;
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'r':
		reset();
		break;
	}
	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y)
{
	float ox = (float)x / (width * 0.5f) - 1.0f;
	float oy = 1.0f - (float)y / (height * 0.5f);
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		eraser = true;
		erase_x = ox;
		erase_y = oy;
		erase_r = 0;
		erase_g = 0;
		erase_b = 0;
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		for (int i = 0; i < 30; ++i)
		{
			if (!rect[i].draw)
			{
				rect[i].draw = true;
				rect[i].r = ran(rd);
				rect[i].g = ran(rd);
				rect[i].b = ran(rd);
				rect[i].x1 = ox;
				rect[i].y1 = oy;
				rect[i].x2 = rect[i].x1 + 0.06f;
				rect[i].y2 = rect[i].y1 - 0.08f;
				break;
			}
		}
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		eraser = false;
	}
	glutPostRedisplay();
}

void Motion(int x, int y)
{
	if (eraser)
	{
		float ox = (float)x / (width * 0.5f) - 1.0f;
		float oy = 1.0f - (float)y / (height * 0.5f);
		erase_x = ox;
		erase_y = oy;
		hit_Erase(ox, oy);
	}
	glutPostRedisplay();
}