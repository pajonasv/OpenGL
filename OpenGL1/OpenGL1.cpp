
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include<math.h>
#include"Shader.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

void spinY(float* vertices, float* newVertices, double angle, int vertexN);
void spinX(float* vertices, float* newVertices, double angle, int vertexN);
void spinZ(float* vertices, float* newVertices, double angle, int vertexN);

struct Polygon {
    float* vertices;
};

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //this next line is for mac users
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800,600,"Hi",NULL,NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f   // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3,  // second triangle
        4, 5, 7,
        5, 6, 7
        };
    
    Shader* shader = new Shader("vertexShader.txt","fragmentShader.txt");
    shader->use();
    unsigned int VBO;
    unsigned int VAO;
    unsigned int EBO;

    glGenBuffers(1, &EBO);
    glGenBuffers(1, &VBO); //setting the vertex buffer object id to VBO
    glGenVertexArrays(1, &VAO);

    // ..:: Initialization code :: ..
    // 1. bind Vertex Array Object
    glBindVertexArray(VAO);
    // 2. copy our vertices array in a vertex buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 3. copy our index array in a element buffer for OpenGL to use
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // 4. then set the vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    
    
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    int width, height, nrChannels;
    unsigned char* data = stbi_load("natewantstobattle.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);



    double angle = 0.0;
    while (!glfwWindowShouldClose(window)) {
        //input
        processInput(window);


        //rendering commands
        glClear(GL_COLOR_BUFFER_BIT);

        angle += 0.001;
        



        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),vertices, GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
     

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        //check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

void spinY(float *vertices, float* newVertices, double angle, int vertexN) {
    /*
        |a, b| |x| = |xa + yb| 
        |c, d| |y| = |xc + yd|
    
    */
    float matrix[][3] = {
        {cos(angle),0,sin(angle)},
        {0, 1, 0},
        {-sin(angle), 0, cos(angle)}
    };
      
    for (int i = 0; i < vertexN*8; i+=8) {
        newVertices[i+0] = (vertices[0 + i] * matrix[0][0]) + (vertices[1 + i] * matrix[0][1]) + (vertices[2 + i] * matrix[0][2]);
        newVertices[i+1] = (vertices[0 + i] * matrix[1][0]) + (vertices[1 + i] * matrix[1][1]) + (vertices[2 + i] * matrix[1][2]);
        newVertices[i+2] = (vertices[0 + i] * matrix[2][0]) + (vertices[1 + i] * matrix[2][1]) + (vertices[2 + i] * matrix[2][2]);
        newVertices[i + 3] = vertices[3 + i];
        newVertices[i + 4] = vertices[4 + i];
        newVertices[i + 5] = vertices[5 + i];
        newVertices[i + 6] = vertices[6 + i];
        newVertices[i + 7] = vertices[7 + i];
    }

    
}
void spinX(float* vertices, float* newVertices, double angle, int vertexN) {
    /*
        |a, b| |x| = |xa - yb|
        |c, d| |y| = |xc + yd|

    */
    float matrix[][3] = {
        {1,0,0},
        {0, cos(angle), -sin(angle)},
        {0, sin(angle), cos(angle)}
    };

    for (int i = 0; i < vertexN * 8; i += 8) {
        newVertices[i + 0] = (vertices[0 + i] * matrix[0][0]) + (vertices[1 + i] * matrix[0][1]) + (vertices[2 + i] * matrix[0][2]);
        newVertices[i + 1] = (vertices[0 + i] * matrix[1][0]) + (vertices[1 + i] * matrix[1][1]) + (vertices[2 + i] * matrix[1][2]);
        newVertices[i + 2] = (vertices[0 + i] * matrix[2][0]) + (vertices[1 + i] * matrix[2][1]) + (vertices[2 + i] * matrix[2][2]);
        newVertices[i + 3] = vertices[3 + i];
        newVertices[i + 4] = vertices[4 + i];
        newVertices[i + 5] = vertices[5 + i];
        newVertices[i + 6] = vertices[6 + i];
        newVertices[i + 7] = vertices[7 + i];
    }

}
void spinZ(float* vertices, float* newVertices, double angle, int vertexN) {
    /*
        |a, b| |x| = |xa - yb|
        |c, d| |y| = |xc + yd|

    */
    float matrix[][3] = {
        {cos(angle),-sin(angle),0},
        {sin(angle), cos(angle), 0},
        {0, 0, 1}
    };

    for (int i = 0; i < vertexN * 8; i += 8) {
        newVertices[i + 0] = (vertices[0 + i] * matrix[0][0]) + (vertices[1 + i] * matrix[0][1]) + (vertices[2 + i] * matrix[0][2]);
        newVertices[i + 1] = (vertices[0 + i] * matrix[1][0]) + (vertices[1 + i] * matrix[1][1]) + (vertices[2 + i] * matrix[1][2]);
        newVertices[i + 2] = (vertices[0 + i] * matrix[2][0]) + (vertices[1 + i] * matrix[2][1]) + (vertices[2 + i] * matrix[2][2]);
        newVertices[i + 3] = vertices[3 + i];
        newVertices[i + 4] = vertices[4 + i];
        newVertices[i + 5] = vertices[5 + i];
        newVertices[i + 6] = vertices[6 + i];
        newVertices[i + 7] = vertices[7 + i];
    }


}

//for when screen is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0,0,width,height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}