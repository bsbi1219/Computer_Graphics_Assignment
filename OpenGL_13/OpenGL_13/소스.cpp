#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <iostream>
#include <random>
#include <vector>

using namespace std;

void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void TimerFunction(int value);
char* filetobuf(const char* file);

random_device rd;
uniform_real_distribution<float> color(0.0f, 1.0f);
uniform_real_distribution<float> ranPos(-0.9f, 0.9f);
uniform_int_distribution<int> ranDir(0, 3); // �����¿�
uniform_int_distribution<int> ranMove(0, 1); // ƨ���, �������

GLint width = 800, height = 800;
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;

GLuint VAO, VBO;

int selectedShape = -1;

struct SHAPE
{
	int vertexNum; // ������ ����
	GLuint VAO, VBO;
	bool isPlus; // ���������� ����
	bool isMove; // �����̴��� ����
	bool isLook; // ���̴��� ����
	vector<float> vertices; // ������ ��ǥ
	float r, g, b;
	float vx, vy; // �߽� ��ǥ
	float size; // ũ��
	int dir; // �����¿�
	int moveType; // 0: ƨ���, 1: �������
}s[15];

void reset()
{
	for (int i = 0; i < 15; ++i)
	{
		s[i].isPlus = false;
		s[i].isMove = true;
		s[i].isLook = true;
		s[i].r = color(rd);
		s[i].g = color(rd);
		s[i].b = color(rd);
		s[i].vx = ranPos(rd);
		s[i].vy = ranPos(rd);
		s[i].dir = ranDir(rd);
		s[i].moveType = ranMove(rd);

		if (i < 3)
		{
			s[i].vertices =
			{
				s[i].vx - 0.01f, s[i].vy + 0.01f, 0.0f,
				s[i].vx - 0.01f, s[i].vy - 0.01f, 0.0f,
				s[i].vx + 0.01f, s[i].vy - 0.01f, 0.0f,

				s[i].vx - 0.01f, s[i].vy + 0.01f, 0.0f,
				s[i].vx + 0.01f, s[i].vy - 0.01f, 0.0f,
				s[i].vx + 0.01f, s[i].vy + 0.01f, 0.0f
			};
			s[i].vertexNum = 1;
			s[i].size = 0.01f;
		}
		else if (i >= 3 && i < 6)
		{
			s[i].vertices =
			{
				s[i].vx, s[i].vy + 0.05f, 0.0f,
				s[i].vx - 0.05f, s[i].vy - 0.03f, 0.0f,
				s[i].vx + 0.05f, s[i].vy - 0.03f, 0.0f,
			};
			s[i].vertexNum = 3;
			s[i].size = 0.05f;
		}
		else if (i >= 6 && i < 9)
		{
			s[i].vertices =
			{
				s[i].vx - 0.05f, s[i].vy + 0.05f, 0.0f,
				s[i].vx - 0.05f, s[i].vy - 0.05f, 0.0f,
				s[i].vx + 0.05f, s[i].vy - 0.05f, 0.0f,

				s[i].vx - 0.05f, s[i].vy + 0.05f, 0.0f,
				s[i].vx + 0.05f, s[i].vy - 0.05f, 0.0f,
				s[i].vx + 0.05f, s[i].vy + 0.05f, 0.0f
			};
			s[i].vertexNum = 4;
			s[i].size = 0.05f;
		}
		else if (i >= 9 && i < 12)
		{
			s[i].vertices =
			{
				s[i].vx, s[i].vy + 0.05f, 0.0f,
				s[i].vx - 0.06f, s[i].vy, 0.0f,
				s[i].vx + 0.06f, s[i].vy, 0.0f,

				s[i].vx - 0.06f, s[i].vy, 0.0f,
				s[i].vx - 0.04f, s[i].vy - 0.065f, 0.0f,
				s[i].vx + 0.04f, s[i].vy - 0.065f, 0.0f,

				s[i].vx - 0.06f, s[i].vy, 0.0f,
				s[i].vx + 0.04f, s[i].vy - 0.065f, 0.0f,
				s[i].vx + 0.06f, s[i].vy, 0.0f
			};
			s[i].vertexNum = 5;
			s[i].size = 0.065f;
		}
		else
		{
			s[i].vertices =
			{
				s[i].vx, s[i].vy, 0.0f,
				s[i].vx + 0.1f, s[i].vy + 0.1f, 0.0f,
			};	
			s[i].vertexNum = 2;
			s[i].size = 0.1f;
		}
		s[i].VAO = VAO;
		s[i].VBO = VBO;

		glGenVertexArrays(1, &s[i].VAO);
		glGenBuffers(1, &s[i].VBO);

		glBindVertexArray(s[i].VAO);
		glBindBuffer(GL_ARRAY_BUFFER, s[i].VBO);
		glBufferData(GL_ARRAY_BUFFER, s[i].vertices.size() * sizeof(float), s[i].vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);
	}
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
	reset();
	make_vertexShaders();
	make_fragmentShaders();
	shaderProgramID = make_shaderProgram();
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutMainLoop();
}

GLvoid drawScene()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	glPointSize(2.0);
	GLint loc = glGetUniformLocation(shaderProgramID, "u_color");
	for (int i = 0; i < 15; ++i)
	{
		if (s[i].isLook)
		{
			glUniform3f(loc, s[i].r, s[i].g, s[i].b);
			glBindVertexArray(s[i].VAO);
			if (s[i].vertexNum != 2)
				glDrawArrays(GL_TRIANGLES, 0, s[i].vertices.size());
			else
				glDrawArrays(GL_LINES, 0, s[i].vertices.size());
		}
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
	case 'c':
		reset();
		break;
	case 's':
		for (int i = 0; i < 15; ++i)
		{
			if (s[i].isLook && s[i].isPlus)
			{
				if (s[i].isMove) s[i].isMove = false;
				else s[i].isMove = true;
			}
		}
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
		for (int i = 0; i < 15; ++i)
		{
			if (s[i].isMove && s[i].isLook && s[i].isPlus)
			{
				if (s[i].moveType == 0) // ƨ���
				{
					switch (s[i].dir)
					{
					case 0: // ��
						s[i].vy += 0.01f;
						if (s[i].vy + s[i].size >= 1.0f) s[i].dir = 1;
						break;
					case 1: // ��
						s[i].vy -= 0.01f;
						if (s[i].vy - s[i].size <= -1.0f) s[i].dir = 0;
						break;
					case 2: // ��
						s[i].vx -= 0.01f;
						if (s[i].vx - s[i].size <= -1.0f) s[i].dir = 3;
						break;
					case 3: // ��
						s[i].vx += 0.01f;
						if (s[i].vx + s[i].size >= 1.0f) s[i].dir = 2;
						break;
					}
				}
				else // �������
				{
					switch (s[i].dir)
					{
					case 0: // ��
						s[i].vy += 0.01f;
						s[i].vx += 0.005f;
						if (s[i].vy + s[i].size >= 1.0f) s[i].dir = 1;
						if (s[i].vx + s[i].size >= 1.0f) s[i].vx = -1.0f + s[i].size;
						break;
					case 1: // ��
						s[i].vy -= 0.01f;
						s[i].vx -= 0.005f;
						if (s[i].vy - s[i].size <= -1.0f) s[i].dir = 0;
						if (s[i].vx - s[i].size <= -1.0f) s[i].vx = 1.0f - s[i].size;
						break;
					case 2: // ��
						s[i].vx -= 0.01f;
						s[i].vy += 0.005f;
						if (s[i].vx - s[i].size <= -1.0f) s[i].dir = 3;
						if (s[i].vy + s[i].size >= 1.0f) s[i].vy = -1.0f + s[i].size;
						break;
					case 3: // ��
						s[i].vx += 0.01f;
						s[i].vy -= 0.005f;
						if (s[i].vx + s[i].size >= 1.0f) s[i].dir = 2;
						if (s[i].vy - s[i].size <= -1.0f) s[i].vy = 1.0f - s[i].size;
						break;
					}
				}
			}
		}
		break;
	}
}

int isOn(int ss)
{
	for (int i = 0; i < 15; ++i)
	{
		if (s[i].isPlus) continue;
		if (!s[i].isLook) continue;
		if (i == ss) continue;

		float dist = sqrt((s[ss].vx - s[i].vx) * (s[ss].vx - s[i].vx) + (s[ss].vy - s[i].vy) * (s[ss].vy - s[i].vy));
		if (dist <= s[ss].size + s[i].size) return i;
	}
	return -1;
}

void makeVertex(int num, int shapeNum)
{
	if (num == 1)
	{
		s[shapeNum].vertices =
		{
			s[shapeNum].vx - 0.01f, s[shapeNum].vy + 0.01f, 0.0f,
			s[shapeNum].vx - 0.01f, s[shapeNum].vy - 0.01f, 0.0f,
			s[shapeNum].vx + 0.01f, s[shapeNum].vy - 0.01f, 0.0f,

			s[shapeNum].vx - 0.01f, s[shapeNum].vy + 0.01f, 0.0f,
			s[shapeNum].vx + 0.01f, s[shapeNum].vy - 0.01f, 0.0f,
			s[shapeNum].vx + 0.01f, s[shapeNum].vy + 0.01f, 0.0f
		};
		s[shapeNum].size = 0.01f;
	}
	else if (num == 2)
	{
		s[shapeNum].vertices =
		{
			s[shapeNum].vx, s[shapeNum].vy, 0.0f,
			s[shapeNum].vx + 1.0f, s[shapeNum].vy + 1.0f, 0.0f,
		};
		s[shapeNum].size = 0.1f;
	}
	else if (num == 3)
	{
		s[shapeNum].vertices =
		{
			s[shapeNum].vx, s[shapeNum].vy + 0.05f, 0.0f,
			s[shapeNum].vx - 0.05f, s[shapeNum].vy - 0.03f, 0.0f,
			s[shapeNum].vx + 0.05f, s[shapeNum].vy - 0.03f, 0.0f,
		};
		s[shapeNum].size = 0.05f;
	}
	else if (num == 4)
	{
		s[shapeNum].vertices =
		{
			s[shapeNum].vx - 0.05f, s[shapeNum].vy + 0.05f, 0.0f,
			s[shapeNum].vx - 0.05f, s[shapeNum].vy - 0.05f, 0.0f,
			s[shapeNum].vx + 0.05f, s[shapeNum].vy - 0.05f, 0.0f,

			s[shapeNum].vx - 0.05f, s[shapeNum].vy + 0.05f, 0.0f,
			s[shapeNum].vx + 0.05f, s[shapeNum].vy - 0.05f, 0.0f,
			s[shapeNum].vx + 0.05f, s[shapeNum].vy + 0.05f, 0.0f
		};
		s[shapeNum].size = 0.05f;
	}
	else
	{
		s[shapeNum].vertices =
		{
			s[shapeNum].vx, s[shapeNum].vy + 0.05f, 0.0f,
			s[shapeNum].vx - 0.06f, s[shapeNum].vy, 0.0f,
			s[shapeNum].vx + 0.06f, s[shapeNum].vy, 0.0f,

			s[shapeNum].vx - 0.06f, s[shapeNum].vy, 0.0f,
			s[shapeNum].vx - 0.04f, s[shapeNum].vy - 0.065f, 0.0f,
			s[shapeNum].vx + 0.04f, s[shapeNum].vy - 0.065f, 0.0f,

			s[shapeNum].vx - 0.06f, s[shapeNum].vy, 0.0f,
			s[shapeNum].vx + 0.04f, s[shapeNum].vy - 0.065f, 0.0f,
			s[shapeNum].vx + 0.06f, s[shapeNum].vy, 0.0f
		};
		s[shapeNum].size = 0.065f;
	}
	glBindVertexArray(s[shapeNum].VAO);
	glBindBuffer(GL_ARRAY_BUFFER, s[shapeNum].VBO);
	glBufferData(GL_ARRAY_BUFFER, s[shapeNum].vertices.size() * sizeof(float), s[shapeNum].vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}

void Mouse(int button, int state, int x, int y)
{
	float ox = (float)x / width * 2.0f - 1.0f;
	float oy = 1.0f - (float)y / height * 2.0f;
	if ((button == GLUT_LEFT_BUTTON && state == GLUT_DOWN))
	{
		for (int i = 0; i < 15; ++i)
		{
			if (!s[i].isLook || s[i].isPlus) continue;
			if (i < 3 && ox >= s[i].vx - 0.01f && ox <= s[i].vx + 0.01f && oy >= s[i].vy - 0.01f && oy <= s[i].vy + 0.01f)
			{
				selectedShape = i;
				cout << selectedShape << "�� �� ���õ�" << endl;
				break;
			}
			else if (i >= 3 && i < 6 && ox >= s[i].vx - 0.05f && ox <= s[i].vx + 0.05f && oy >= s[i].vy - 0.03f && oy <= s[i].vy + 0.05f)
			{
				selectedShape = i;
				cout << selectedShape << "�� �ﰢ�� ���õ�" << endl;
				break;
			}
			else if (i >= 6 && i < 9 && ox >= s[i].vx - 0.05f && ox <= s[i].vx + 0.05f && oy >= s[i].vy - 0.05f && oy <= s[i].vy + 0.05f)
			{
				selectedShape = i;
				cout << selectedShape << "�� �簢�� ���õ�" << endl;
				break;
			}
			else if (i >= 9 && i < 12 && ox >= s[i].vx - 0.06f && ox <= s[i].vx + 0.06f && oy >= s[i].vy - 0.065f && oy <= s[i].vy + 0.05f)
			{
				selectedShape = i;
				cout << selectedShape << "�� ������ ���õ�" << endl;
				break;
			}
			else if (i >= 12 && ox >= s[i].vx && ox <= s[i].vx + 0.1f && s[i].vy >= s[i].vy && s[i].vy <= s[i].vy + 0.1f)
			{
				selectedShape = i;
				cout << selectedShape << "�� �� ���õ�" << endl;
				break;
			}
			else selectedShape = -1;
		}
	}
	if ((button == GLUT_LEFT_BUTTON && state == GLUT_UP))
	{
		int onShape = isOn(selectedShape);
		cout << onShape << "���� �浹" << endl;
		if (onShape != -1 && selectedShape != -1)
		{
			s[onShape].isMove = true;
			s[onShape].isPlus = true;
			s[selectedShape].isPlus = true;
			s[selectedShape].isLook = false;

			int PlusNum = s[onShape].vertexNum + s[selectedShape].vertexNum;
			s[onShape].vertexNum = (PlusNum > 5) ? 1 : PlusNum;
			makeVertex(s[onShape].vertexNum, onShape);
		}
		selectedShape = -1;
	}
	glutPostRedisplay();
}

void Motion(int x, int y)
{
	if (selectedShape != -1)
	{
		float ox = (float)x / (width * 0.5f) - 1.0f;
		float oy = 1.0f - (float)y / (height * 0.5f);

		float a = ox - s[selectedShape].vx;
		float b = oy - s[selectedShape].vy;

		s[selectedShape].vx = ox;
		s[selectedShape].vy = oy;

		for (size_t i = 0; i < s[selectedShape].vertices.size(); i += 3)
		{
			s[selectedShape].vertices[i] += a;
			s[selectedShape].vertices[i + 1] += b;
		}

		glBindVertexArray(s[selectedShape].VAO);
		glBindBuffer(GL_ARRAY_BUFFER, s[selectedShape].VBO);
		glBufferData(GL_ARRAY_BUFFER, s[selectedShape].vertices.size() * sizeof(float), s[selectedShape].vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);

		glutPostRedisplay();
	}
}