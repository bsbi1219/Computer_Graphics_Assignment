#include <iostream>
#include <random>
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <vector>

using namespace std;

random_device rd;
uniform_real_distribution<float> ran_Color(0.0f, 1.0f);
uniform_real_distribution<float> ran_Size(0.05f, 0.2f);
uniform_real_distribution<float> ran_Speed(0.01f, 0.03f);
uniform_int_distribution<int> move_ud(0, 1);
uniform_int_distribution<int> move_lr(2, 3);
uniform_int_distribution<int> ran_Rect(0, 4);

int width = 600;
int height = 600;
int ranRect = -1;

bool timer_1 = false;
bool timer_2 = false;
bool timer_3 = false;
bool timer_4 = false;
bool timer_5 = false;

vector<pair<float, float>> pathBuffer;
const int followDelay = 20;

bool ranRect_wall = false;

int followIndex[5] = { -1, -1, -1, -1, -1 };

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
	float speed;
	bool drawn;
	int move_dir_ud; // 0 : 위, 1 : 아래, 2 : 왼쪽, 3 : 오른쪽
	int move_dir_lr;
}rect[5];

void random_Speed()
{
	for (int i = 0; i < 5; ++i)
	{
		rect[i].speed = ran_Speed(rd);
	}
}

float lerp(float a, float b, float t) {
	return a + (b - a) * t;
}

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

	for (int i = 0; i < 5; ++i) followIndex[i] = -1;
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
				{
					rect[i].move_dir_lr = 3;
				}
				if (rect[i].mx + rect[i].size >= 1.0f)
				{
					rect[i].move_dir_lr = 2;
				}

				if (rect[i].my - rect[i].size <= -1.0f)
				{
					rect[i].move_dir_ud = 0;
				}
				if (rect[i].my + rect[i].size >= 1.0f)
				{
					rect[i].move_dir_ud = 1;
				}

				if (rect[i].move_dir_ud == 0) rect[i].my += rect[i].speed;
				else if (rect[i].move_dir_ud == 1) rect[i].my -= rect[i].speed;
				if (rect[i].move_dir_lr == 2) rect[i].mx -= rect[i].speed;
				else if (rect[i].move_dir_lr == 3) rect[i].mx += rect[i].speed;

				if (ranRect >= 0 && ranRect < 5 && rect[ranRect].drawn && i == ranRect) {
					pathBuffer.push_back({ rect[ranRect].mx, rect[ranRect].my });
					int maxHistory = followDelay * 4 + 1;
					if (pathBuffer.size() > maxHistory)
						pathBuffer.erase(pathBuffer.begin(), pathBuffer.begin() + (pathBuffer.size() - maxHistory));
				}

			}
		}
		glutPostRedisplay();
		glutTimerFunc(10, TimerFunction, 1);
		break;
	case 2:
		if (!timer_2) break;
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

				if (rect[i].move_dir_lr == 2) rect[i].mx -= rect[i].speed;
				else if (rect[i].move_dir_lr == 3) rect[i].mx += rect[i].speed;
				if (rect[i].move_dir_ud == 0) rect[i].my += 0.001f;
				else if (rect[i].move_dir_ud == 1) rect[i].my -= 0.001f;

				if (ranRect >= 0 && ranRect < 5 && rect[ranRect].drawn && i == ranRect) {
					pathBuffer.push_back({ rect[ranRect].mx, rect[ranRect].my });
					int maxHistory = followDelay * 4 + 1;
					if (pathBuffer.size() > maxHistory)
						pathBuffer.erase(pathBuffer.begin(), pathBuffer.begin() + (pathBuffer.size() - maxHistory));
				}
			}
		}
		glutPostRedisplay();
		glutTimerFunc(10, TimerFunction, 2);
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
		glutTimerFunc(10, TimerFunction, 4);
		break;
	case 5:
		if (!timer_5) break;
		if (ranRect < 0 || ranRect > 4) break;

		// 1. ranRect만 계속 움직이게
		if (rect[ranRect].drawn) {
			// 벽 충돌 처리
			if (rect[ranRect].mx - rect[ranRect].size <= -1.0f)
				rect[ranRect].move_dir_lr = 3;
			if (rect[ranRect].mx + rect[ranRect].size >= 1.0f)
				rect[ranRect].move_dir_lr = 2;
			if (rect[ranRect].my - rect[ranRect].size <= -1.0f)
				rect[ranRect].move_dir_ud = 0;
			if (rect[ranRect].my + rect[ranRect].size >= 1.0f)
				rect[ranRect].move_dir_ud = 1;

			// 이동
			if (rect[ranRect].move_dir_ud == 0) rect[ranRect].my += rect[ranRect].speed;
			else if (rect[ranRect].move_dir_ud == 1) rect[ranRect].my -= rect[ranRect].speed;
			if (rect[ranRect].move_dir_lr == 2) rect[ranRect].mx -= rect[ranRect].speed;
			else if (rect[ranRect].move_dir_lr == 3) rect[ranRect].mx += rect[ranRect].speed;

			// 경로 기록
			pathBuffer.push_back({ rect[ranRect].mx, rect[ranRect].my });
			int maxHistory = followDelay * 4 + 1;
			if (pathBuffer.size() > maxHistory)
				pathBuffer.erase(pathBuffer.begin(), pathBuffer.begin() + (pathBuffer.size() - maxHistory));
		}

		// 2. 나머지 사각형은 pathBuffer를 따라감
		int pathReady = (int)pathBuffer.size() - followDelay * 4 - 1;
		for (int i = 0; i < 5; ++i)
		{
			if (i == ranRect) continue;
			if (pathReady < 0) continue;

			if (followIndex[i] == -1) {
				int idx = (int)pathBuffer.size() - followDelay * i - 1;
				if (idx >= 0) followIndex[i] = idx;
				else continue;
			}
			if (followIndex[i] < 0 || followIndex[i] >= (int)pathBuffer.size()) continue;

			float target_x = pathBuffer[followIndex[i]].first;
			float target_y = pathBuffer[followIndex[i]].second;

			rect[i].mx = lerp(rect[i].mx, target_x, 0.2f);
			rect[i].my = lerp(rect[i].my, target_y, 0.2f);

			float dist = sqrt((rect[i].mx - target_x) * (rect[i].mx - target_x) + (rect[i].my - target_y) * (rect[i].my - target_y));
			if (dist < 0.01f && followIndex[i] < (int)pathBuffer.size() - 1)
				followIndex[i]++;
		}
		glutPostRedisplay();
		glutTimerFunc(10, TimerFunction, 5);
		break;
	}
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '1':
		if (timer_1 || timer_2) timer_1 = false;
		else
		{
			timer_1 = true;
			for (int i = 0; i < 5; ++i)
			{
				rect[i].move_dir_ud = move_ud(rd);
				rect[i].move_dir_lr = move_lr(rd);
			}
			random_Speed();
			glutTimerFunc(10, TimerFunction, 1);
		}
		break;
	case '2':
		if (timer_2 || timer_1) timer_2 = false;
		else
		{
			timer_2 = true;
			for (int i = 0; i < 5; ++i)
			{
				rect[i].move_dir_ud = move_ud(rd);
				rect[i].move_dir_lr = move_lr(rd);
			}
			random_Speed();
			glutTimerFunc(10, TimerFunction, 2);
		}
		break;
	case '3':
		if (timer_3) timer_3 = false;
		else
		{
			timer_3 = true;
			glutTimerFunc(10, TimerFunction, 3);
		}
		break;
	case '4':
		if (timer_4) timer_4 = false;
		else
		{
			timer_4 = true;
			glutTimerFunc(10, TimerFunction, 4);
		}
		break;
	case '5':
		// 2번, 1번 타이머 모두 정지
		timer_1 = false;
		timer_2 = false;
		// 따라가기 상태 초기화
		timer_5 = !timer_5;
		if (timer_5) {
			ranRect = ran_Rect(rd);
			pathBuffer.clear();
			for (int i = 0; i < 5; ++i) followIndex[i] = -1;
			glutTimerFunc(10, TimerFunction, 5);
		}
		else {
			ranRect = -1;
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
