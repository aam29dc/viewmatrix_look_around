#include <SOIL2.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <stack>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Utils.h"

using namespace std;

int main(void) {
    if (!glfwInit()) { exit(EXIT_FAILURE); }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "Untitled", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
    glfwSwapInterval(1);

    //glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    GLuint program = Utils::createShaderProgram("C:/Users/aam29/source/repos/gl/gl/vertShader.glsl",
        "C:/Users/aam29/source/repos/gl/gl/fragShader.glsl");

    GLuint vao;
    GLuint vbo[2];

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLfloat vertices[] = { -1,1,-1,0,0,    -1,-1,-1,1,0,    1,-1,-1,1,1,    1,1,-1,0,1,    1,1,1,0,0,    1,-1,1,1,0,    -1,-1,1,1,1,    -1,1,1,0,1 };
    GLuint indices[] = { 0,1,2,    0,2,3,    3,2,5,    3,5,4,    6,5,4,    6,4,7,    7,6,1,    7,1,0,    7,0,3,    7,3,4,    6,1,2,    6,2,5};

    glGenBuffers(2, vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glm::mat4 model, rot, view;

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    float aspect = (float)width / (float)height;
    glm::mat4 projo = glm::perspective(90.0f, (float)(width / height), 0.1f, 100.0f);

    GLint rloc, mloc, vloc, ploc;

    rloc = glGetUniformLocation(program, "rot");
    mloc = glGetUniformLocation(program, "model");
    vloc = glGetUniformLocation(program, "view");
    ploc = glGetUniformLocation(program, "proj");

    glUseProgram(program);

    glUniformMatrix4fv(vloc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(ploc, 1, GL_FALSE, glm::value_ptr(projo));

    GLuint textobj;

    glGenTextures(1, &textobj);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textobj);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    int twidth, theight, nrChannels = 0;
    unsigned char* data = SOIL_load_image("C:/Users/aam29/source/repos/gl/gl/brick.jpg", &twidth, &theight, &nrChannels, 0);    if (data == nullptr) printf("error reading img");

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(data);
    glUniform1i(glGetUniformLocation(program, "img"), 0);

    GLuint textobj2;

    glGenTextures(1, &textobj2);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textobj2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    data = SOIL_load_image("C:/Users/aam29/source/repos/gl/gl/white.jpg", &twidth, &theight, &nrChannels, 0);    if (data == nullptr) printf("error reading img");

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, twidth, theight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(data);
    glUniform1i(glGetUniformLocation(program, "img2"), 1);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    float x = 0.25;

    // cam look around
    double lastx, lasty, posx = 0;
    double posy = 0;
    double ltime, ctime = 0;
    double dtime = 0;
    double yaw = glm::radians(-180.0);
    double pitch = 0;
    double offx, offy = 0;
    double ms = 0.01;
    glm::vec3 pos, direction, right, up;
    pos = glm::vec3(0, 0, -1);

    // the rate at which display() is called is referred to as the frame rate
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.5, 0.5, 0.5, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(vao);
        glUseProgram(program);

        ltime = ctime;
        ctime = glfwGetTime();
        dtime = ctime - ltime;

        lastx = posx; lasty = posy;

        //controls
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            pos += (direction * (float)dtime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            pos -= (direction * (float)dtime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            pos -= (glm::cross(direction, glm::vec3(0,1,0)) * (float)dtime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            pos += (glm::cross(direction, glm::vec3(0, 1, 0)) * (float)dtime);

        // cam look around
        glfwGetCursorPos(window, &posx, &posy);
        //glfwSetCursorPos(window, height / 2, width / 2);
        offx = ms*(posx - lastx);
        offy = ms*(lasty - posy);
        yaw += offx; pitch += offy;

        if (pitch >= glm::radians(89.)) pitch = glm::radians(89.);
        if (pitch <= glm::radians(-89.)) pitch = glm::radians(-89.);
        
        direction = glm::normalize(glm::vec3(cos(yaw)*cos(pitch), sin(pitch), cos(pitch)*sin(yaw)));
        right = glm::normalize(glm::cross(direction, glm::vec3(0, 1, 0)));
        up = glm::normalize(glm::cross(direction, right));

        view = glm::lookAt(pos, pos + direction, glm::vec3(0, 1, 0));
        glUniformMatrix4fv(vloc, 1, GL_FALSE, glm::value_ptr(view));
        //

        rot = glm::rotate(glm::mat4(1.0), x, glm::vec3(1.0, 0.75, 0.25));
        glUniformMatrix4fv(rloc, 1, GL_FALSE, glm::value_ptr(rot));

        glfwGetFramebufferSize(window, &width, &height);
        aspect = (float)(width / height);
        glViewport(0, 0, width, height);
   
        //draw cube
        model = glm::scale(glm::mat4(1.0), glm::vec3(0.25, 0.25, 0.25))
            * glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, -1.0)) * rot;
        glUniformMatrix4fv(mloc, 1, GL_FALSE, glm::value_ptr(model));

        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textobj2);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textobj);
        //glUniform1i(glGetUniformLocation(program, "img"), 0);

        glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteTextures(1, &textobj);
    glDeleteTextures(1, &textobj2);
    glDeleteBuffers(4, vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(program);

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}