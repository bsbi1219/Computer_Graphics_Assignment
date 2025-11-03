#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <gl/glm/gtc/type_ptr.hpp>
#define MAX_LINE_LENGTH 256

using namespace std;

random_device rd;
uniform_int_distribution<int> randFace(0, 5);

void Keyboard(unsigned char key, int x, int y);
void init();
void makeAxis();
void timer(int value);

typedef struct
{
    float x, y, z;
} Vertex;
typedef struct
{
    unsigned int v1, v2, v3;
} Face;
typedef struct
{
    Vertex* vertices;
    size_t vertex_count;
    Face* faces;
    size_t face_count;
} Model;
typedef struct
{
    float x, y, z;
    float r, g, b;
} Color;

void read_newline(char* str)
{
    char* pos;
    if ((pos = strchr(str, '\n')) != NULL) *pos = '\0';
}
void read_obj_file(const char* filename, Model* model)
{
    FILE* file;
    fopen_s(&file, filename, "r");
    if (!file) { perror("Error opening file"); exit(EXIT_FAILURE); }

    char line[MAX_LINE_LENGTH];
    model->vertex_count = 0;
    model->face_count = 0;

    while (fgets(line, sizeof(line), file))
    {
        read_newline(line);
        if (line[0] == 'v' && line[1] == ' ') model->vertex_count++;
        else if (line[0] == 'f' && line[1] == ' ') model->face_count++;
    }

    fseek(file, 0, SEEK_SET);
    model->vertices = (Vertex*)malloc(model->vertex_count * sizeof(Vertex));
    model->faces = (Face*)malloc(model->face_count * sizeof(Face));

    size_t v_idx = 0, f_idx = 0;
    while (fgets(line, sizeof(line), file))
    {
        read_newline(line);
        if (line[0] == 'v' && line[1] == ' ')
        {
            sscanf_s(line + 2, "%f %f %f",
                &model->vertices[v_idx].x,
                &model->vertices[v_idx].y,
                &model->vertices[v_idx].z);
            v_idx++;
        }
        else if (line[0] == 'f' && line[1] == ' ')
        {
            unsigned int v1, v2, v3;
            sscanf_s(line + 2, "%u %u %u", &v1, &v2, &v3);
            model->faces[f_idx].v1 = v1 - 1;
            model->faces[f_idx].v2 = v2 - 1;
            model->faces[f_idx].v3 = v3 - 1;
            f_idx++;
        }
    }
    fclose(file);
}
char* filetobuf(const char* file)
{
    FILE* fptr = fopen(file, "rb");
    if (!fptr) return NULL;
    fseek(fptr, 0, SEEK_END);
    long length = ftell(fptr);
    char* buf = (char*)malloc(length + 1);
    fseek(fptr, 0, SEEK_SET);
    fread(buf, length, 1, fptr);
    fclose(fptr);
    buf[length] = 0;
    return buf;
}

GLuint shaderProgram;
GLuint CubeVAO, CubeVBO, CubeEBO, AxisVAO, AxisVBO, PyrVAO, PyrVBO, PyrEBO;
Model cubeModel, PyramidModel;

GLenum polygonMode = GL_FILL;

bool drawCube = true;
bool keyH = true;
bool keyY = false;
bool keyS = false;
bool keyU = false;
bool keyT = false;
int keyF = 0;
int keyB = 0;
int keyO = 0;
int keyR = 0;
float angleX = 0.0f;
float angleY = 0.0f;
float angleZ = 0.0f;
float angle0 = 0.0f;
float angle1 = 0.0f;
float angle2 = 0.0f;
float angle3 = 0.0f;
float angleSide = 0.0f;
int dirX = 0;
float sz = 1.0f;
float sy = 1.0f;

glm::mat4 view = glm::lookAt(glm::vec3(-2.0f, 2.0f, 3.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 10.0f);
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 identity = glm::mat4(1.0f);

Color Colors[12]; // 큐브는 삼각형 12개
Color pyrColors[6]; // 피라미드는 삼각형 6개
Color colors[6] = { {1,0,0}, {0,1,0}, {0,0,1}, {1,1,0}, {0,1,1}, {1,0,1} };

void make_shader()
{
    char* vsrc = filetobuf("vertex.glsl");
    char* fsrc = filetobuf("fragment.glsl");

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vsrc, NULL);
    glCompileShader(vs);
    free(vsrc);

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fsrc, NULL);
    glCompileShader(fs);
    free(fsrc);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);

    glDeleteShader(vs);
    glDeleteShader(fs);
}
void init()
{
    glewInit();
    read_obj_file("cube.obj", &cubeModel);
    make_shader();

    // --- VAO, VBO, EBO 생성 ---
    glGenVertexArrays(1, &CubeVAO);
    glBindVertexArray(CubeVAO);

    // VBO: 꼭짓점 위치
    glGenBuffers(1, &CubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, CubeVBO);
    glBufferData(GL_ARRAY_BUFFER, cubeModel.vertex_count * sizeof(Vertex),
        cubeModel.vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &CubeEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeModel.face_count * 3 * sizeof(unsigned int), cubeModel.faces, GL_STATIC_DRAW);

    Color* coloredVertices = (Color*)malloc(cubeModel.vertex_count * sizeof(Color));
    for (size_t i = 0; i < cubeModel.vertex_count; ++i)
    {
		coloredVertices[i].x = cubeModel.vertices[i].x;
		coloredVertices[i].y = cubeModel.vertices[i].y;
		coloredVertices[i].z = cubeModel.vertices[i].z;

        coloredVertices[i].r = (float)i / cubeModel.vertex_count + 0.3f;
        coloredVertices[i].g = 1.0f - (float)i / cubeModel.vertex_count + 0.3f;
        coloredVertices[i].b = (float)(i % 2) + 0.3f;
    }

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(1);

    read_obj_file("pyramid.obj", &PyramidModel);

    glGenVertexArrays(1, &PyrVAO);
    glBindVertexArray(PyrVAO);

    glGenBuffers(1, &PyrVBO);
    glBindBuffer(GL_ARRAY_BUFFER, PyrVBO);
    glBufferData(GL_ARRAY_BUFFER, PyramidModel.vertex_count * sizeof(Vertex),
        PyramidModel.vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &PyrEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, PyrEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, PyramidModel.face_count * 3 * sizeof(unsigned int), PyramidModel.faces, GL_STATIC_DRAW);

    Color* coloredVertices2 = (Color*)malloc(PyramidModel.vertex_count * sizeof(Color));
    for (size_t i = 0; i < PyramidModel.vertex_count; ++i)
    {
        coloredVertices2[i].x = PyramidModel.vertices[i].x;
        coloredVertices2[i].y = PyramidModel.vertices[i].y;
        coloredVertices2[i].z = PyramidModel.vertices[i].z;

		coloredVertices2[i].r = (float)i / PyramidModel.vertex_count + 0.3f;
		coloredVertices2[i].g = 1.0f - (float)i / PyramidModel.vertex_count + 0.3f;
		coloredVertices2[i].b = (float)(i % 2) + 0.3f;
    }

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(1);

    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}
void makeAxis()
{
    GLfloat vertices[] =
    {
        -5.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
         5.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
         0.0f, -3.0f, 0.0f, 0.0f, 1.0f, 0.0f,
         0.0f,  3.0f, 0.0f, 0.0f, 1.0f, 0.0f,
         0.0f, 0.0f, -20.0f, 0.0f, 0.0f, 1.0f,
         0.0f, 0.0f,  20.0f, 0.0f, 0.0f, 1.0f
    };

    glGenVertexArrays(1, &AxisVAO);
    glBindVertexArray(AxisVAO);
    glGenBuffers(1, &AxisVBO);
    glBindBuffer(GL_ARRAY_BUFFER, AxisVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void display()
{
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);

	if (keyU)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

    if (keyH)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);

    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

    glBindVertexArray(CubeVAO);
    for (int i = 0; i < 6; ++i) // 6면
    {
        if (drawCube)
        {
            glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0, 1, 0));

            if (i == 0)
				model = glm::scale(model, glm::vec3(sz, sy, 1.0f));
            else if (i == 4)
            {
                glm::vec3 pivot(0.0f, 0.5f, 0.0f);
                model = glm::translate(model, pivot);
                model = glm::rotate(model, glm::radians(angleZ), glm::vec3(0, 0, 1));
                model = glm::translate(model, -pivot);
            }
            else if (i == 1)
            {
                unsigned int baseIdx = cubeModel.faces[i * 2].v3;
                Vertex baseVertex = cubeModel.vertices[baseIdx];
                glm::vec3 pivot(baseVertex.x, baseVertex.y, baseVertex.z);
                model = glm::translate(model, pivot);
                model = glm::rotate(model, glm::radians(angleX), glm::vec3(1, 0, 0));
                model = glm::translate(model, -pivot);
            }
            else if (i == 3 || i == 5)
                model = glm::rotate(model, glm::radians(angleSide), glm::vec3(1, 0, 0));

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

            glDrawElements(GL_TRIANGLES, 3 * 2, GL_UNSIGNED_INT, (void*)(i * 2 * 3 * sizeof(unsigned int)));
        }
    }

    glBindVertexArray(PyrVAO);
    for (int i = 0; i < PyramidModel.face_count; ++i)
    {
        if (!drawCube)
        {
            unsigned int baseIdx = PyramidModel.faces[i].v1;
            Vertex baseVertex = PyramidModel.vertices[baseIdx];
            glm::vec3 pivot(baseVertex.x, baseVertex.y, baseVertex.z);
            glm::mat4 pyrModel = glm::mat4(1.0f);
            pyrModel = glm::translate(pyrModel, pivot);

            if (i == 0)
                pyrModel = glm::rotate(pyrModel, glm::radians(angle0), glm::vec3(1, 0, 0));
            if (i == 2)
                pyrModel = glm::rotate(pyrModel, glm::radians(angle2), glm::vec3(1, 0, 0));
            if (i == 1)
                pyrModel = glm::rotate(pyrModel, glm::radians(angle1), glm::vec3(0, 0, 1));
            if (i == 3)
                pyrModel = glm::rotate(pyrModel, glm::radians(angle3), glm::vec3(0, 0, 1));

            pyrModel = glm::translate(pyrModel, -pivot);

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(pyrModel));

            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(i * 3 * sizeof(unsigned int)));
        }
    }

    glm::mat4 axisModel = glm::mat4(1.0f);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(axisModel));

    glBindVertexArray(AxisVAO);
    // X axis (red)
    glDrawArrays(GL_LINES, 0, 2);
    // Y axis (green)
    glDrawArrays(GL_LINES, 2, 2);
    // Z axis (blue)
    glDrawArrays(GL_LINES, 4, 2);

    glutSwapBuffers();

    // 카메라
    // angle += 0.5f;
    // if (angle > 360.0f) angle = 0.0f;
}

void reset()
{
    drawCube = true;
    keyH = true;
    keyY = false;
    keyS = false;
    keyF = 0;
    keyB = 0;
    keyO = 0;
    keyR = 0;
    angleX = 0.0f;
    angleY = 0.0f;
    angle0 = 0.0f;
    angle1 = 0.0f;
    angle2 = 0.0f;
    angle3 = 0.0f;
    angleSide = 0.0f;
    dirX = 0;
    sz = 1.0f;
    sy = 1.0f;
}

void Keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'h':
		keyH = !keyH;
        break;
    case 'p':
		drawCube = !drawCube;
        break;
    case 'y':
		keyY = !keyY;
        glutTimerFunc(16, timer, 1);
        break;
    case 'c':
        reset();
        break;
    case 't':
		keyT = !keyT;
		glutTimerFunc(16, timer, 7);
        break;
    case 'f':
		keyF += 1;
		if (keyF > 2) keyF = 1;
        glutTimerFunc(16, timer, 6);
        break;
    case 's':
		keyS = !keyS;
        glutTimerFunc(16, timer, 4);
        break;
    case 'b':
		keyB += 1;
		if (keyB > 2) keyB = 1;
        glutTimerFunc(16, timer, 5);
        break;
    case 'o':
        if (!drawCube)
        {
            keyO += 1;
			if (keyO > 2) keyO = 1;
            glutTimerFunc(16, timer, 2);
        }
        break;
    case 'r':
        if (!drawCube)
        {
            keyR += 1;
            if (keyR > 2) keyR = 1;
            glutTimerFunc(16, timer, 3);
        }
        break;
    case 'u':
		keyU = !keyU;
        break;
    case 'q':
        exit(0);
    }
    glutPostRedisplay();
}

float h = 1.0f;
float a = 1.0f;
float theta = acos(h / sqrt(h * h + (a / 2) * (a / 2))) * 180.0f / 3.14;
void timer(int value)
{
    static bool done[4] = { true, true, true, true };
    switch (value)
    {
    case 1: // y축 회전
        if (keyY)
        {
            angleY += 3.0f;
            if (angleY >= 360.0f) angleY -= 360.0f;
            glutPostRedisplay();
            glutTimerFunc(16, timer, 1);
            break;
        }
	case 2: // 피라미드 한 번에 열기/닫기
        if (keyO == 1)
        {
            if (angle0 <= (-90.0f - theta) * 2.0f)
            {
                break;
            }
            angle0 -= 2.0f;
            angle2 += 2.0f;
            angle1 -= 2.0f;
            angle3 += 2.0f;
        }
        else if (keyO == 2)
        {
            if (angle0 >= 0.0f)
            {
                break;
            }
            angle0 += 2.0f;
            angle2 -= 2.0f;
            angle1 += 2.0f;
            angle3 -= 2.0f;
        }
        glutPostRedisplay();
        glutTimerFunc(16, timer, 2);
        break;
	case 3: // 피라미드 하나씩 열기/닫기
        if (keyR == 1)
        {
            if (done[0])
            {
                angle0 -= 2.0f;
				if (angle0 <= -116.0f)
					done[0] = false;
            }
            else if (done[1])
            {
                angle1 -= 2.0f;
                if (angle1 <= -116.0f)
                    done[1] = false;
            }
            else if (done[2])
            {
                angle2 += 2.0f;
				if (angle2 >= 116.0f)
					done[2] = false;
            }
            else if (done[3])
            {
                angle3 += 2.0f;
			    if (angle3 >= 116.0f)
				    done[3] = false;
            }
        }
        else if (keyR == 2)
        {
            if (!done[0])
            {
                angle0 += 2.0f;
				if (angle0 >= 0.0f)
					done[0] = true;
            }
            else if (!done[1])
            {
                angle1 += 2.0f;
                if (angle1 >= 0.0f)
                    done[1] = true;
            }
            else if (!done[2])
            {
                angle2 -= 2.0f;
                if (angle2 <= 0.0f)
                    done[2] = true;
            }
            else if (!done[3])
			{
                angle3 -= 2.0f;
                if (angle3 <= 0.0f)
                    done[3] = true;
			}
        }
        glutPostRedisplay();
        glutTimerFunc(16, timer, 3);
        break;
	case 4: // 큐브 측면 회전
        if (keyS)
        {
            angleSide += 3.0f;
            if (angleSide >= 360.0f) angleSide -= 360.0f;
            glutPostRedisplay();
            glutTimerFunc(16, timer, 4);
            break;
        }
	case 5: // 큐브 뒷면 스케일링
        if (keyB == 1)
        {
            sz -= 0.01f;
            sy -= 0.01f;
            if (sz <= 0.0f) break;
		}
        else if (keyB == 2)
        {
            sz += 0.01f;
            sy += 0.01f;
			if (sz >= 1.0f) keyB = 0;
        }
        glutPostRedisplay();
        glutTimerFunc(16, timer, 5);
        break;
    case 6: // 큐브 앞면 열고 닫기
        if (keyF == 1)
        {
			angleX -= 2.0f;
			if (angleX <= -90.0f) break;
        }
		else if (keyF == 2)
		{
			angleX += 2.0f;
			if (angleX >= 0.0f) break;
		}
        glutPostRedisplay();
        glutTimerFunc(16, timer, 6);
        break;
    case 7:
		if (keyT)
		{
			angleZ += 3.0f;
			if (angleZ >= 360.0f) angleZ -= 360.0f;
			glutPostRedisplay();
			glutTimerFunc(16, timer, 7);
			break;
		}
    }
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OBJ Cube 3D");

    init();
    makeAxis();
    reset();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(Keyboard);

    glutMainLoop();
    return 0;
}