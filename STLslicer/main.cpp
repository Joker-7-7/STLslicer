#include <glad/glad.h>
// suppress documentation warning from glfw3.h
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"
#include <GLFW/glfw3.h>
#pragma clang diagnostic pop

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <vector>

#include "shader.h"
#include "vertex.h"
#include "printer.h"
#include "util.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define EPSILON 0.0001
GLfloat layer;

const unsigned int SCR_WIDTH = 500;
const unsigned int SCR_HEIGHT = int(SCR_WIDTH * float(printer_resolution.y) / float(printer_resolution.x));

GLfloat lastX = SCR_WIDTH / 2.0;
GLfloat lastY = SCR_HEIGHT / 2.0;
GLfloat yaw = -90.0f;	// Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
GLfloat pitch = 0.0f;

float zoom = 0.05;
float fov = 45.0f;
float lastFrame = 0.0f;
float currentFrame = 0.0f;
int fpscnt = 0;
float fpsTime = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void loadMesh(std::string stl, size_t* p_num_of_verts, std::vector<GLfloat>* p_bounds, GLfloat* p_total_thickness);
void drawObject(Shader* shader);
void drawPlaneFirst(Shader* shader);
void drawPlaneSecond(Shader* shader);
void drawXaxis(Shader* shader);


GLuint VAO, vertVBO, planeVAOfirst, planeVBOfirst, planeVAOsecond, planeVBOsecond;
GLuint XaxisVAO, XaxisVertVBO;

size_t num_of_verts;
std::vector<GLfloat> bounds; // bounds = {xmin, xmax, ymin, ymax, zmin, zmax}
GLfloat total_thickness;

glm::vec3 camera_position = glm::vec3(0.0f, 0.0f, 3.0f);     // Положение камеры
glm::vec3 camera_front = glm::vec3(0.0f, 0.0f, -1.0f);       // Направление камеры
glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);           // Вектор камеры
glm::vec3 camera_right = glm::cross(camera_front, camera_up);

int main(int argc, char* argv[])
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "STL Slicer", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //glfwSetCursorPosCallback(window, mouse_callback);
    //glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader sliceShader("slice.vert", "slice.frag");
    std::string fileSTL = "3dmodel.stl";
    loadMesh(fileSTL, &num_of_verts, &bounds, &total_thickness);
    //prepareSlice();
    layer = 5;

    int i = 0;
    GLfloat height = 1.0f; // slice height
    GLdouble eqn[4] = { 10.0, -10.0, 10.0, 10.0 };
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.34f, 0.57f, 1.0f);

        sliceShader.use();

        glm::mat4 model(1);
        model = glm::scale(model, glm::vec3(zoom, zoom, zoom));
        int model_location = glGetUniformLocation(sliceShader.ID, "model"); // Получаем позицию параметра в шейдере
        glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));// Записываем значение параметра

        glm::mat4 view(1);// просмотр матрицы, преобразование мировой координаты в систему координат наблюдения
        view = glm::lookAt(camera_position, camera_position + camera_front, camera_up);
        int view_location = glGetUniformLocation(sliceShader.ID, "view");
        glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(view));


        glm::mat4 proj(1);
        //proj = glm::perspective(fov, (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT, 0.1f, 100.0f);
        int proj_location = glGetUniformLocation(sliceShader.ID, "proj");
        glUniformMatrix4fv(proj_location, 1, GL_FALSE, glm::value_ptr(proj));

       drawPlaneFirst(&sliceShader);
       drawPlaneSecond(&sliceShader);
       drawObject(&sliceShader);

        //glEnable(GL_CLIP_PLANE0);
        //glClipPlane(GL_CLIP_PLANE0, eqn);
        //glColor3d(1, 0, 0);

        drawObject(&sliceShader);



        //// draw sphere           
   

        //drawXaxis(&sliceShader);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &vertVBO);
    glDeleteVertexArrays(1, &planeVAOfirst);
    glDeleteBuffers(1, &planeVBOfirst);
    glDeleteVertexArrays(1, &planeVAOsecond);
    glDeleteBuffers(1, &planeVBOsecond);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------


// load all the vertices and normal into GPU memory, and return bound box
// ----------------------------------------------------------------------
void loadMesh(std::string stl, size_t* p_num_of_verts, std::vector<GLfloat>* p_bounds, GLfloat* p_total_thickness)
{
    std::vector<Vertex> vertices;
    std::vector<Normal> normals;
    std::vector<Triangle> objectTriangles;
    getVertices(&vertices, &normals, stl);
    createTriangles(objectTriangles, vertices, normals);


    float coordZfirst = 0.0;
    std::cout << "Write Z coord value for first plane: ";
    std::cin >> coordZfirst;
    GLfloat maskVertfirst[] = {
        // Первый треугольник
         30.0f,  30.0f, coordZfirst,  // Верхний правый угол
         30.0f, -30.0f, coordZfirst,  // Нижний правый угол
        -30.0f,  30.0f, coordZfirst,  // Верхний левый угол
        // Второй треугольник
         30.0f, -30.0f, coordZfirst,  // Нижний правый угол
        -30.0f, -30.0f, coordZfirst,  // Нижний левый угол
        -30.0f,  30.0f, coordZfirst   // Верхний левый угол
    };


    float coordZsecond = 0.0;
    std::cout << "Write Z coord value for second plane: ";
    std::cin >> coordZsecond;
    GLfloat maskVertsecond[] = {
        // Первый треугольник
         30.0f,  30.0f, coordZsecond,  // Верхний правый угол
         30.0f, -30.0f, coordZsecond,  // Нижний правый угол
        -30.0f,  30.0f, coordZsecond,  // Верхний левый угол
        // Второй треугольник
         30.0f, -30.0f, coordZsecond,  // Нижний правый угол
        -30.0f, -30.0f, coordZsecond,  // Нижний левый угол
        -30.0f,  30.0f, coordZsecond   // Верхний левый угол
    };


    Plane planeFirst(Vertex(30.0, 30.0, coordZfirst), Vertex(30.0f, -30.0f, coordZfirst), Vertex(-30.0f, 30.0f, coordZfirst));
    Plane planeSecond(Vertex(30.0, 30.0, coordZsecond), Vertex(30.0f, -30.0f, coordZsecond), Vertex(-30.0f, 30.0f, coordZsecond));
    std::vector <Vertex> verts;
    sliceSolid(objectTriangles, planeFirst, planeSecond, verts);

    *p_num_of_verts = size_t(verts.size());
    *p_bounds = find_min_max(verts);
    *p_total_thickness = (*p_bounds)[5] - (*p_bounds)[4];

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &vertVBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, vertVBO);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * 3 * sizeof(GLfloat), verts.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);


    glGenVertexArrays(1, &planeVAOfirst);
    glGenBuffers(1, &planeVBOfirst);
    glBindVertexArray(planeVAOfirst);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBOfirst);
    glBufferData(GL_ARRAY_BUFFER, sizeof(maskVertfirst), &maskVertfirst, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    glGenVertexArrays(1, &planeVAOsecond);
    glGenBuffers(1, &planeVBOsecond);
    glBindVertexArray(planeVAOsecond);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBOsecond);
    glBufferData(GL_ARRAY_BUFFER, sizeof(maskVertsecond), &maskVertsecond, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    //GLfloat VertXaxis[] = {
    //    -1.0f, -1.0f, 0.0f,  // Верхний правый угол
    //     1.0f, -1.0f, 0.0f  // Нижний правый угол
    //};
    //glGenVertexArrays(1, &XaxisVAO);
    //glGenBuffers(1, &XaxisVertVBO);
    //glBindVertexArray(XaxisVAO);
    //glBindBuffer(GL_ARRAY_BUFFER, XaxisVertVBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(VertXaxis), &VertXaxis, GL_STATIC_DRAW);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
    //glEnableVertexAttribArray(0);
    //glBindVertexArray(0);


    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);    //frame mode
}

void drawObject(Shader* shader)
{
    GLint vertexColorLocation = glGetUniformLocation(shader->ID, "OurColor");
    glUniform4f(vertexColorLocation, 0.0f, 1.0, 0.0f, 1.0f);


    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, num_of_verts);
    //glPointSize(2);
    //glDrawArrays(GL_POINTS, 0, num_of_verts);
    glBindVertexArray(0);
}
void drawPlaneFirst(Shader* shader)
{
    GLint vertexColorLocation = glGetUniformLocation(shader->ID, "OurColor");
    glUniform4f(vertexColorLocation, 1.0f, 0.0, 0.0f, 1.0f);


    //   draw plane

    glBindVertexArray(planeVAOfirst);
    glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisable(GL_STENCIL_TEST);

}

void drawPlaneSecond(Shader* shader)
{
    GLint vertexColorLocation = glGetUniformLocation(shader->ID, "OurColor");
    glUniform4f(vertexColorLocation, 1.0f, 1.0, 0.0f, 1.0f);

    //   draw plane

    glBindVertexArray(planeVAOsecond);
    glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisable(GL_STENCIL_TEST);

}

void drawXaxis(Shader* shader)
{
    GLint vertexColorLocation = glGetUniformLocation(shader->ID, "OurColor");
    glUniform4f(vertexColorLocation, 0.0f, 0.0, 1.0f, 1.0f);


    //   draw line
    glBindVertexArray(XaxisVAO);
    glDrawArrays(GL_LINE, 0, 2);
    glPointSize(2);
    glBindVertexArray(0);

}


void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);


    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        zoom += 0.0001;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        zoom -= 0.0001;

    //float delta = 0.05f;
    //if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    //    camera_position += camera_front * delta;
    //if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    //    camera_position -= camera_front * delta;
    //if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    //    camera_position -= camera_right * delta;
    //if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    //    camera_position += camera_right * delta;

    float deltaTime = 0.01f;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera_front += camera_up * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera_front -= camera_up * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera_front -= camera_right * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera_front += camera_right * deltaTime;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to left
    lastX = xpos;
    lastY = ypos;

    GLfloat sensitivity = 0.05;	// Change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camera_front = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{

    if (fov >= 1.0f && fov <= 45.0f)
        fov -= yoffset;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 45.0f)
        fov = 45.0f;
}
