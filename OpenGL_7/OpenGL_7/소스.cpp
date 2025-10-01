#include <iostream>
#include <random>
#include <math.h>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

using namespace std;

random_device rd;
uniform_real_distribution<float> ran_Color(0.0f, 1.0f);

int width = 1000;
int height = 600;
int selectBlock = -1;
bool timer_on = false;

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);

struct Block
{
	float x1, y1, x2, y2;
	float bx1, by1, bx2, by2;
	float r, g, b;
	bool canMove;
}block[10];

void reset()
{
	for (int i = 0; i < 10; ++i)
	{
		block[i].r = ran_Color(rd);
		block[i].g = ran_Color(rd);
		block[i].b = ran_Color(rd);
		block[i].canMove = true;
	}

	block[0].bx1 = -0.9f; block[0].by1 = 0.8f; block[0].bx2 = -0.7f; block[0].by2 = 0.6f; // 0.2/0.2
	block[1].bx1 = -0.9f; block[1].by1 = 0.6f; block[1].bx2 = -0.5f; block[1].by2 = 0.3f; // 0.4/0.3
	block[2].bx1 = -0.5f; block[2].by1 = 0.9f; block[2].bx2 = -0.4f; block[2].by2 = 0.3f; // 0.1/0.6
	block[3].bx1 = -0.85f; block[3].by1 = 0.3f; block[3].bx2 = -0.2f; block[3].by2 = 0.2f; // 0.65/0.1
	block[4].bx1 = -0.95f; block[4].by1 = 0.2f; block[4].bx2 = -0.8f; block[4].by2 = -0.2f; // 0.15/0.4
	block[5].bx1 = -0.65f; block[5].by1 = 0.2f; block[5].bx2 = -0.5f; block[5].by2 = -0.2f; // 0.15/0.4
	block[6].bx1 = -0.35f; block[6].by1 = 0.2f; block[6].bx2 = -0.2f; block[6].by2 = -0.2f; // 0.15/0.4
	block[7].bx1 = -0.6f; block[7].by1 = -0.2f; block[7].bx2 = -0.3f; block[7].by2 = -0.5f; // 0.3/0.3
	block[8].bx1 = -0.95f; block[8].by1 = -0.2f; block[8].bx2 = -0.9f; block[8].by2 = -0.6f; // 0.05/0.4
	block[9].bx1 = -0.92f; block[9].by1 = -0.6f; block[9].bx2 = -0.1f; block[9].by2 = -0.8f; // 0.82/0.2

	uniform_real_distribution<float> ran_pos(0.0f, 1.0f);
	for (int i = 0; i < 10; ++i)
	{
		float w = block[i].bx2 - block[i].bx1;
		float h = block[i].by2 - block[i].by1;

		// x, y 범위 내에서 블록이 화면을 벗어나지 않게
		float x1 = ran_pos(rd) * (1.0f - w);
		float y1 = -0.5f + ran_pos(rd) * (1.0f - h);
		block[i].x1 = x1;
		block[i].y1 = y1;
		block[i].x2 = x1 + w;
		block[i].y2 = y1 + h;
	}

	timer_on = false;
	selectBlock = -1;
}

void draw_empty_rect(float x1, float y1, float x2, float y2)
{
	glBegin(GL_LINE_LOOP);
	glVertex2f(x1, y1);
	glVertex2f(x2, y1);
	glVertex2f(x2, y2);
	glVertex2f(x1, y2);
	glEnd();
}

void draw_board()
{
	for (int i = 0; i < 10; ++i)
	{
		glColor3f(block[i].r, block[i].g, block[i].b);
		draw_empty_rect(block[i].bx1, block[i].by1, block[i].bx2, block[i].by2);
	}
}

void draw_block()
{
	for (int i = 0; i < 10; ++i)
	{
		glColor3f(block[i].r, block[i].g, block[i].b);
		glRectf(block[i].x1, block[i].y1, block[i].x2, block[i].y2);
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
		glutMotionFunc(Motion);
		glutMainLoop();
	}
}

GLvoid drawScene()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	draw_board();
	draw_block();
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
		if (timer_on)
		{
			const float eps = 0.001f;
			for (int i = 0; i < 10; ++i)
			{
				if (block[i].canMove)
				{
					float dx = (block[i].bx1 - block[i].x1);
					float dy = (block[i].by1 - block[i].y1);
					float dist = sqrt(dx * dx + dy * dy);
					if (dist < eps)
					{
						block[i].x1 = block[i].bx1;
						block[i].y1 = block[i].by1;
						block[i].x2 = block[i].bx2;
						block[i].y2 = block[i].by2;
						block[i].canMove = false;
						cout << "블록 맞춰짐" << endl;
						break;
					}
					dx = (dx / dist) * 0.001f;
					dy = (dy / dist) * 0.001f;
					block[i].x1 += dx;
					block[i].y1 += dy;
					block[i].x2 += dx;
					block[i].y2 += dy;
					break;
				}
			}
			glutPostRedisplay();
			glutTimerFunc(1, TimerFunction, 1);
		}
	}
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'r':
		reset();
		break;
	case 'h':
		if (!timer_on)
		{
			glutTimerFunc(1, TimerFunction, 1);
			timer_on = true;
		}
		else
		{
			timer_on = false;
		}
		break;
	case 'q':
		glutLeaveMainLoop();
		break;
	}
	glutPostRedisplay();
}

void Mouse(int button, int state, int x, int y)
{
	if ((button == GLUT_LEFT_BUTTON && state == GLUT_DOWN))
	{
		float ox = (float)x / (width * 0.5f) - 1.0f;
		float oy = 1.0f - (float)y / (height * 0.5f);
		cout << "마우스 좌표 : " << ox << ", " << oy << endl;
		for (int i = 9; i >= 0; --i)
		{
			if (ox >= block[i].x1 && ox <= block[i].x2 && oy <= block[i].y1 && oy >= block[i].y2 && block[i].canMove)
			{
				selectBlock = i;
				cout << "선택 블록 : " << i << endl;
				break;
			}
		}
	}
	if ((button == GLUT_LEFT_BUTTON && state == GLUT_UP))
	{
		if (selectBlock != -1)
		{
			if ((block[selectBlock].x1 - block[selectBlock].bx1 <= 0.04f && block[selectBlock].x1 >= block[selectBlock].bx1) || 
				(block[selectBlock].bx1 - block[selectBlock].x1 <= 0.04f && block[selectBlock].x1 <= block[selectBlock].bx1))
			{
				block[selectBlock].x1 = block[selectBlock].bx1;
				block[selectBlock].y1 = block[selectBlock].by1;
				block[selectBlock].x2 = block[selectBlock].bx2;
				block[selectBlock].y2 = block[selectBlock].by2;
				block[selectBlock].canMove = false;
				cout << "블록 맞춰짐" << endl;
			}
		}
		selectBlock = -1;
		cout << "선택 블록 해제" << endl;
	}
	glutPostRedisplay();
}

void Motion(int x, int y)
{
	if (selectBlock != -1)
	{
		float ox = (float)x / (width * 0.5f) - 1.0f;
		float oy = 1.0f - (float)y / (height * 0.5f);
		float w = block[selectBlock].x2 - block[selectBlock].x1;
		float h = block[selectBlock].y1 - block[selectBlock].y2;

		// 중심을 마우스 위치로 이동
		block[selectBlock].x1 = ox - w / 2.0f;
		block[selectBlock].x2 = ox + w / 2.0f;
		block[selectBlock].y1 = oy + h / 2.0f;
		block[selectBlock].y2 = oy - h / 2.0f;
		glutPostRedisplay();
	}
}