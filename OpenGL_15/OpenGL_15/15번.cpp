#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <string>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <gl/glm/gtc/type_ptr.hpp>
using namespace std;

#define MAX_LINE_LENGTH 256

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
void read_newline(char* str) 
{
    char* pos;
    if ((pos = strchr(str, '\n')) != NULL)
        *pos = '\0';
}
void read_obj_file(const char* filename, Model* model) 
{
    FILE* file;
    fopen_s(&file, filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    char line[MAX_LINE_LENGTH];
    model->vertex_count = 0;
    model->face_count = 0;
    while (fgets(line, sizeof(line), file)) 
    {
        read_newline(line);
        if (line[0] == 'v' && line[1] == ' ')
            model->vertex_count++;
        else if (line[0] == 'f' && line[1] == ' ')
            model->face_count++;
    }
    fseek(file, 0, SEEK_SET);
    model->vertices = (Vertex*)malloc(model->vertex_count * sizeof(Vertex));
    model->faces = (Face*)malloc(model->face_count * sizeof(Face));
    size_t vertex_index = 0; size_t face_index = 0;
    while (fgets(line, sizeof(line), file)) 
    {
        read_newline(line);
        if (line[0] == 'v' && line[1] == ' ') 
        {
            int result = sscanf_s(line + 2, "%f %f %f", &model->vertices[vertex_index].x,
                &model->vertices[vertex_index].y,
                &model->vertices[vertex_index].z);
            vertex_index++;
        }
        else if (line[0] == 'f' && line[1] == ' ') 
        {
            unsigned int v1, v2, v3;
            int result = sscanf_s(line + 2, "%u %u %u", &v1, &v2, &v3);
            model->faces[face_index].v1 = v1 - 1; // OBJ indices start at 1
            model->faces[face_index].v2 = v2 - 1;
            model->faces[face_index].v3 = v3 - 1;
            face_index++;
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
GLuint VAO, VBO, EBO;
Model cubeModel;

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

// === 초기화 ===
void init() 
{
    glewInit();
    read_obj_file("cube.obj", &cubeModel);
    make_shader();

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        cubeModel.vertex_count * sizeof(Vertex),
        cubeModel.vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        cubeModel.face_count * 3 * sizeof(unsigned int),
        cubeModel.faces, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0, 0.0, 0.0, 1.0);
}

// === 렌더링 ===
float angle = 0.0f;

void display() 
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);

    glm::mat4 model(1.0f);
    model = glm::rotate(model, glm::radians(angle), glm::vec3(1, 1, 0));

    glm::mat4 view = glm::lookAt(
        glm::vec3(1.5f, 1.5f, 1.5f),  // 카메라 위치
        glm::vec3(0.0f, 0.0f, 0.0f),  // 바라보는 지점
        glm::vec3(0.0f, 1.0f, 0.0f)); // 업 벡터

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 10.0f);

    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, cubeModel.face_count * 3, GL_UNSIGNED_INT, 0);

    glutSwapBuffers();
    angle += 0.5f;
    if (angle > 360.0f) angle = 0.0f;
}

void timer(int value) 
{
    
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
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    //glutTimerFunc(16, timer, 0);

    glutMainLoop();
    return 0;
}
