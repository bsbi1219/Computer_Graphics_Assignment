#include <iostream>
#include <random>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

using namespace std;

int width = 800;
int height = 600;

bool left_button = false;

int select_Rect = -1;
int lap_Rect = -1;

random_device rd;
uniform_real_distribution<float> ran(0.0f, 1.0f);
uniform_real_distribution<float> ran_pos(0.0f, 1.0f);
uniform_real_distribution<float> dev_pos(0.01f, 0.06f);

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
}rect[200];

int a_Rect = 0;
int all_Rect = 0; // 출력용
int real_Rect = 0; // 실제 사각형 개수

void draw_Rect()
{
	for (int i = 0; i < all_Rect; ++i)
	{
		if (rect[i].draw)
		{
			glColor3f(rect[i].r, rect[i].g, rect[i].b);
			glRectf(rect[i].x1, rect[i].y1, rect[i].x2, rect[i].y2);
		}
	}
}

void make_Rect()
{
	if (a_Rect < 10 && real_Rect < 30)
	{
		rect[all_Rect].x1 = ran_pos(rd);
		rect[all_Rect].x2 = rect[all_Rect].x1 + 0.04f;
		rect[all_Rect].y1 = ran_pos(rd);
		rect[all_Rect].y2 = rect[all_Rect].y1 - 0.06f;
		rect[all_Rect].r = ran(rd);
		rect[all_Rect].g = ran(rd);
		rect[all_Rect].b = ran(rd);
		rect[all_Rect].draw = true;
		a_Rect++;
		all_Rect++;
		real_Rect++;
	}
}

bool is_overlap(int ox, int oy)
{
	for (int i = 0; i < all_Rect; ++i)
	{
		if (i == select_Rect) continue;

		if (!(rect[i].x1 > rect[select_Rect].x2 ||
			rect[i].x2 < rect[select_Rect].x1 ||
			rect[i].y1 < rect[select_Rect].y2 ||
			rect[i].y2 > rect[select_Rect].y1))
		{
			if (rect[i].draw)
			{
				lap_Rect = i;
				return true;
			}
		}
	}
	return false;
}

void reset()
{
	for (int i = 0; i < 200; ++i)
	{
		rect[i].draw = false;
	}
}

void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("homework_3");

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
	case 'a':
		make_Rect();
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
		left_button = true;
		for (int i = 0; i < all_Rect; ++i)
		{
			if (ox >= rect[i].x1 && ox <= rect[i].x2 && oy <= rect[i].y1 && oy >= rect[i].y2)
			{
				if (rect[i].draw == false) continue;
				select_Rect = i;
				break;
			}
		}
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		for (int i = 0; i < all_Rect; ++i)
		{
			if (ox >= rect[i].x1 && ox <= rect[i].x2 && oy <= rect[i].y1 && oy >= rect[i].y2 && rect[i].draw)
			{
				rect[i].draw = false;
				real_Rect--;

				rect[all_Rect].r = ran(rd);
				rect[all_Rect].g = ran(rd);
				rect[all_Rect].b = ran(rd);
				rect[all_Rect].x1 = rect[i].x1 - ((rect[i].x2 - rect[i].x1) / 2);
				rect[all_Rect].x2 = rect[all_Rect].x1 + dev_pos(rd) + ((rect[i].x2 - rect[i].x1) / 2);
				rect[all_Rect].y1 = rect[i].y1;
				rect[all_Rect].y2 = rect[all_Rect].y1 - dev_pos(rd) - ((rect[i].y1 - rect[i].y2) / 2);
				rect[all_Rect].draw = true;
				real_Rect++;
				all_Rect++;

				rect[all_Rect].r = ran(rd);
				rect[all_Rect].g = ran(rd);
				rect[all_Rect].b = ran(rd);
				rect[all_Rect].x1 = rect[i].x1 + ((rect[i].x2 - rect[i].x1) / 2);
				rect[all_Rect].x2 = rect[all_Rect].x1 + dev_pos(rd) + ((rect[i].x2 - rect[i].x1) / 2);
				rect[all_Rect].y1 = rect[i].y1;
				rect[all_Rect].y2 = rect[all_Rect].y1 - dev_pos(rd) - ((rect[i].y1 - rect[i].y2) / 2);
				rect[all_Rect].draw = true;
				real_Rect++;
				all_Rect++;
				break;
			}
		}
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		left_button = false;

		if (is_overlap(ox, oy))
		{
			rect[lap_Rect].draw = false;
			rect[select_Rect].draw = false;

			real_Rect -= 2;
			a_Rect -= 2;

			float nx1 = min(rect[lap_Rect].x1, rect[select_Rect].x1);
			float nx2 = max(rect[lap_Rect].x2, rect[select_Rect].x2);
			float ny1 = max(rect[lap_Rect].y1, rect[select_Rect].y1);
			float ny2 = min(rect[lap_Rect].y2, rect[select_Rect].y2);

			rect[all_Rect].r = ran(rd);
			rect[all_Rect].g = ran(rd);
			rect[all_Rect].b = ran(rd);
			rect[all_Rect].x1 = nx1;
			rect[all_Rect].x2 = nx2;
			rect[all_Rect].y1 = ny1;
			rect[all_Rect].y2 = ny2;

			rect[all_Rect].draw = true;

			real_Rect++;
			all_Rect++;
		}

		lap_Rect = -1;
		select_Rect = -1;
	}
	glutPostRedisplay();
}

void Motion(int x, int y)
{
	if (left_button && select_Rect != -1)
	{
		float ox = (float)x / (width * 0.5f) - 1.0f;
		float oy = 1.0f - (float)y / (height * 0.5f);

		// 기존 사각형의 크기 계산
		float w = rect[select_Rect].x2 - rect[select_Rect].x1;
		float h = rect[select_Rect].y1 - rect[select_Rect].y2;

		// 중심을 마우스 위치로 이동
		rect[select_Rect].x1 = ox - w / 2.0f;
		rect[select_Rect].x2 = ox + w / 2.0f;
		rect[select_Rect].y1 = oy + h / 2.0f;
		rect[select_Rect].y2 = oy - h / 2.0f;
	}
	glutPostRedisplay();
}