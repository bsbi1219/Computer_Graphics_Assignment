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
#define MAX_LINE_LENGTH 4096

using namespace std;

random_device rd;
uniform_int_distribution<int> randFace(0, 5);

struct OBJSHAPE;
void read_obj_file(const char* filename, OBJSHAPE* model);
void read_newline(char* str);
void make_shader();
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
    float x, y, z;
    float r, g, b;
} Color;

class OBJSHAPE {
public:
    Vertex* vertices = nullptr;
    size_t vertex_count = 0;
    Face* faces = nullptr;
    size_t face_count = 0;

    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;

    // init은 이제 멤버로서 자신의 VAO VBO EBO를 설정
    void init(const char* fileName)
    {
        read_obj_file(fileName, this);


        unsigned int* indices = (unsigned int*)malloc(face_count * 3 * sizeof(unsigned int));
        for (size_t i = 0; i < face_count; ++i)
        {
            indices[i * 3 + 0] = faces[i].v1;
            indices[i * 3 + 1] = faces[i].v2;
            indices[i * 3 + 2] = faces[i].v3;
        }

        // 컬러 생성 (간단히 정점당 색을 만든다)
        Color* colored = (Color*)malloc(vertex_count * sizeof(Color));
        for (size_t i = 0; i < vertex_count; ++i)
        {
            colored[i].x = vertices[i].x;
            colored[i].y = vertices[i].y;
            colored[i].z = vertices[i].z;
            colored[i].r = (float)i / vertex_count + 0.1f;
            colored[i].g = 1.0f - (float)i / vertex_count + 0.1f;
            colored[i].b = (float)(i % 2) * 0.5f + 0.1f;
        }

        // VAO VBO EBO 생성
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        // VBO: 위치만 업로드. 컬러는 별도 VBO로 올릴 수도 있음. 여기선 interleave 대신 두 VBO 사용
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(Vertex), vertices, GL_STATIC_DRAW);
        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);

        // color buffer
        GLuint colorVBO;
        glGenBuffers(1, &colorVBO);
        glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
        glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(Color), colored, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Color), (void*)(sizeof(float) * 3));
        // 위줄은 Color 구조체가 x y z r g b 로 되어 있으니 실제 RGB 오프셋을 맞춰야 함
        // 안전하게는 offsetof 사용 권장. 여기서는 간단히 아래처럼 설정
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Color), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // EBO
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, face_count * 3 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

        // 정리
        free(indices);
        free(colored);
        glBindVertexArray(0);
    }
};

class CUBE : public OBJSHAPE
{
public:
    CUBE() { }
    ~CUBE() { }
};

class PYRAMID : public OBJSHAPE
{
public:
	PYRAMID() { }
	~PYRAMID() { }
};

class SPHERE : public OBJSHAPE
{  
public:
    SPHERE() { }
    ~SPHERE() { }
};

class CYLINDER : public OBJSHAPE
{
public:
    CYLINDER() { }
    ~CYLINDER() { }
};

CUBE cube;
PYRAMID pyramid;
SPHERE sphere;
CYLINDER cylinder;
GLuint AxisVAO, AxisVBO;

void read_newline(char* str)
{
    char* pos;
    if ((pos = strchr(str, '\n')) != NULL) *pos = '\0';
}
void read_obj_file(const char* filename, OBJSHAPE* model)
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
            unsigned int v[3];
            char* ptr = line + 2;
            for (int i = 0; i < 3; ++i) {
                v[i] = atoi(ptr) - 1;
                ptr = strchr(ptr, ' ');
                if (!ptr) break;
                ptr++;
            }
            model->faces[f_idx].v1 = v[0];
            model->faces[f_idx].v2 = v[1];
            model->faces[f_idx].v3 = v[2];
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

glm::mat4 view = glm::lookAt(glm::vec3(-2.0f, 2.0f, 3.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 10.0f);
glm::mat4 model = glm::mat4(1.0f);
glm::mat4 identity = glm::mat4(1.0f);

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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);

    // view, projection, model 행렬 전달
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // 큐브 그리기
    glm::mat4 modelCube = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelCube));

    glBindVertexArray(cube.VAO);
    glDrawElements(GL_TRIANGLES, cube.face_count * 3, GL_UNSIGNED_INT, 0);

    // 구 그리기
    glm::mat4 modelSphere = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, -3.0f, 0.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelSphere));

    glBindVertexArray(sphere.VAO);
    glDrawElements(GL_TRIANGLES, sphere.face_count * 3, GL_UNSIGNED_INT, 0);

    // 실린더 그리기
    glm::mat4 modelCylinder = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelCylinder));

    glBindVertexArray(cylinder.VAO);
    glDrawElements(GL_TRIANGLES, cylinder.face_count * 3, GL_UNSIGNED_INT, 0);

    // 축
    glm::mat4 modelAxis = glm::mat4(1.0f);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelAxis));
    glBindVertexArray(AxisVAO);
    glDrawArrays(GL_LINES, 0, 6);

    glutSwapBuffers();
}

void Keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'x': case 'X':
        break;
    case 'y': case 'Y':
		break;
    case 'r': case 'R':
		break;
    case 'a': case 'A':
		break;
	case 'b': case 'B': 
		break;
    case 'd': case 'D':
		break;
	case 'e': case 'E':
		break;
    case 't':
        break;
    case 'u':
        break;
    case 'v':
        break;
    case 'c':
        break;
    case 's':
        break;
    case 'q':
        exit(0);
    }
    glutPostRedisplay();
}

void timer(int value)
{
    switch (value)
    {
    case 1:
        break;
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
    glutCreateWindow("computer Grapics");

    glewInit();

    make_shader();

    init();
    makeAxis();

    cube.init("cube.obj");
    pyramid.init("pyramid.obj");
	sphere.init("sphere.obj");
	cylinder.init("cylinder.obj");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(Keyboard);

    glutMainLoop();
    return 0;
}