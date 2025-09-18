#include <iostream>
#include <random>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

using namespace std;

random_device rd;
uniform_real_distribution<float> ran_Color(0.0f, 1.0f);
uniform_real_distribution<float> ran_Size(0.05f, 0.2f);
uniform_int_distribution<int> move_ud(0, 1);
uniform_int_distribution<int> move_lr(2, 3);

int width = 600;
int height = 600;

bool timer_1 = false;
bool timer_2 = false;
bool timer_3 = false;
bool timer_4 = false;
bool timer_5 = false;

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y);

struct RECTANGLE
{
	float ox, oy;
	float mx, my;
	float r, g, b;
	float size;
	bool drawn;
	int move_dir_ud; // 0 : 위, 1 : 아래, 2 : 왼쪽, 3 : 오른쪽
	int move_dir_lr;
}rect[5];

void reset()
{
	for (int i = 0; i < 5; ++i)
	{
		rect[i].r = ran_Color(rd);
		rect[i].g = ran_Color(rd);
		rect[i].b = ran_Color(rd);
		rect[i].drawn = false;
		rect[i].size = 0.1f;
	}
	timer_1 = false;
	timer_2 = false;
	timer_3 = false;
	timer_4 = false;
	timer_5 = false;
}

void draw_Rect()
{
	for (int i = 0; i < 5; ++i)
	{
		if (rect[i].drawn)
		{
			glColor3f(rect[i].r, rect[i].g, rect[i].b);
			glRectf(rect[i].mx - rect[i].size, rect[i].my + rect[i].size, rect[i].mx + rect[i].size, rect[i].my - rect[i].size);
		}
	}
}

void origin_Pos()
{
	for (int i = 0; i < 5; ++i)
	{
		if (rect[i].drawn)
		{
			rect[i].mx = rect[i].ox;
			rect[i].my = rect[i].oy;
		}
	}
}

void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("homework_4");

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
		glutMainLoop();
	}
}

GLvoid drawScene()
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	draw_Rect();
	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

void TimerFunction(int value)
{
	switch (value)
	{
	case 1:
		if (!timer_1) break;
		for (int i = 0; i < 5; ++i)
		{
			if (rect[i].drawn)
			{
				if (rect[i].mx - rect[i].size <= -1.0f)
					rect[i].move_dir_lr = 3;
				if (rect[i].mx + rect[i].size >= 1.0f)
					rect[i].move_dir_lr = 2;

				if (rect[i].my - rect[i].size <= -1.0f)
					rect[i].move_dir_ud = 0;
				if (rect[i].my + rect[i].size >= 1.0f)
					rect[i].move_dir_ud = 1;

				if (rect[i].move_dir_ud == 0) rect[i].my += 0.03f;
				else if (rect[i].move_dir_ud == 1) rect[i].my -= 0.03f;
				if (rect[i].move_dir_lr == 2) rect[i].mx -= 0.03f;
				else if (rect[i].move_dir_lr == 3) rect[i].mx += 0.03f;
			}
		}
		glutPostRedisplay();
		glutTimerFunc(100, TimerFunction, 1);
		break;
	case 2:
		break;
	case 3:
		if (!timer_3) break;
		for (int i = 0; i < 5; ++i)
		{
			if (rect[i].drawn)
			{
				rect[i].size = ran_Size(rd);
			}
		}
		glutPostRedisplay();
		glutTimerFunc(100, TimerFunction, 3);
		break;
	case 4:
		if (!timer_4) break;
		for (int i = 0; i < 5; ++i)
		{
			if (rect[i].drawn)
			{
				rect[i].r = ran_Color(rd);
				rect[i].g = ran_Color(rd);
				rect[i].b = ran_Color(rd);
			}
		}
		glutPostRedisplay();
		glutTimerFunc(100, TimerFunction, 4);
		break;
	case 5:
		break;
	}
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '1':
		if (timer_1) timer_1 = false;
		else
		{
			timer_1 = true;
			for (int i = 0; i < 5; ++i)
			{
				rect[i].move_dir_ud = move_ud(rd);
				rect[i].move_dir_lr = move_lr(rd);
			}
			glutTimerFunc(100, TimerFunction, 1);
		}
		break;
	case '2':
		if (timer_2) timer_2 = false;
		else
		{
			timer_2 = true;
			glutTimerFunc(100, TimerFunction, 2);
		}
		break;
	case '3':
		if (timer_3) timer_3 = false;
		else
		{
			timer_3 = true;
			glutTimerFunc(100, TimerFunction, 3);
		}
		break;
	case '4':
		if (timer_4) timer_4 = false;
		else
		{
			timer_4 = true;
			glutTimerFunc(100, TimerFunction, 4);
		}
		break;
	case '5':
		if (timer_5) timer_5 = false;
		else
		{
			timer_5 = true;
			glutTimerFunc(100, TimerFunction, 5);
		}
		break;
	case 's':
		timer_1 = false;
		timer_2 = false;
		timer_3 = false;
		timer_4 = false;
		timer_5 = false;
		break;
	case 'm':
		origin_Pos();
		break;
	case 'r':
		reset();
		break;
	case 'q':
		glutLeaveMainLoop();
	}
	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y)
{
	float ox = (float)x / (width * 0.5f) - 1.0f;
	float oy = 1.0f - (float)y / (height * 0.5f);
	if ((button == GLUT_LEFT_BUTTON && state == GLUT_DOWN))
	{
		for (int i = 0; i < 5; ++i)
		{
			if (!rect[i].drawn)
			{
				rect[i].ox = ox;
				rect[i].oy = oy;
				rect[i].mx = ox;
				rect[i].my = oy;
				rect[i].drawn = true;
				break;
			}
		}
	}
	glutPostRedisplay();
}
