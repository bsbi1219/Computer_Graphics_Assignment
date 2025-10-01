#include <iostream>
#include <random>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

using namespace std;

int width = 800;
int height = 600;

random_device rd;
uniform_real_distribution<float> ran_Color(0.0f, 1.0f);
uniform_real_distribution<float> ran_Pos(-1.0f, 0.7f);
uniform_real_distribution<float> ran_Size(0.1f, 0.3f);
uniform_int_distribution<int> ran_Num(0, 3);

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void Mouse(int button, int state, int x, int y);

struct RECTANGLE
{
	float x, y, w, h;
	bool selected;
	float r, g, b;
	float div;
}rect[5];

struct RectPiece
{
	float x1, y1, x2, y2, x3, y3, x4, y4, x5, y5, x6, y6, x7, y7, x8, y8, w, h;
	float r, g, b;
	float size;
	bool drawn;
}piece[5];

void draw_func(float a1, float a2, float a3, float a4, int i)
{
	glRectf(a1 + piece[i].size, a2 + piece[i].size, a3 - piece[i].size, a4 - piece[i].size);
}

void draw_Rect()
{
	for (int i = 0; i < 5; ++i)
	{
		if (!rect[i].selected)
		{
			glColor3f(rect[i].r, rect[i].g, rect[i].b);
			glRectf(rect[i].x, rect[i].y, rect[i].x + rect[i].w, rect[i].y + rect[i].h);
		}
		else
		{
			if (piece[i].drawn)
			{
				if (rect[i].div == 3)
				{
					glColor3f(piece[i].r, piece[i].g, piece[i].b);
					draw_func(piece[i].x1, piece[i].y1, piece[i].x1 + piece[i].w, piece[i].y1 + piece[i].h, i);
					draw_func(piece[i].x2, piece[i].y2, piece[i].x2 + piece[i].w, piece[i].y2 + piece[i].h, i);
					draw_func(piece[i].x3, piece[i].y3, piece[i].x3 + piece[i].w, piece[i].y3 + piece[i].h, i);
					draw_func(piece[i].x4, piece[i].y4, piece[i].x4 + piece[i].w, piece[i].y4 + piece[i].h, i);
					draw_func(piece[i].x5, piece[i].y5, piece[i].x5 + piece[i].w, piece[i].y5 + piece[i].h, i);
					draw_func(piece[i].x6, piece[i].y6, piece[i].x6 + piece[i].w, piece[i].y6 + piece[i].h, i);
					draw_func(piece[i].x7, piece[i].y7, piece[i].x7 + piece[i].w, piece[i].y7 + piece[i].h, i);
					draw_func(piece[i].x8, piece[i].y8, piece[i].x8 + piece[i].w, piece[i].y8 + piece[i].h, i);
				}
				else
				{
					glColor3f(piece[i].r, piece[i].g, piece[i].b);
					draw_func(piece[i].x1, piece[i].y1, piece[i].x1 + piece[i].w, piece[i].y1 + piece[i].h, i);
					draw_func(piece[i].x2, piece[i].y2, piece[i].x2 + piece[i].w, piece[i].y2 + piece[i].h, i);
					draw_func(piece[i].x3, piece[i].y3, piece[i].x3 + piece[i].w, piece[i].y3 + piece[i].h, i);
					draw_func(piece[i].x4, piece[i].y4, piece[i].x4 + piece[i].w, piece[i].y4 + piece[i].h, i);
				}
			}
		}
	}
}

void reset()
{
	for (int i = 0; i < 5; ++i)
	{
		rect[i].x = ran_Pos(rd);
		rect[i].y = ran_Pos(rd);
		rect[i].r = ran_Color(rd);
		rect[i].g = ran_Color(rd);
		rect[i].b = ran_Color(rd);
		rect[i].w = ran_Size(rd);
		rect[i].h = ran_Size(rd);
		rect[i].div = ran_Num(rd);
		rect[i].selected = false;
	}

	for (int i = 0; i < 5; ++i)
	{
		piece[i].r = rect[i].r;
		piece[i].g = rect[i].g;
		piece[i].b = rect[i].b;
		piece[i].size = 0;
		piece[i].drawn = true;

		if (rect[i].div == 3)
		{
			piece[i].w = rect[i].w / 3;
			piece[i].h = rect[i].h / 3;

			piece[i].x1 = rect[i].x;
			piece[i].y1 = rect[i].y;

			piece[i].x2 = rect[i].x + piece[i].w;
			piece[i].y2 = rect[i].y;

			piece[i].x3 = rect[i].x + piece[i].w * 2;
			piece[i].y3 = rect[i].y;

			piece[i].x4 = rect[i].x;
			piece[i].y4 = rect[i].y + piece[i].h;

			piece[i].x5 = rect[i].x + piece[i].w * 2;
			piece[i].y5 = rect[i].y + piece[i].h;

			piece[i].x6 = rect[i].x;
			piece[i].y6 = rect[i].y + piece[i].h * 2;

			piece[i].x7 = rect[i].x + piece[i].w;
			piece[i].y7 = rect[i].y + piece[i].h * 2;

			piece[i].x8 = rect[i].x + piece[i].w * 2;
			piece[i].y8 = rect[i].y + piece[i].h * 2;
		}
		else if (rect[i].div == 0)
		{
			piece[i].w = rect[i].w / 2;
			piece[i].h = rect[i].h / 2;

			piece[i].x1 = rect[i].x;
			piece[i].y1 = rect[i].y + piece[i].h;

			piece[i].x2 = rect[i].x - piece[i].w;
			piece[i].y2 = rect[i].y;

			piece[i].x3 = rect[i].x + piece[i].w;
			piece[i].y3 = rect[i].y;

			piece[i].x4 = rect[i].x;
			piece[i].y4 = rect[i].y - piece[i].h;
		}
		else
		{
			piece[i].w = rect[i].w / 2;
			piece[i].h = rect[i].h / 2;

			piece[i].x1 = rect[i].x;
			piece[i].y1 = rect[i].y;

			piece[i].x2 = rect[i].x + piece[i].w;
			piece[i].y2 = rect[i].y;

			piece[i].x3 = rect[i].x;
			piece[i].y3 = rect[i].y + piece[i].h;

			piece[i].x4 = rect[i].x + piece[i].w;
			piece[i].y4 = rect[i].y + piece[i].h;
		}
	}
}

void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("homework_6");

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
		glutMouseFunc(Mouse);
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

void TimerFunction(int value)
{
	switch (value)
	{
	case 1:
		glutPostRedisplay();
		for (int i = 0; i < 5; ++i)
		{
			if (rect[i].selected)
			{
				if (piece[i].size > piece[i].w / 2 || piece[i].size > piece[i].h / 2)
				{
					piece[i].drawn = false;
				}
				if (rect[i].div == 0)
				{
					piece[i].y1 += 0.001f;
					piece[i].x2 -= 0.001f;
					piece[i].x3 += 0.001f;
					piece[i].y4 -= 0.001f;
				}
				else if (rect[i].div == 1)
				{
					piece[i].x1 -= 0.001f;
					piece[i].y1 += 0.001f;
					piece[i].x2 += 0.001f;
					piece[i].y2 += 0.001f;
					piece[i].x3 -= 0.001f;
					piece[i].y3 -= 0.001f;
					piece[i].x4 += 0.001f;
					piece[i].y4 -= 0.001f;
				}
				else if (rect[i].div == 2)
				{
					piece[i].x1 -= 0.001f;
					piece[i].x2 -= 0.001f;
					piece[i].x3 -= 0.001f;
					piece[i].x4 -= 0.001f;
				}
				else
				{
					piece[i].x1 -= 0.001f;
					piece[i].y1 += 0.001f;

					piece[i].y2 += 0.001f;

					piece[i].x3 += 0.001f;
					piece[i].y3 += 0.001f;

					piece[i].x4 -= 0.001f;

					piece[i].x5 += 0.001f;

					piece[i].x6 -= 0.001f;
					piece[i].y6 -= 0.001f;

					piece[i].y7 -= 0.001f;

					piece[i].x8 += 0.001f;
					piece[i].y8 -= 0.001f;
				}
				piece[i].size += 0.00007f;
			}
		}
		glutPostRedisplay();
		glutTimerFunc(5, TimerFunction, 1);
		break;
	}
}

void Mouse(int button, int state, int x, int y)
{
	float ox = (float)x / (width * 0.5f) - 1.0f;
	float oy = 1.0f - (float)y / (height * 0.5f);

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		for (int i = 0; i < 5; ++i)
		{
			if (ox >= rect[i].x && ox <= rect[i].x + rect[i].w && oy >= rect[i].y && oy <= rect[i].y + rect[i].h)
			{
				rect[i].selected = true;
				glutTimerFunc(5, TimerFunction, 1);
				break;
			}
		}
	}
	glutPostRedisplay();
}