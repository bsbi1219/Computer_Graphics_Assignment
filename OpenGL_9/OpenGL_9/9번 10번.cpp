#define _CRT_SECURE_NO_WARNINGS //--- 프로그램 맨 앞에 선언할 것
#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <iostream>
#include <random>
#include <math.h>

using namespace std;

void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y); 
void TimerFunction(int value);
char* filetobuf(const char* file); //--- 텍스트 파일을 읽어서 문자열로 반환하는 함수

random_device rd;
uniform_real_distribution<float> ranColor(0.0f, 1.0f);
uniform_real_distribution<float> ran_Size(0.0f, 0.2f);
uniform_int_distribution<int> ran_lr(0, 1); // 0: left, 1: right
uniform_int_distribution<int> ran_ud(2, 3); // 2: up 3: down

GLint width = 500, height = 500;
GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader; //--- 프래그먼트 세이더 객체

GLuint VAO;
GLuint VBO;

int triCnt[4];
bool line = false;
bool start = true;

int moving = 0;

struct TRIANGLE
{
	float vx[3], vy[3];
	float r, g, b;
	float loc;
	int vCnt;
	float dir_lr, dir_ud;
	GLuint VAO, VBO;
}tri[16];

void reset()
{
	for (int i = 0; i < 4; ++i)
	{
		triCnt[i] = 0;
	}
	for (int i = 0; i < 16; ++i)
	{
		tri[i].loc = 0;
		tri[i].dir_lr = ran_lr(rd);
		tri[i].dir_ud = ran_ud(rd);
	}
}

void resetLine()
{
	if (start)
	{
		float vertex[] =
		{
			-1.0f, 0.0f, 0.0f,
			 1.0f, 0.0f, 0.0f,
			 0.0f, 1.0f, 0.0f,
			 0.0f, -1.0f, 0.0f
		};
		int vCnt = 4;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		start = false;
	}
}

void main(int argc, char** argv) //--- 윈도우 출력하고 콜백함수 설정
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // 화면 모드
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("Example1");
	glewExperimental = GL_TRUE;
	glewInit();
	make_vertexShaders();
	make_fragmentShaders();
	shaderProgramID = make_shaderProgram();
	reset();
	resetLine();
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMainLoop();
}

GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
{
	GLfloat rColor, gColor, bColor;
	rColor = gColor = bColor = 0.0f; // 배경색
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	glPointSize(5.0);
	glBindVertexArray(VAO);
	GLint loc = glGetUniformLocation(shaderProgramID, "u_color");
	glUniform3f(loc, 1.0f, 1.0f, 1.0f);
	glDrawArrays(GL_LINES, 0, 4);
	for (int i = 0; i < 16; ++i)
	{
		if (tri[i].loc != -1)
		{
			glBindVertexArray(tri[i].VAO);

			GLint loc = glGetUniformLocation(shaderProgramID, "u_color");
			glUniform3f(loc, tri[i].r, tri[i].g, tri[i].b);

			if (!line)
				glDrawArrays(GL_TRIANGLES, 0, tri[i].vCnt);
			else
				glDrawArrays(GL_LINE_LOOP, 0, tri[i].vCnt);
		}
	}
	glBindVertexArray(0);
	glutSwapBuffers(); // 화면에 출력하기
}
//--- 다시그리기 콜백 함수
GLvoid Reshape(int w, int h) //--- 콜백 함수: 다시 그리기 콜백 함수
{
	glViewport(0, 0, w, h);
}

void make_vertexShaders()
{
	GLchar* vertexSource;
	//--- 버텍스 세이더 읽어 저장하고 컴파일 하기
	//--- filetobuf: 사용자정의 함수로 텍스트를 읽어서 문자열에 저장하는 함수
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
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

void make_fragmentShaders()
{
	GLchar* fragmentSource;
	//--- 프래그먼트 세이더 읽어 저장하고 컴파일하기
	fragmentSource = filetobuf("fragment.glsl"); // 프래그세이더 읽어오기
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: frag_shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

int returnIndex(int loc)
{
	int idx = -1;
	if (loc == 1) idx = 0 + triCnt[0];
	else if (loc == 2) idx = 4 + triCnt[1];
	else if (loc == 3) idx = 8 + triCnt[2];
	else if (loc == 4) idx = 12 + triCnt[3];

	return idx;
}

void createShape(float x, float y, int loc)
{
	float vertices[9];

	float size_x = ran_Size(rd);
	float size_y = ran_Size(rd);

	float temp[] =
	{
		x, y + size_y, 0.0f,
		x - size_x, y - size_y, 0.0f,
		x + size_x, y - size_y, 0.0f
	};
	memcpy(vertices, temp, sizeof(temp));
	int vCnt = 3;

	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vCnt * 3, vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	int idx = returnIndex(loc);
	if (idx < 0 || idx >= 16) return;

	tri[idx].vx[0] = vertices[0];
	tri[idx].vx[1] = vertices[3];
	tri[idx].vx[2] = vertices[6];
	tri[idx].vy[0] = vertices[1];
	tri[idx].vy[1] = vertices[4];
	tri[idx].vy[2] = vertices[7];
	tri[idx].r = ranColor(rd);
	tri[idx].g = ranColor(rd);
	tri[idx].b = ranColor(rd);
	tri[idx].loc = loc;
	tri[idx].vCnt = vCnt;
	tri[idx].VAO = VAO;
	tri[idx].VBO = VBO;
	idx++;
	triCnt[loc - 1]++;
}

GLuint make_shaderProgram()
{
	GLint result;
	GLchar* errorLog = NULL;
	GLuint shaderID;
	shaderID = glCreateProgram(); //--- 세이더 프로그램 만들기
	glAttachShader(shaderID, vertexShader); //--- 세이더 프로그램에 버텍스 세이더 붙이기
	glAttachShader(shaderID, fragmentShader); //--- 세이더 프로그램에 프래그먼트 세이더 붙이기
	glLinkProgram(shaderID); //--- 세이더 프로그램 링크하기
	glDeleteShader(vertexShader); //--- 세이더 객체를 세이더 프로그램에 링크했음으로, 세이더 객체 자체는 삭제 가능
	glDeleteShader(fragmentShader);
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result); // ---세이더가 잘 연결되었는지 체크하기
	if (!result) {
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
		return false;
	}
	glUseProgram(shaderID); //--- 만들어진 세이더 프로그램 사용하기
	//--- 여러 개의 세이더프로그램 만들 수 있고, 그 중 한개의 프로그램을 사용하려면
	//--- glUseProgram 함수를 호출하여 사용 할 특정 프로그램을 지정한다.
	//--- 사용하기 직전에 호출할 수 있다.
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

void moveShape(unsigned char key)
{
	if (key == '1' && moving != 1)
	{
		moving = 1;
		glutTimerFunc(10, TimerFunction, 1);
	}
	else if (key == '2' && moving != 2)
	{
		glutTimerFunc(10, TimerFunction, 2);
	}
	else if (key == '3' && moving != 3)
	{
		glutTimerFunc(10, TimerFunction, 3);
	}
	else if (key == '4' && moving != 4)
	{
		glutTimerFunc(10, TimerFunction, 4);
	}
	else if (moving != 0) moving = 0;
	else return;
}

void Bounce_tri(float speed)
{
	for (int i = 0; i < 16; ++i)
	{
		if (tri[i].loc != -1)
		{
			if (tri[i].vy[0] > 1.0f) tri[i].dir_ud = 3;
			if (tri[i].vy[1] < -1.0f || tri[i].vy[2] < -1.0f) tri[i].dir_ud = 2;
			if (tri[i].vx[1] < -1.0f) tri[i].dir_lr = 1;
			if (tri[i].vx[2] > 1.0f) tri[i].dir_lr = 0;

			if (tri[i].dir_lr == 0)
			{
				for (int j = 0; j < 3; ++j)
				{
					tri[i].vx[j] -= speed;
				}
			}
			else
			{
				for (int j = 0; j < 3; ++j)
				{
					tri[i].vx[j] += speed;
				}
			}

			if (tri[i].dir_ud == 2)
			{
				for (int j = 0; j < 3; ++j)
				{
					tri[i].vy[j] += speed;
				}
			}
			else
			{
				for (int j = 0; j < 3; ++j)
				{
					tri[i].vy[j] -= speed;
				}
			}
		}

		GLuint VAO, VBO, EBO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		float vertices[] =
		{
			tri[i].vx[0], tri[i].vy[0], 0.0f,
			tri[i].vx[1], tri[i].vy[1], 0.0f,
			tri[i].vx[2], tri[i].vy[2], 0.0f
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * tri[i].vCnt * 3, vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		tri[i].VAO = VAO;
		tri[i].VBO = VBO;
	}
}

void Zigzag_tri()
{

}

void Rect_Spiral()
{

}

void Cir_Spiral()
{

}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'a':
		line = false;
		break;
	case 'b':
		line = true;
		break;
	case 'c':
		for (int i = 0; i < 16; ++i)
		{
			tri[i].loc = -1;
		}
		for (int i = 0; i < 4; ++i)
		{
			triCnt[i] = 0;
		}
		break;
	case '1': case '2': case '3': case '4':
		moveShape(key);
		break;
	case 'q':
		exit(0);
		break;
	}
	glutPostRedisplay();
}

void TimerFunction(int value)
{
	float speed = 0.01f;
	switch (value)
	{
	case 1:
		if (moving == 1)
		{
			Bounce_tri(speed);
			glutPostRedisplay();
			glutTimerFunc(10, TimerFunction, 1);
		}
		break;
	case 2:
		moving = 2;
		break;
	case 3:
		moving = 3;
		break;
	case 4:
		moving = 4;
		break;
	}
}

int findLoc(float ox, float oy)
{
	if (ox > 0.0f && ox < width && oy > 0.0f && oy < height) // 1사분면
	{
		return 1;
	}
	else if (ox < 0.0f && ox > -1.0f && oy > 0.0f && oy < height) // 2사분면
	{
		return 2;
	}
	else if (ox < 0.0f && ox > -1.0f && oy < 0.0f && oy > -1.0f) // 3사분면
	{
		return 3;
	}
	else if (ox > 0.0f && ox < width && oy < 0.0f && oy > -1.0f) // 4사분면
	{
		return 4;
	}
}

void Mouse(int button, int state, int x, int y)
{
	float ox = (float)x / width * 2.0f - 1.0f;
	float oy = 1.0f - (float)y / height * 2.0f;
	if ((button == GLUT_LEFT_BUTTON && state == GLUT_DOWN))
	{
		if (triCnt[0] + triCnt[1] + triCnt[2] + triCnt[3] >= 16) return;
		int loc = findLoc(ox, oy);
		cout << loc << endl;
		for (int i = 0; i < 4; ++i)
		{
			if (loc == i + 1 && triCnt[i] > 0)
			{
				for (int j = 0; j < 4; ++j)
				{
					tri[i * 4 + j].loc = -1;
					triCnt[i] = 0;
				}
			}
		}
		createShape(ox, oy, loc);
	}
	if ((button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN))
	{
		int loc = findLoc(ox, oy);
		for (int i = 0; i < 4; ++i)
		{
			if (loc == i + 1 && triCnt[i] < 4)
			{
				createShape(ox, oy, loc);
			}
		}
	}
	glutPostRedisplay();
}