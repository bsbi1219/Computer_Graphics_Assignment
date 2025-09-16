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
void TimerFunc(int value);

bool timer_Stop = false;

float r = 1.0f, g = 1.0f, b = 1.0f;

void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("homework_1");

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
		glutMainLoop();
	}
}

GLvoid drawScene()
{
	glClearColor(r, g, b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
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
	case 'c':
		r = 0.0f; g = 1.0f; b = 1.0f;
		break;
	case 'm':
		r = 1.0f; g = 0.0f; b = 1.0f;
		break;
	case 'y':
		r = 1.0f; g = 1.0f; b = 0.0f;
		break;
	case 'a':
		r = ran(rd); g = ran(rd); b = ran(rd);
		break;
	case 'w':
		r = 1.0f; g = 1.0f; b = 1.0f;
		break;
	case 'k':
		r = 0.0f; g = 0.0f; b = 0.0f;
		break;
	case 't':
		glutTimerFunc(100, TimerFunc, 1);
		break;
	case 's':
		timer_Stop = true;
		break;
	case 'q':
		glutLeaveMainLoop();
	}
	glutPostRedisplay();
}

void TimerFunc(int value)
{
	r = ran(rd); g = ran(rd); b = ran(rd);
	glutPostRedisplay();
	if (!timer_Stop)
		glutTimerFunc(100, TimerFunc, 1);
	else
		timer_Stop = false;
}

