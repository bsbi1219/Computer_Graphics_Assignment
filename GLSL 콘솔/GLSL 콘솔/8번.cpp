#define _CRT_SECURE_NO_WARNINGS //--- ���α׷� �� �տ� ������ ��
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
// void InitBuffer();
char* filetobuf(const char* file); //--- �ؽ�Ʈ ������ �о ���ڿ��� ��ȯ�ϴ� �Լ�

random_device rd;
uniform_real_distribution<float> ranColor(0.0f, 1.0f);

GLint width, height;
GLuint shaderProgramID; //--- ���̴� ���α׷� �̸�
GLuint vertexShader; //--- ���ؽ� ���̴� ��ü
GLuint fragmentShader; //--- �����׸�Ʈ ���̴� ��ü

int num = -1;
int shape_num = 0;
int selected_shape = -1;
GLuint VAO;

struct SHAPE
{
	int type;
	GLuint VAO;
	GLuint VBO;
	GLuint EBO;
	float vx[4];
	float vy[4];
	float r, g, b;
	int vertexCount;
	int indexCount;
}shape[10];

void main(int argc, char** argv) //--- ������ ����ϰ� �ݹ��Լ� ����
{
	width = 500;
	height = 500;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // ȭ�� ���
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("Example1");
	glewExperimental = GL_TRUE;
	glewInit();
	make_vertexShaders();
	make_fragmentShaders();
	shaderProgramID = make_shaderProgram();
	// InitBuffer();
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMainLoop();
}

GLvoid drawScene() //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
{
	GLfloat rColor, gColor, bColor;
	rColor = gColor = bColor = 0.0f; // ����
	glClearColor(rColor, gColor, bColor, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	glPointSize(5.0);
	glBindVertexArray(VAO);
	for (int i = 0; i < shape_num; ++i)
	{
		glBindVertexArray(shape[i].VAO);
		GLint loc = glGetUniformLocation(shaderProgramID, "u_color");
		glUniform3f(loc, shape[i].r, shape[i].g, shape[i].b);

		if (shape[i].type == 0) // ��
			glDrawArrays(GL_POINTS, 0, shape[i].vertexCount);
		else if (shape[i].type == 1) // ��
			glDrawArrays(GL_LINES, 0, shape[i].vertexCount);
		else if (shape[i].type == 2) // �ﰢ��
			glDrawArrays(GL_TRIANGLES, 0, shape[i].vertexCount);
		else if (shape[i].type == 3) // �簢��
		{
			glBindVertexArray(shape[i].VAO);
			glDrawElements(GL_TRIANGLES, shape[i].indexCount, GL_UNSIGNED_INT, 0);
		}
	}
	glBindVertexArray(0);
	glutSwapBuffers(); // ȭ�鿡 ����ϱ�
}
//--- �ٽñ׸��� �ݹ� �Լ�
GLvoid Reshape(int w, int h) //--- �ݹ� �Լ�: �ٽ� �׸��� �ݹ� �Լ�
{
	glViewport(0, 0, w, h);
}

void make_vertexShaders()
{
	GLchar* vertexSource;
	//--- ���ؽ� ���̴� �о� �����ϰ� ������ �ϱ�
	//--- filetobuf: ��������� �Լ��� �ؽ�Ʈ�� �о ���ڿ��� �����ϴ� �Լ�
	vertexSource = filetobuf("vertex.glsl");
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if(!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}

void make_fragmentShaders()
{
	GLchar* fragmentSource;
	//--- �����׸�Ʈ ���̴� �о� �����ϰ� �������ϱ�
	fragmentSource = filetobuf("fragment.glsl"); // �����׼��̴� �о����
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: frag_shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}

void createShape(int num, float x, float y)
{
	float vertices[12];
	unsigned int indices[6];
	int vCnt = 0;
	int iCnt = 0;

	if (num == 0)
	{
		float temp[] = { x, y, 0.0f };
		memcpy(vertices, temp, sizeof(temp));
		vCnt = 1;
	}
	else if (num == 1)
	{
		float temp[] = 
		{ 
			x, y, 0.0f, 
			x, y + 0.1f, 0.0f 
		};
		memcpy(vertices, temp, sizeof(temp));
		vCnt = 2;
	}
	else if (num == 2)
	{
		float temp[] = 
		{ 
			x, y, 0.0f, 
			x + 0.1f, y, 0.0f, 
			x, y + 0.1f, 0.0f 
		};
		memcpy(vertices, temp, sizeof(temp));
		vCnt = 3;
	}
	else if (num == 3)
	{
		float temp[] = 
		{
			x, y, 0.0f,
			x + 0.1f, y, 0.0f,
			x + 0.1f, y + 0.1f, 0.0f,
			x, y + 0.1f, 0.0f
		};
		memcpy(vertices, temp, sizeof(temp));
		vCnt = 4;

		unsigned int idx[] = 
		{
			0, 1, 2,
			2, 3, 0
		};
		memcpy(indices, idx, sizeof(idx));
		iCnt = 6;
	}

	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vCnt * 3, vertices, GL_STATIC_DRAW);

	if (num == 3) { // �簢��
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * iCnt, indices, GL_STATIC_DRAW);
	}

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	shape[shape_num].vx[0] = x;
	shape[shape_num].vy[0] = y;
	if (num == 1)
	{
		shape[shape_num].vx[1] = x;
		shape[shape_num].vy[1] = y + 0.1f;
	}
	if (num == 2)
	{
		shape[shape_num].vx[1] = x + 0.1f;
		shape[shape_num].vy[1] = y;

		shape[shape_num].vx[2] = x;
		shape[shape_num].vy[2] = y + 0.1f;
	}
	if (num == 3)
	{
		shape[shape_num].vx[1] = x + 0.1f;
		shape[shape_num].vy[1] = y;

		shape[shape_num].vx[2] = x + 0.1f;
		shape[shape_num].vy[2] = y + 0.1f;

		shape[shape_num].vx[3] = x;
		shape[shape_num].vy[3] = y + 0.1f;
	}
	shape[shape_num].VAO = VAO;
	shape[shape_num].VBO = VBO;
	shape[shape_num].EBO = EBO;
	shape[shape_num].r = ranColor(rd);
	shape[shape_num].g = ranColor(rd);
	shape[shape_num].b = ranColor(rd);
	shape[shape_num].type = num;
	shape[shape_num].vertexCount = vCnt;
	shape[shape_num].indexCount = iCnt;
	shape_num++;
}

void moveShape(char key)
{
	SHAPE& s = shape[selected_shape];

	switch(key)
	{
        case 'w': // ��
			for (int i = 0; i < s.vertexCount; ++i) s.vy[i] += 0.03;
			break;
		case 's': // �Ʒ�
			for (int i = 0; i < s.vertexCount; ++i) s.vy[i] -= 0.03;
			break;
		case 'a': // ����
			for (int i = 0; i < s.vertexCount; ++i) s.vx[i] -= 0.03;
			break;
		case 'd': // ������
			for (int i = 0; i < s.vertexCount; ++i) s.vx[i] += 0.03;
			break;
		case 'i':
			for (int i = 0; i < s.vertexCount; ++i) 
			{
				s.vx[i] -= 0.03;
				s.vy[i] += 0.03;
			}
			break;
		case 'j':
			for (int i = 0; i < s.vertexCount; ++i)
			{
				s.vx[i] += 0.03;
				s.vy[i] += 0.03;
			}
			break;
		case 'k':
			for (int i = 0; i < s.vertexCount; ++i)
			{
				s.vx[i] -= 0.03;
				s.vy[i] -= 0.03;
			}
			break;
		case 'l':
			for (int i = 0; i < s.vertexCount; ++i)
			{
				s.vx[i] += 0.03;
				s.vy[i] -= 0.03;
			}
			break;
	}
	float vertices[12];
	for (int i = 0; i < s.vertexCount; i++)
	{
		vertices[i * 3 + 0] = s.vx[i];
		vertices[i * 3 + 1] = s.vy[i];
		vertices[i * 3 + 2] = 0.0f;
	}
	glBindBuffer(GL_ARRAY_BUFFER, s.VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * s.vertexCount * 3, vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GLuint make_shaderProgram()
{
	GLint result;
	GLchar* errorLog = NULL;
	GLuint shaderID;
	shaderID = glCreateProgram(); //--- ���̴� ���α׷� �����
	glAttachShader(shaderID, vertexShader); //--- ���̴� ���α׷��� ���ؽ� ���̴� ���̱�
	glAttachShader(shaderID, fragmentShader); //--- ���̴� ���α׷��� �����׸�Ʈ ���̴� ���̱�
	glLinkProgram(shaderID); //--- ���̴� ���α׷� ��ũ�ϱ�
	glDeleteShader(vertexShader); //--- ���̴� ��ü�� ���̴� ���α׷��� ��ũ��������, ���̴� ��ü ��ü�� ���� ����
	glDeleteShader(fragmentShader);
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result); // ---���̴��� �� ����Ǿ����� üũ�ϱ�
	if (!result) {
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program ���� ����\n" << errorLog << std::endl;
		return false;
	}
	glUseProgram(shaderID); //--- ������� ���̴� ���α׷� ����ϱ�
	//--- ���� ���� ���̴����α׷� ���� �� �ְ�, �� �� �Ѱ��� ���α׷��� ����Ϸ���
	//--- glUseProgram �Լ��� ȣ���Ͽ� ��� �� Ư�� ���α׷��� �����Ѵ�.
	//--- ����ϱ� ������ ȣ���� �� �ִ�.
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
	case 'p':
		num = 0;
		break;
	case 'o':
		num = 1;
		break;
	case 't':
		num = 2;
		break;
	case 'r':
		num = 3;
		break;
	case 'w': case 's' : case 'a' : case 'd':
		moveShape(key);
		glutPostRedisplay();
		break;
	case 'i': case 'j': case 'k': case 'l':
		moveShape(key);
		glutPostRedisplay();
		break;
	case 'c':
		shape_num = 0;
		glutPostRedisplay();
		break;
	}
}

void Mouse(int button, int state, int x, int y)
{
	float ox = (float)x / width * 2.0f - 1.0f;
	float oy = 1.0f - (float)y / height * 2.0f;
	if ((button == GLUT_LEFT_BUTTON && state == GLUT_DOWN))
	{
		GLint loc = glGetUniformLocation(shaderProgramID, "u_mouse");
		glUseProgram(shaderProgramID);
		glUniform2f(loc, ox, oy);
		if (shape_num < 10)
		{
			createShape(num, ox, oy);
			glutPostRedisplay();
		}
	}
	if ((button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN))
	{
		GLint loc = glGetUniformLocation(shaderProgramID, "u_mouse");
		glUseProgram(shaderProgramID);
		glUniform2f(loc, ox, oy);
		for (int i = 0; i < shape_num; ++i)
		{
			if (shape[i].type == 0)
			{
				if (abs(shape[i].vx[0] - ox) < 0.01 && abs(shape[i].vy[0] - oy) < 0.01)
				{
					selected_shape = i;
					cout << "�� ����" << endl;
					break;
				}
			}
			else if (shape[i].type == 1)
			{
				if (abs(shape[i].vx[0] - ox) < 0.01 && abs(shape[i].vy[0] - oy) <= 0.1)
				{
					selected_shape = i;
					cout << "�� ����" << endl;
					break;
				}
			}
			else if (shape[i].type == 2)
			{
				float minX = min({ shape[i].vx[0], shape[i].vx[1], shape[i].vx[2] });
				float maxX = max({ shape[i].vx[0], shape[i].vx[1], shape[i].vx[2] });
				float minY = min({ shape[i].vy[0], shape[i].vy[1], shape[i].vy[2] });
				float maxY = max({ shape[i].vy[0], shape[i].vy[1], shape[i].vy[2] });
				if(ox >= minX && ox <= maxX && oy >= minY && oy <= maxY)
				{
					selected_shape = i;
					cout << "�ﰢ�� ����" << endl;
					break;
				}
			}
			else if (shape[i].type == 3)
			{
				if (shape[i].vx[0] <= ox && shape[i].vx[1] >= ox && shape[i].vy[0] <= oy && shape[i].vy[3] >= oy)
				{
					selected_shape = i;
					cout << "�簢�� ����" << endl;
					break;
				}
			}
		}
	}
}