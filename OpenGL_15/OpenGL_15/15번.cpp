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
    float r, g, b;
} FaceColor;

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
GLuint VAO, VBO, EBO, AxisVAO, AxisVBO, PyrVAO, PyrVBO, PyrEBO;
Model cubeModel, PyramidModel;

float angle = 0.0f;

bool face[6] = { false, false, false, false, false, false };
bool TriFace[6] = { false, false, false, false, false, false };

glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(1, 1, 0));
glm::mat4 view = glm::lookAt(glm::vec3(-2.0f, 2.0f, 3.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 10.0f);

glm::mat4 identity = glm::mat4(1.0f);

FaceColor faceColors[12]; // 큐브는 삼각형 12개
FaceColor pyrFaceColors[6]; // 피라미드는 삼각형 6개
FaceColor colors[6] = { {1,0,0}, {0,1,0}, {0,0,1}, {1,1,0}, {0,1,1}, {1,0,1} };

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
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // VBO: 꼭짓점 위치
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, cubeModel.vertex_count * sizeof(Vertex),
        cubeModel.vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cubeModel.face_count * 3 * sizeof(unsigned int), cubeModel.faces, GL_STATIC_DRAW);

    for (int i = 0; i < cubeModel.face_count; ++i)
    {
        faceColors[i].r = (float)(i % 3 == 0);
        faceColors[i].g = (float)(i % 3 == 1);
        faceColors[i].b = (float)(i % 3 == 2);
    }

    for (int i = 0; i < 6; i++)
    {
        faceColors[i * 2] = colors[i];
        faceColors[i * 2 + 1] = colors[i];
    }

    read_obj_file("pyramid.obj", &PyramidModel);

    glGenVertexArrays(1, &PyrVAO);
    glBindVertexArray(PyrVAO);

    glGenBuffers(1, &PyrVBO);
    glBindBuffer(GL_ARRAY_BUFFER, PyrVBO);
    glBufferData(GL_ARRAY_BUFFER, PyramidModel.vertex_count * sizeof(Vertex),
        PyramidModel.vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &PyrEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, PyrEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, PyramidModel.face_count * 3 * sizeof(unsigned int), PyramidModel.faces, GL_STATIC_DRAW);

    for (int i = 0; i < PyramidModel.face_count; ++i)
    {
        pyrFaceColors[i].r = (float)(i % 3 == 0);
        pyrFaceColors[i].g = (float)(i % 3 == 1);
        pyrFaceColors[i].b = (float)(i % 3 == 2);
    }

    for (int i = 0; i < 6; i++)
    {
        if (i < 4)
            pyrFaceColors[i] = colors[i];
        if (i == 4)
        {
            pyrFaceColors[i] = colors[i];
            pyrFaceColors[i + 1] = colors[i];
            break;
        }
    }

    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}
void makeAxis()
{
    GLfloat vertices[] =
    {
        // X axis (red)
        -5.0f, 0.0f, 0.0f,
        5.0f, 0.0f, 0.0f,
        // Y axis (green)
        0.0f, -3.0f, 0.0f,
        0.0f, 3.0f, 0.0f,
        // Z axis (blue)
        0.0f, 0.0f, -20.0f,
        0.0f, 0.0f, 20.0f
    };

	glGenVertexArrays(1, &AxisVAO);
	glBindVertexArray(AxisVAO);
	glGenBuffers(1, &AxisVBO);
	glBindBuffer(GL_ARRAY_BUFFER, AxisVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

void display() 
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);

    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(AxisVAO);
    // X axis (red)
    glUniform3f(glGetUniformLocation(shaderProgram, "u_color"), 1.0f, 0.0f, 0.0f);
    glDrawArrays(GL_LINES, 0, 2);
    // Y axis (green)
    glUniform3f(glGetUniformLocation(shaderProgram, "u_color"), 0.0f, 1.0f, 0.0f);
    glDrawArrays(GL_LINES, 2, 2);
    // Z axis (blue)
    glUniform3f(glGetUniformLocation(shaderProgram, "u_color"), 0.0f, 0.0f, 1.0f);
    glDrawArrays(GL_LINES, 4, 2);

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(VAO);
    for (int i = 0; i < 6; ++i) // 6면
    {
        if (face[i])
        {
            glUniform3f(glGetUniformLocation(shaderProgram, "u_color"),
                faceColors[i * 2].r, faceColors[i * 2].g, faceColors[i * 2].b);

            glDrawElements(GL_TRIANGLES, 3 * 2, GL_UNSIGNED_INT, (void*)(i * 2 * 3 * sizeof(unsigned int)));
        }
    }

    glBindVertexArray(PyrVAO);
    for (int i = 0; i < PyramidModel.face_count; ++i)
    {
        if (TriFace[i])
        {
            glUniform3f(glGetUniformLocation(shaderProgram, "u_color"),
                pyrFaceColors[i].r, pyrFaceColors[i].g, pyrFaceColors[i].b);
            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(i * 3 * sizeof(unsigned int)));
        }
    }

    glutSwapBuffers();

    // 카메라
    // angle += 0.5f;
    // if (angle > 360.0f) angle = 0.0f;
}

void returnFace(int num)
{
	if (face[num])
		face[num] = false;
	else
		face[num] = true;
}

void returnPyr(int num)
{
	if (num == 7 && TriFace[0])
		TriFace[0] = false;
	else if (num == 7 && !TriFace[0])
		TriFace[0] = true;
	else if (num == 8 && TriFace[1])
		TriFace[1] = false;
	else if (num == 8 && !TriFace[1])
		TriFace[1] = true;
	else if (num == 9 && TriFace[2])
		TriFace[2] = false;
	else if (num == 9 && !TriFace[2])
		TriFace[2] = true;
	else if (num == 0 && TriFace[3])
		TriFace[3] = false;
	else if (num == 0 && !TriFace[3])
		TriFace[3] = true;
}

void randomFace()
{
    int falseCnt = 0;
    for (int i = 0; i < 6; ++i)
        if (!face[i]) falseCnt++;

    if (falseCnt < 2) return;

    int ran1 = -1, ran2 = -1;
    do {
        ran1 = randFace(rd);
    } while (face[ran1]);
    face[ran1] = true;

    do {
        ran2 = randFace(rd);
    } while (face[ran2] || ran2 == ran1);
    face[ran2] = true;
}

void randomPyr()
{
	int falseCnt = 0;
	for (int i = 0; i < 4; ++i)
		if (!TriFace[i]) falseCnt++;
	if (falseCnt == 0) return;

	TriFace[4] = true; TriFace[5] = true;
    int ran1 = -1;
	do {
		ran1 = randFace(rd) % 4;
	} while (TriFace[ran1]);
    TriFace[ran1] = true;
}

void Keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
	case '1': case '2': case '3': case '4': case '5': case '6':
		for (auto& val : TriFace)
			val = false;
        returnFace(key - '1');
		break;
    case '7': case '8': case '9': case '0':
        for (auto& val : face)
            val = false;
		returnPyr(key - '0');
		break;
    case 'c':
        randomFace();
        break;
    case 't':
        randomPyr();
        break;
    case 'q':
        exit(0);
    }
    glutPostRedisplay();
}

void timer(int value) 
{
    glutPostRedisplay();
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

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(Keyboard);

    glutMainLoop();
    return 0;
}
