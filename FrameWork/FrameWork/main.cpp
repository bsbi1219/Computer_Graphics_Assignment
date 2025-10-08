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

class FRAMEWORK
{
private:
	GLuint shaderProgramID;
	GLuint vertexShader, fragmentShader;
	GLuint VAO, VBO;
	GLint width = 900;
	GLint height = 900;

public:
	FRAMEWORK(int w, int h) : width(w), height(h) {}
	void Init();
	void DrawScene();
	void Reshape(int w, int h);

	void Init()
	{
		make_vertexShaders();
		make_fragmentShaders();
		shaderProgramID = make_shaderProgram();
	}
	GLvoid drawScene() //--- 콜백 함수: 그리기 콜백 함수
	{
		GLfloat rColor, gColor, bColor;
		rColor = gColor = bColor = 0.0f; // 배경색
		glClearColor(rColor, gColor, bColor, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shaderProgramID);
		glPointSize(5.0); // 굵기
		glBindVertexArray(VAO); // VAO 바인드
		GLint loc = glGetUniformLocation(shaderProgramID, "u_color");
		glUniform3f(loc, 1.0f, 1.0f, 1.0f); // 색 지정
		glDrawArrays(GL_TRIANGLES, 0, 3); // 그리기
		glBindVertexArray(0);
		glutSwapBuffers(); // 화면에 출력하기
	}
	GLvoid Reshape(int w, int h) // 다시 그리기 콜백 함수
	{
		glViewport(0, 0, w, h);
	}

private:
	void MakeVertexShader();
	void MakeFragmentShader();
	GLuint MakeShaderProgram();
	char* FileToBuf(const char* file);

	void make_vertexShaders()
	{
		GLchar* vertexSource;
		// 버텍스 세이더 읽어 저장하고 컴파일 하기
		// filetobuf: 사용자정의 함수로 텍스트를 읽어서 문자열에 저장하는 함수
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
		// 프래그먼트 세이더 읽어 저장하고 컴파일하기
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
};

void TimerFunction(int value); // 타이머
void Keyboard(unsigned char key, int x, int y); // 키 입력
void Mouse(int button, int state, int x, int y); // 마우스 입력
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

	fw.Init(); // 셰이더 세팅

	glutDisplayFunc([]() { fw.DrawScene(); });
	glutReshapeFunc([](int w, int h) { fw.Reshape(w, h); });
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);

	glutMainLoop();
}

// 키 입력
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

// 타이머
void TimerFunction(int value)
{
	switch (value)
	{
	case 1:
		break;
	}
}

// 마우스 입력
void Mouse(int button, int state, int x, int y)
{
	float mx = (float)x / width * 2.0f - 1.0f;
	float my = 1.0f - (float)y / height * 2.0f;
	if ((button == GLUT_LEFT_BUTTON && state == GLUT_DOWN))
	{

	}
	glutPostRedisplay();
}