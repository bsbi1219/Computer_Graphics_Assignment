#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <iostream>
#include <random>

using namespace std;
random_device rd;
// uniform_real_distribution<float> ranColor(0.0f, 1.0f);

GLint width = 900;
GLint height = 900;

// ���� �� VAO, VBO���� + �׸���
class DRAW
{
private:
	vector<float> vertices;
	GLuint VAO, VBO;

public:
	DRAW(const vector<float>& verts) : vertices(verts)
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void Draw()
	{
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);
		glBindVertexArray(0);
	}

	GLuint getVAO() const { return VAO; }

	~DRAW()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}
};

// ��ü Ʋ ����
class FRAMEWORK
{
private:
	GLuint shaderProgramID;
	GLuint vertexShader, fragmentShader;

public:
	FRAMEWORK(int w, int h)
	{
		width = w;
		height = h;
	}
	void Init()
	{
		make_vertexShaders();
		make_fragmentShaders();
		shaderProgramID = make_shaderProgram();
	}
	GLvoid DrawScene(const DRAW& d) //--- �ݹ� �Լ�: �׸��� �ݹ� �Լ�
	{
		GLfloat rColor, gColor, bColor;
		rColor = gColor = bColor = 0.0f; // ����
		glClearColor(rColor, gColor, bColor, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shaderProgramID);
		glPointSize(5.0); // ����
		glBindVertexArray(d.getVAO()); // VAO ���ε�
		GLint loc = glGetUniformLocation(shaderProgramID, "u_color");
		glUniform3f(loc, 1.0f, 1.0f, 1.0f); // �� ����
		glDrawArrays(GL_TRIANGLES, 0, 3); // �׸���
		glBindVertexArray(0);
		glutSwapBuffers(); // ȭ�鿡 ����ϱ�
	}
	GLvoid Reshape(int w, int h) // �ٽ� �׸��� �ݹ� �Լ�
	{
		glViewport(0, 0, w, h);
	}

private:
	void make_vertexShaders()
	{
		GLchar* vertexSource;
		// ���ؽ� ���̴� �о� �����ϰ� ������ �ϱ�
		// filetobuf: ��������� �Լ��� �ؽ�Ʈ�� �о ���ڿ��� �����ϴ� �Լ�
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
			std::cerr << "ERROR: vertex shader ������ ����\n" << errorLog << std::endl;
			return;
		}
	}
	void make_fragmentShaders()
	{
		GLchar* fragmentSource;
		// �����׸�Ʈ ���̴� �о� �����ϰ� �������ϱ�
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
};

void TimerFunction(int value); // Ÿ�̸�
void Keyboard(unsigned char key, int x, int y); // Ű �Է�
void Mouse(int button, int state, int x, int y); // ���콺 �Է�
FRAMEWORK fw(900, 900);

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(900, 900);
	glutCreateWindow("Framework");

	glewExperimental = GL_TRUE;
	glewInit();

	fw.Init(); // ���̴� ����

	glutDisplayFunc([]() { fw.DrawScene(); });
	glutReshapeFunc([](int w, int h) { fw.Reshape(w, h); });
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);

	glutMainLoop();
}

// Ű �Է�
void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'q':
		exit(0);
		break;
	}
	glutPostRedisplay();
}

// Ÿ�̸�
void TimerFunction(int value)
{
	switch (value)
	{
	case 1:
		break;
	}
}

// ���콺 �Է�
void Mouse(int button, int state, int x, int y)
{
	float mx = (float)x / width * 2.0f - 1.0f;
	float my = 1.0f - (float)y / height * 2.0f;
	if ((button == GLUT_LEFT_BUTTON && state == GLUT_DOWN))
	{

	}
	glutPostRedisplay();
}