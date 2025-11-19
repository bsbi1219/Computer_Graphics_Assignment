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
uniform_int_distribution<int> randUD(1, 2);
uniform_real_distribution<float> ranHeight(1.0f, 6.0f);
uniform_real_distribution<float> ranSpeed(0.1f, 0.2f);
uniform_real_distribution<float> ranColor(0.0f, 1.0f);

struct OBJSHAPE;
void read_obj_file(const char* filename, OBJSHAPE* model);
void read_newline(char* str);
void make_shader();
void Keyboard(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);
void init();
void timer(int value);
void consoleOrder();
void makeMaze();
void reset();

string order{};
stringstream ss{};
int weight{};
int height{};
float wSpace{}, hSpace{};
int cubeCnt{};
float xPos = 0.0f;
float yPos = 15.0f;
float zPos = 20.0f;

bool proj = true;
bool start = false;
bool startEnd = false;
bool upDownCube = false;
bool keyV = false;
bool keyR = false;
bool keyS = false;
bool keyU = false;
bool view3 = true;
int cameraRotate = 0;

typedef struct {
    float x, y, z;
    float nx, ny, nz;
} Vertex;
typedef struct {
    unsigned int v1, v2, v3;
} Face;
typedef struct {
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
    GLuint normalVBO;

    // init은 이제 멤버로서 자신의 VAO VBO EBO를 설정
    void init(const char* fileName)
    {
        read_obj_file(fileName, this);

        glm::vec3* normals = new glm::vec3[vertex_count];
        for (int i = 0; i < vertex_count; i++)
            normals[i] = glm::vec3(0);

        for (int i = 0; i < face_count; i++) {
            unsigned int v1 = faces[i].v1;
            unsigned int v2 = faces[i].v2;
            unsigned int v3 = faces[i].v3;

            glm::vec3 p1(vertices[v1].x, vertices[v1].y, vertices[v1].z);
            glm::vec3 p2(vertices[v2].x, vertices[v2].y, vertices[v2].z);
            glm::vec3 p3(vertices[v3].x, vertices[v3].y, vertices[v3].z);

            glm::vec3 n = glm::normalize(glm::cross(p2 - p1, p3 - p1));

            normals[v1] += n;
            normals[v2] += n;
            normals[v3] += n;
        }

        for (int i = 0; i < vertex_count; i++)
            normals[i] = glm::normalize(normals[i]);

        unsigned int* indices = (unsigned int*)malloc(face_count * 3 * sizeof(unsigned int));
        for (size_t i = 0; i < face_count; ++i)
        {
            indices[i * 3 + 0] = faces[i].v1;
            indices[i * 3 + 1] = faces[i].v2;
            indices[i * 3 + 2] = faces[i].v3;
        }

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(Vertex), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &normalVBO);
        glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
        glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(glm::vec3), normals, GL_STATIC_DRAW);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(2);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, face_count * 3 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

        delete[] normals;
        delete[] indices;
        glBindVertexArray(0);
    }
};

class CUBE : public OBJSHAPE
{
public:
    GLuint colorVBO = 0;

    float height = ranHeight(rd);
    float yPos = 30.0f;
    float speed = ranSpeed(rd);
    float upDownSpeed = ranSpeed(rd);
    float scale = 1.0f;
    int growing = randUD(rd);
    CUBE() {}
    ~CUBE() {}

    void setColor(float r, float g, float b)
    {
        Color* colored = new Color[vertex_count];
        for (size_t i = 0; i < vertex_count; ++i) {
            colored[i] = { vertices[i].x, vertices[i].y, vertices[i].z, r, g, b };
        }

        glBindVertexArray(VAO);

        glGenBuffers(1, &colorVBO);
        glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
        glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(Color), colored, GL_STATIC_DRAW);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Color), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        delete[] colored;
    }
};

class PYRAMID : public OBJSHAPE
{
public:
    PYRAMID() {}
    ~PYRAMID() {}
};

class SPHERE : public OBJSHAPE
{
public:
    GLuint colorVBO = 0;

    float w, h;
    int sx, sy;

    SPHERE() {}
    ~SPHERE() {}

    void setColor(float r, float g, float b)
    {
        Color* colored = new Color[vertex_count];
        for (size_t i = 0; i < vertex_count; ++i) {
            colored[i] = { vertices[i].x, vertices[i].y, vertices[i].z, r, g, b };
        }

        glBindVertexArray(VAO);

        glGenBuffers(1, &colorVBO);
        glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
        glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(Color), colored, GL_STATIC_DRAW);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Color), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        delete[] colored;
    }
};

class FLOOR {
public:
    void makeFloor() {
        GLfloat vertices[] = {
            -5.0f, 0.0f, -5.0f,  0.7f, 0.5f, 0.6f,
             5.0f, 0.0f, -5.0f,  0.7f, 0.5f, 0.6f,
             5.0f, 0.0f,  5.0f,  0.7f, 0.5f, 0.6f,
            -5.0f, 0.0f,  5.0f,  0.7f, 0.5f, 0.6f,
        };

        GLuint indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }
    void drawFloor() {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
private:
    GLuint VAO, VBO, EBO;
};

vector<CUBE> cube;
vector<vector<int>> mazeNum;
vector<vector<int>> maze;
SPHERE sphere;
FLOOR Floor;
GLuint floorVAO, floorVBO;

glm::vec3 sphereDir(0, 0, 1);
float eyeHeight = wSpace * 0.5f;

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
glm::vec3 cameraPos(xPos, yPos, zPos);
glm::vec3 target(0.0f, 0.0f, 0.0f);

glm::mat4 view = glm::lookAt(cameraPos, target, glm::vec3(0.0f, 1.0f, 0.0f));
glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 50.0f);
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
    glEnable(GL_NORMALIZE);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}
//void makeAxis()
//{
//    GLfloat vertices[] =
//    {
//        -5.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
//         5.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
//         0.0f, -3.0f, 0.0f, 0.0f, 1.0f, 0.0f,
//         0.0f,  3.0f, 0.0f, 0.0f, 1.0f, 0.0f,
//         0.0f, 0.0f, -20.0f, 0.0f, 0.0f, 1.0f,
//         0.0f, 0.0f,  20.0f, 0.0f, 0.0f, 1.0f
//    };
//
//    glGenVertexArrays(1, &AxisVAO);
//    glBindVertexArray(AxisVAO);
//    glGenBuffers(1, &AxisVBO);
//    glBindBuffer(GL_ARRAY_BUFFER, AxisVBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
//    glEnableVertexAttribArray(1);
//}

void drawCube(GLint modelLoc) {
    int i = 0, j = 0, idx = 0;
    float xCenter = -5.0f + (wSpace * 0.5f);
    float zCenter = -5.0f + (hSpace * 0.5f);
    for (auto& c : cube) {
        if (i >= height || j >= weight) break;
        if (keyR) {
            if (maze[i][j] == 1) {
                idx++;
                j++;
                if (j == weight) {
                    j = 0;
                    i++;
                }
                continue;
            }
        }
        glm::mat4 modelCube = glm::mat4(1.0f);
        modelCube = glm::translate(glm::mat4(1.0f), glm::vec3(xCenter + j * wSpace, c.yPos, zCenter + i * hSpace))
            * glm::scale(glm::mat4(1.0f), glm::vec3(wSpace, c.height * c.scale, hSpace));

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelCube));
        glBindVertexArray(c.VAO);
        glDrawElements(GL_TRIANGLES, c.face_count * 3, GL_UNSIGNED_INT, 0);
        idx++;
        j++;
        if (j == weight) {
            j = 0;
            i++;
        }
    }
}

void drawSphere(GLint modelLoc) {
    if (keyS) {
        glm::mat4 modelSphere = glm::mat4(1.0f);
        modelSphere = glm::translate(glm::mat4(1.0f), glm::vec3(sphere.w, wSpace / 2, sphere.h))
            * glm::scale(glm::mat4(1.0f), glm::vec3(wSpace / 2, wSpace / 2, hSpace / 2));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelSphere));

        glBindVertexArray(sphere.VAO);
        glDrawElements(GL_TRIANGLES, sphere.face_count * 3, GL_UNSIGNED_INT, 0);
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgram);

    if (proj) 
    {
        view = glm::lookAt(glm::vec3(xPos, yPos, zPos), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 50.0f);
    }
    else {
        float planeHalf = 10.0f;
        float aspect = 800.0f / 600.0f;
        projection = glm::ortho(-planeHalf * aspect, planeHalf * aspect, -planeHalf, planeHalf, 0.1f, 50.0f);
        view = glm::lookAt(glm::vec3(xPos, yPos, zPos), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    if (!view3) {
        glm::vec3 camPos = glm::vec3(sphere.w - 0.1f, eyeHeight + 1.5f, sphere.h - 0.1f);
        glm::vec3 target = camPos + sphereDir;
        projection = glm::perspective(glm::radians(90.0f), 800.0f / 600.0f, 0.1f, 50.0f);
        view = glm::lookAt(camPos, target, glm::vec3(0, 1, 0));
    }

    glUniform3f(glGetUniformLocation(shaderProgram, "lightPos"),
        10.0f, 10.0f, 10.0f);

    glUniform3f(glGetUniformLocation(shaderProgram, "viewPos"),
        xPos, yPos, zPos);

    // view, projection, model 행렬 전달
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glm::mat4 modelFloor = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelFloor));
    Floor.drawFloor();

    if (start)
    {
        drawCube(modelLoc);
        if (view3) drawSphere(modelLoc);
        // miniMap
        glViewport(600, 400, 200, 200);

        glm::mat4 miniView = glm::lookAt(
            glm::vec3(0.0f, 20.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, -1.0f)
        );
        float half = 8.0f;
        glm::mat4 miniProj = glm::ortho(-half, half, -half, half, 0.1f, 50.0f);

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(miniView));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(miniProj));

        drawCube(modelLoc);
        drawSphere(modelLoc);

        glViewport(0, 0, 800, 600);
    }
    glutSwapBuffers();
}

void Keyboard(unsigned char key, int x, int y) {
    switch (key){
    case 'o':
        proj = false;
        break;
    case 'p':
        proj = true;
        break;
    case 'z': 
        if (proj) zPos += 1.0f;
        break;
    case 'Z':
        if (proj) zPos -= 1.0f;
        break;
    case 'm': 
        upDownCube = true;
        glutTimerFunc(16, timer, 3);
        break;
    case 'M':
        upDownCube = false;
        break;
    case 'y':
        if (cameraRotate == 1 || cameraRotate == 2) cameraRotate = 0;
        else {
            cameraRotate = 1;
            glutTimerFunc(16, timer, 2);
        }
        break;
    case 'Y':
        if (cameraRotate == 1 || cameraRotate == 2) cameraRotate = 0;
        else {
            cameraRotate = 2;
            glutTimerFunc(16, timer, 2);
        }
        break;
    case 'r':
        if (start && !keyR) {
            keyR = true;
            makeMaze();
        }
        break;
    case 'v':
        if (upDownCube) {
            keyV = !keyV;
            if (keyV) {
                for (auto& c : cube) {
                    c.scale = 1 / c.height;
                    float targetY = c.height * c.scale * 0.5f;
                    c.yPos = targetY;
                }
            }
            else {
                if (upDownCube) glutTimerFunc(16, timer, 3);
            }
        }
        break;
    case 's':
        if (keyR) keyS = true;
        break;
    case '+':
        for (auto& c : cube) {
            c.upDownSpeed += 0.05f;
        }
        break;
    case '-':
        for (auto& c : cube) {
            if (c.upDownSpeed > 0.05f)
                c.upDownSpeed -= 0.05f;
        }
        break;
    case '1': 
        if (keyS) view3 = false;
        break;
    case '3':
        if (keyS) view3 = true;
        break;
    case 'u':
        if (keyS) keyU = !keyU;
        if (keyU) glutTimerFunc(40, timer, 4);
        break;
    case 'c':
        if (start) reset();
        break;
    case 'q':
        exit(0);
    }
    cout << key << "키 입력" << endl;
    glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
    if (keyS) {
        switch (key) {
        case GLUT_KEY_UP:
            if (sphere.sy - 1 >= 0 && maze[sphere.sy - 1][sphere.sx] == 1) {
                sphere.sy--;
                sphere.h -= hSpace;
                sphereDir = glm::vec3(0, 0, -1);
            }
            break;
        case GLUT_KEY_DOWN:
            if (sphere.sy + 1 < height && maze[sphere.sy + 1][sphere.sx] == 1) {
                sphere.sy++;
                sphere.h += hSpace;
                sphereDir = glm::vec3(0, 0, 1);
            }
            break;
        case GLUT_KEY_LEFT:
            if (sphere.sx - 1 >= 0 && maze[sphere.sy][sphere.sx - 1] == 1) {
                sphere.sx--;
                sphere.w -= wSpace;
                sphereDir = glm::vec3(-1, 0, 0);
            }
            break;
        case GLUT_KEY_RIGHT:
            if (sphere.sx + 1 < weight && maze[sphere.sy][sphere.sx + 1] == 1) {
                sphere.sx++;
                sphere.w += wSpace;
                sphereDir = glm::vec3(1, 0, 0);
            }
            break;
        }
        cout << key << "키 입력" << endl;
        glutPostRedisplay();
    }
}

float camAngle = 0.0f;
float radius = glm::length(cameraPos);
void timer(int value) {
    switch (value) {
    case 1: // 처음 큐브 내려올 때
    {
        bool allDone = true;
        for (auto& c : cube) {
            float delta = c.yPos - (c.height * 0.5f);
            if (delta > 0.0f) allDone = false;
            if (delta < c.speed) c.speed = delta;
            c.yPos -= c.speed;
            if (c.yPos <= c.height * 0.5f) c.yPos = c.height * 0.5f;
        }
        glutPostRedisplay();
        if (!allDone) glutTimerFunc(16, timer, 1);
        else {
            startEnd = true;
            break;
        }
        break;
    }
    case 2: // 카메라 y축 기준 회전
    {
        if (cameraRotate == 0) break;
        else if (cameraRotate == 1) {
            camAngle += 0.02f;
        }
        else if (cameraRotate == 2) {
            camAngle -= 0.02f;
        }
        xPos = cos(camAngle) * radius;
        zPos = sin(camAngle) * radius;
        glutPostRedisplay();
        glutTimerFunc(16, timer, 2);
        break;
    }
    case 3: // 육면체 위아래로 움직임
    {
        if (upDownCube && !keyV)
        {
            for (auto& c : cube) {
                if (c.growing == 1) {
                    c.scale += (c.upDownSpeed / 4);
                    if (c.scale >= 1.5f) c.growing = 2;
                }
                else {
                    c.scale -= (c.upDownSpeed / 4);
                    if (c.scale <= 0.5f) c.growing = 1;
                }
                float targetY = c.height * c.scale * 0.5f;
                c.yPos = targetY;
            }
            glutPostRedisplay();
            glutTimerFunc(16, timer, 3);
        }
        else
            break;
        break;
    }
    case 4:
        if (keyU) {
            sphere.setColor(ranColor(rd), ranColor(rd), ranColor(rd));
            glutPostRedisplay();
            glutTimerFunc(40, timer, 4);
        }
        break;
    }
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
}

void consoleOrder() {
    cout << "가로 길이와 세로 길이를 입력해주세요. q: 프로그램 종료" << '\n' << "* 길이 제한 : 5 ~ 25 * (벗어난다면 자동으로 최대 최소로 설정)" << '\n';
    cout << "입력 : ";
    getline(cin, order);
    if (order[0] == 'q') exit(0);
    ss.clear();
    ss.str(order);
    ss >> weight >> height;
    if (weight < 5) weight = 5;
    if (height < 5) height = 5;
    if (weight > 25) weight = 25;
    if (height > 25) height = 25;
    cout << weight << " " << height << endl;
    wSpace = 10.0f / weight;
    hSpace = 10.0f / height;

    mazeNum.assign(height, vector<int>(weight, 0));
    maze.assign(height, vector<int>(weight, 0));
}

void dfs(int x, int y, vector<vector<bool>>& visited, int weight, int height) {
    visited[y][x] = true;
    maze[y][x] = 1;

    int dx[4] = { 1, -1, 0, 0 };
    int dy[4] = { 0, 0, 1, -1 };

    vector<int> dir = { 0, 1, 2, 3 };
    shuffle(dir.begin(), dir.end(), std::mt19937{ std::random_device{}() });

    for (int d : dir) {
        int nx = x + dx[d] * 2;
        int ny = y + dy[d] * 2;

        if (nx >= 0 && nx < weight && ny >= 0 && ny < height && !visited[ny][nx]) {
            maze[y + dy[d]][x + dx[d]] = 1;
            maze[ny][nx] = 1;
            dfs(nx, ny, visited, weight, height);
        }
    }
}

void makeMaze() {
    vector<vector<bool>> visited(height, vector<bool>(weight, false));

    int num = 0;
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < weight; ++j) {
            mazeNum[i][j] = num;
            num++;
        }
    }
    int sx = weight / 2;
    int sy = height - 1;
    if (sx % 2 == 0) sx++;
    if (sy % 2 == 0) sy--;

    dfs(sx, sy, visited, weight, height);

    sphere.w = -5.0f + wSpace * 0.5f + sx * wSpace;
    sphere.h = -5.0f + hSpace * 0.5f + sy * hSpace;
    sphere.sx = sx;
    sphere.sy = sy;
}

void reset() {
    xPos = 0.0f;
    yPos = 15.0f;
    zPos = 20.0f;

    proj = true;
    start = false;
    startEnd = false;
    upDownCube = false;
    keyV = false;
    keyR = false;
    keyS = false;
    keyU = false;
    view3 = true;
    cameraRotate = 0;

    consoleOrder();
    cubeCnt = weight * height;

    cube.clear();
    cube.resize(cubeCnt);
    for (auto& c : cube) {
        c.init("cube.obj");
        c.setColor(1.0f, 0.75f, 0.8f);
        c.height = ranHeight(rd);
        c.yPos = 30.0f;
        c.speed = ranSpeed(rd);
        c.upDownSpeed = ranSpeed(rd);
        c.scale = 1.0f;
        c.growing = randUD(rd);
    }

    Floor.makeFloor();
    sphere.init("sphere.obj");
    sphere.setColor(0.6f, 0.3f, 0.8f);

    start = true;
    glutTimerFunc(16, timer, 1);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("computer Grapics");

    glewInit();

    make_shader();

    init();

    consoleOrder();
    cubeCnt = weight * height;
    cube.resize(cubeCnt);
    start = true;
    glutTimerFunc(16, timer, 1);

    for (auto& c : cube) {
        c.init("cube.obj");
        c.setColor(1.0f, 0.75f, 0.8f);
    }
    Floor.makeFloor();
    sphere.init("sphere.obj");
    sphere.setColor(0.6f, 0.3f, 0.8f);

    cout << "******** 키보드 명령어 ********" << endl;
    cout << "o/p: 투영 선택 (직각 투영 / 원근 투영)" << endl;
    cout << "z/Z: 원근 투영 시 z축으로 이동" << endl;
    cout << "m/M: 육면체들이 위 아래로 움직인다/멈춘다" << endl;
    cout << "y/Y: 카메라가 y축기준 양/음 방향으로 회전" << endl;
    cout << "r: 미로 생성" << endl;
    cout << "v: 육면체들 움직임이 멈추고 낮은 높이로 변화(다시 누르면 움직임 다시 시작)" << endl;
    cout << "s: 미로에서 객체 생성" << endl;
    cout << "→/←/↑/↓: 객체를 미로에서 앞/뒤/좌/우 이동" << endl;
    cout << "+/-: 육면체 움직이는 속도 증가/감소" << endl;
    cout << "1/3: 카메라 시점 1인칭/3인칭 변환" << endl;
    cout << "c: 초기화" << endl;
    cout << "q: 프로그램 종료" << endl;

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(Keyboard);
    glutSpecialFunc(specialKeys);
    
    glutMainLoop();
    return 0;
}