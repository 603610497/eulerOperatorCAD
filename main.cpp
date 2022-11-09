#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include <iostream>
#include "halfEdge.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#pragma comment(lib,"lib/glfw3.lib")

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

glm::vec3 cameraPos = glm::vec3(0, 0, 15.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = 0.0f;
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;



int main()
{

    std::cout << "input the number of holes: " << std::endl;
    int numOfHoles = 2;
    std::cin >> numOfHoles;

    auto ret0 = mvfs(point{ 0.f, 0.f, 0.f });
    auto loop = std::get<2>(ret0)->faces.front()->outerLoop;
    auto ret1 = mev(std::get<0>(ret0), loop, point{ 8.f, 0.f, 0.f });
    auto ret2 = mev(std::get<1>(ret1), loop, point{ 8.f, 3.f, 0.f });
    auto ret3 = mev(std::get<1>(ret2), loop, point{ 0.f, 3.f, 0.f });
    auto ret4 = mef(loop, std::get<1>(ret3), std::get<0>(ret0));
    
    for (int i = 0; i < numOfHoles; i++)
    {
        float dis= 8 / (float)(numOfHoles+numOfHoles-1+2);
        loop = std::get<1>(ret4)->outerLoop;
        auto ret5 = mev(std::get<0>(ret0), loop, point{ (2*i+1)*dis, 1.f, 0.f });
        auto ret6 = kemr(std::get<0>(ret0), std::get<1>(ret5), loop);
        loop = std::get<0>(ret6);
        auto ret7 = mev(std::get<1>(ret5), loop, point{ (2*i+2)*dis, 1.f, 0.f });
        auto ret8 = mev(std::get<1>(ret7), loop, point{ (2*i+2)*dis, 2.f, 0.f });
        auto ret9 = mev(std::get<1>(ret8), loop, point{ (2*i+1)*dis, 2.f, 0.f });
        auto ret10 = mef(loop, std::get<1>(ret9), std::get<1>(ret5));

        kfmrh(std::get<2>(ret0)->faces.front()->outerLoop, std::get<1>(ret10)->outerLoop);
    }
    sweepFace(std::get<1>(ret4), point{ 0.f, 0.f, 2.f });

    std::ofstream out;
    out.open("triangle.txt");
    for (auto i = std::get<2>(ret0)->faces.begin();i!= std::get<2>(ret0)->faces.end(); i++)
    {
        if ((*i)->innerLoops.size() > 0)
        {
            std::vector<point> outer;
            std::vector<std::vector<point>> allInnerloops;
            findCorner((*i)->outerLoop,outer);
            for (auto it = (*i)->innerLoops.begin(); it!= (*i)->innerLoops.end(); it++)
            {
                std::vector<point> inner1;
                findCorner(*it,inner1);
                allInnerloops.push_back(inner1);
            }
            
            std::sort(allInnerloops.begin(), allInnerloops.end(), [](const std::vector<point>& L1, const std::vector<point>& L2) {return L1[0].x < L2[0].x; });


            //out << outer[0].x << " " << outer[0].y << " " << outer[0].z << std::endl;
            for (int j = 0; j < allInnerloops.size()-1; j++)
            {
                out << allInnerloops[j][3].x << " " << allInnerloops[j][3].y << " " << allInnerloops[j][3].z << std::endl;
                out << allInnerloops[j][2].x << " " << allInnerloops[j][2].y << " " << allInnerloops[j][2].z << std::endl;
                out << allInnerloops[j + 1][0].x << " " << allInnerloops[j + 1][0].y << " " << allInnerloops[j + 1][0].z << std::endl;

                out << allInnerloops[j][2].x << " " << allInnerloops[j][2].y << " " << allInnerloops[j][2].z << std::endl;
                out << allInnerloops[j + 1][1].x << " " << allInnerloops[j + 1][1].y << " " << allInnerloops[j + 1][1].z << std::endl;
                out << allInnerloops[j + 1][0].x << " " << allInnerloops[j + 1][0].y << " " << allInnerloops[j + 1][0].z << std::endl;

                out << outer[0].x << " " << outer[0].y << " " << outer[0].z << std::endl;
                out << allInnerloops[j][0].x << " " << allInnerloops[j][0].y << " " << allInnerloops[j][0].z << std::endl;
                out << allInnerloops[j][3].x << " " << allInnerloops[j][3].y << " " << allInnerloops[j][3].z << std::endl;

                out << outer[0].x << " " << outer[0].y << " " << outer[0].z << std::endl;
                out << allInnerloops[j][3].x << " " << allInnerloops[j][3].y << " " << allInnerloops[j][3].z << std::endl;
                out << allInnerloops[j+1][0].x << " " << allInnerloops[j+1][0].y << " " << allInnerloops[j+1][0].z << std::endl;

                out << allInnerloops[j][1].x << " " << allInnerloops[j][1].y << " " << allInnerloops[j][1].z << std::endl;
                out << outer[1].x << " " << outer[1].y << " " << outer[1].z << std::endl;
                out << allInnerloops[j][2].x << " " << allInnerloops[j][2].y << " " << allInnerloops[j][2].z << std::endl;

                out << allInnerloops[j][2].x << " " << allInnerloops[j][2].y << " " << allInnerloops[j][2].z << std::endl;
                out << outer[1].x << " " << outer[1].y << " " << outer[1].z << std::endl;
                out << allInnerloops[j + 1][1].x << " " << allInnerloops[j + 1][1].y << " " << allInnerloops[j + 1][1].z << std::endl;
            }

            out << outer[0].x << " " << outer[0].y << " " << outer[0].z << std::endl;
            out << outer[1].x << " " << outer[1].y << " " << outer[1].z << std::endl;
            out << allInnerloops[0][0].x << " " << allInnerloops[0][0].y << " " << allInnerloops[0][0].z << std::endl;

            out << allInnerloops[0][0].x << " " << allInnerloops[0][0].y << " " << allInnerloops[0][0].z << std::endl;
            out << outer[1].x << " " << outer[1].y << " " << outer[1].z << std::endl;
            out << allInnerloops[0][1].x << " " << allInnerloops[0][1].y << " " << allInnerloops[0][1].z << std::endl;

            out << outer[0].x << " " << outer[0].y << " " << outer[0].z << std::endl;
            out << allInnerloops[allInnerloops.size()-1][0].x << " " << allInnerloops[allInnerloops.size() - 1][0].y << " " << allInnerloops[allInnerloops.size() - 1][0].z << std::endl;
            out << allInnerloops[allInnerloops.size() - 1][3].x << " " << allInnerloops[allInnerloops.size() - 1][3].y << " " << allInnerloops[allInnerloops.size() - 1][3].z << std::endl;

            out << outer[0].x << " " << outer[0].y << " " << outer[0].z << std::endl;
            out << allInnerloops[allInnerloops.size() - 1][3].x << " " << allInnerloops[allInnerloops.size() - 1][3].y << " " << allInnerloops[allInnerloops.size() - 1][3].z << std::endl;
            out << outer[3].x << " " << outer[3].y << " " << outer[3].z << std::endl;

            out << allInnerloops[allInnerloops.size() - 1][1].x << " " << allInnerloops[allInnerloops.size() - 1][1].y << " " << allInnerloops[allInnerloops.size() - 1][1].z << std::endl;
            out << outer[1].x << " " << outer[1].y << " " << outer[1].z << std::endl;
            out << allInnerloops[allInnerloops.size() - 1][2].x << " " << allInnerloops[allInnerloops.size() - 1][2].y << " " << allInnerloops[allInnerloops.size() - 1][2].z << std::endl;

            out << allInnerloops[allInnerloops.size() - 1][2].x << " " << allInnerloops[allInnerloops.size() - 1][2].y << " " << allInnerloops[allInnerloops.size() - 1][2].z << std::endl;
            out << outer[1].x << " " << outer[1].y << " " << outer[1].z << std::endl;
            out << outer[2].x << " " << outer[2].y << " " << outer[2].z << std::endl;

            out << outer[3].x << " " << outer[3].y << " " << outer[3].z << std::endl;
            out << allInnerloops[allInnerloops.size() - 1][3].x << " " << allInnerloops[allInnerloops.size() - 1][3].y << " " << allInnerloops[allInnerloops.size() - 1][3].z << std::endl;
            out << outer[2].x << " " << outer[2].y << " " << outer[2].z << std::endl;

            out << allInnerloops[allInnerloops.size() - 1][3].x << " " << allInnerloops[allInnerloops.size() - 1][3].y << " " << allInnerloops[allInnerloops.size() - 1][3].z << std::endl;
            out << allInnerloops[allInnerloops.size() - 1][2].x << " " << allInnerloops[allInnerloops.size() - 1][2].y << " " << allInnerloops[allInnerloops.size() - 1][2].z << std::endl;
            out << outer[2].x << " " << outer[2].y << " " << outer[2].z << std::endl;
        }
        else
        {
            auto hf = (*i)->outerLoop->leadingHalfEdge;
            float x1 = hf->vertex->coordinate.x;
            float y1 = hf->vertex->coordinate.y;
            float z1 = hf->vertex->coordinate.z;
            hf = hf->next;
            out << x1 << " " << y1 << " " << z1 << std::endl;
            float x2 = hf->vertex->coordinate.x;
            float y2 = hf->vertex->coordinate.y;
            float z2 = hf->vertex->coordinate.z;
            hf = hf->next;
            out << x2 << " " << y2 << " " << z2 << std::endl;
            float x3 = hf->vertex->coordinate.x;
            float y3 = hf->vertex->coordinate.y;
            float z3 = hf->vertex->coordinate.z;
            hf = hf->next;
            out << x3 << " " << y3 << " " << z3 << std::endl;
            out << x1 << " " << y1 << " " << z1 << std::endl;
            out << x3 << " " << y3 << " " << z3 << std::endl;
            float x4 = hf->vertex->coordinate.x;
            float y4 = hf->vertex->coordinate.y;
            float z4 = hf->vertex->coordinate.z;
            out << x4 << " " << y4 << " " << z4 << std::endl;
        }
    }
    out.close();


    std::vector<float> vp;
    std::ifstream infile("triangle.txt");
    std::string s1;
    float x, y, z;
    if (infile)
    {
        while (std::getline(infile, s1))
        {
            std::stringstream ss;
            if (s1.compare(0, 1, "#") == 0)
            {
                continue;
            }
            else
            {
                ss << s1;
                ss >> x;
                ss >> y;
                ss >> z;
            }
            vp.push_back(x);
            vp.push_back(y);
            vp.push_back(z);
        }
    }
    else
    {
        std::cout << "file open error" << std::endl;
    }

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);

    Shader ourShader("shader/vs.txt", "shader/fs.txt");

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vp.size(), &vp[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    ourShader.use();
    glm::mat4 projection = glm::perspective(glm::radians(35.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    ourShader.setMat4("projection", projection);
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();

        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        glm::mat4 view = glm::lookAt(cameraPos, -cameraPos + glm::vec3(0,0,0), cameraUp);
        ourShader.setMat4("view", view);

        glBindVertexArray(VAO);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-4,0,0));
        float angle = 20.0f * 0;
        model = glm::rotate(model, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", model);

        glDrawArrays(GL_TRIANGLES, 0, vp.size()/3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = static_cast<float>(2.5 * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(-cameraPos+glm::vec3(0,0,0), cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(-cameraPos + glm::vec3(0, 0, 0), cameraUp)) * cameraSpeed;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{

    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = -lastY + ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.001f; 
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 1.57)
        pitch = 1.57;
    if (pitch < -1.57)
        pitch = -1.57;

    glm::mat4 targetToOrigin = glm::mat4(1.0f);
    float distance = std::pow(std::pow(cameraPos.x, 2) + std::pow(cameraPos.y, 2) + std::pow(cameraPos.z, 2), 0.5);
    float x = distance * (float)std::cos(pitch) * (float)std::sin(-yaw);
    float y = distance * (float)std::sin(pitch);
    float z = distance * (float)std::cos(pitch) * (float)std::cos(-yaw);
    glm::vec3 p = targetToOrigin * glm::vec4(x, y, z, 1.0f);
    cameraPos = p;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}