#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"

#include <iostream>

// window
gps::Window myWindow;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;
glm::mat4 lightRotation;
glm::mat4 sunLightRotation;


// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;
glm::vec3 sunLight;

// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;
GLuint sunLightLocation;


// camera
gps::Camera myCamera(    
    glm::vec3(0.0f, 100.0f, 5.5f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));

GLfloat cameraSpeed = 3.0f;
GLfloat mouseSpeed = 0.1f;

GLboolean pressedKeys[1024];

// models
gps::Model3D universe;
gps::Model3D ozn;
gps::Model3D satelit;
gps::Model3D solar;


GLfloat angle;

float angleSatellite = 0.0f;
float angleDirectionalLight = 0.0f;
float anglePointLight = 0.0f;
float oznTranslate = 0.0f;

// shaders
gps::Shader myBasicShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;
gps::Shader depthMapShader;

std::vector<const GLchar*> faces;
gps::SkyBox mySkyBox;
gps::Shader skyboxShader;

bool rotateSatellite = false;
bool moveOzn = true;


float previousX = 400, previousY = 300;
float yaw = -90.0f, pitch;

bool startVisualisation = false;
float universeAngle = 0.0f;


GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		std::string error;
		switch (errorCode) {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_STACK_OVERFLOW:
                error = "STACK_OVERFLOW";
                break;
            case GL_STACK_UNDERFLOW:
                error = "STACK_UNDERFLOW";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
        }
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

	if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        } else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {   

    GLfloat X = xpos - previousX;
    GLfloat Y = previousY - ypos;

    previousX = xpos;
    previousY = ypos;

    X *= mouseSpeed;
    Y *= mouseSpeed;

    yaw += X;
    pitch += Y;

    myCamera.rotate(pitch, yaw);
}

void processMovement() {

    // CAMERA MOVEMENTS

    if (pressedKeys[GLFW_KEY_W]) { // MOVE CAMERA BACKWARD
        myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_S]) { // MOVE CAMERA FORWARD
        myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_A]) { // MOVE CAMERA LEFT
        myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_D]) { // MOVE CAMERA RIGHT
        myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_J]) { // MOVE DOWN
        myCamera.move(gps::MOVE_DOWN, cameraSpeed);
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    if (pressedKeys[GLFW_KEY_U]) { // MOVE UP
        myCamera.move(gps::MOVE_UP, cameraSpeed);
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    }

    // DIRECTIONAL LIGHT MOVEMENT

    if (pressedKeys[GLFW_KEY_K]) { // ROTATE DIR LIGHT LEFT
        angleDirectionalLight -= 1.0f;
    }

    if (pressedKeys[GLFW_KEY_L]) { // ROTATE DIR LIGHT RIGHT
        angleDirectionalLight += 1.0f;
    }

    // POINT LIGHT MOVEMENT
    if (pressedKeys[GLFW_KEY_Z]) { // ROTATE POINT LIGHT LEFT
        anglePointLight -= 1.0f;
    }

    if (pressedKeys[GLFW_KEY_X]) { // ROTATE POINT LIGHT RIGHT
        anglePointLight += 1.0f;
    }

    // SURFACE VIEW

    if (pressedKeys[GLFW_KEY_R]) { // POINT VIEW
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }

    if (pressedKeys[GLFW_KEY_T]) { // WIREFRAME VIEW
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    if (pressedKeys[GLFW_KEY_Y]) { // SOLID VIEW
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (pressedKeys[GLFW_KEY_H]) { // SMOOTH VIEW
        glDisable(GL_MULTISAMPLE);
        glEnable(GL_POINT_SMOOTH);
    }
    if (pressedKeys[GLFW_KEY_G]) { // POLYGONAL VIEW
        glDisable(GL_POINT_SMOOTH);
        glEnable(GL_MULTISAMPLE);
    }   

    // SATELLITE ROTATION

    if (pressedKeys[GLFW_KEY_Q]) { // SATELLITE ROTATE LEFT
        angleSatellite -= 1.0f;
        rotateSatellite = false;
    }

    if (pressedKeys[GLFW_KEY_E]) { // SATELLITE ROTATE RIGHT
        angleSatellite += 1.0f;
        rotateSatellite = true;
    }

    // OZN MOVEMENT

    if (pressedKeys[GLFW_KEY_M]) { // OZN MOVES TO RIGHT
        moveOzn = false; 
    }

    if (pressedKeys[GLFW_KEY_N]) { // OZN MOVES TO LEFT
        moveOzn = true; 
    }

    // UNIVERSE VISUALISATION

    if (pressedKeys[GLFW_KEY_P]) { // START VISUALISATION
        startVisualisation = true;
    }

    if (pressedKeys[GLFW_KEY_O]) { // STOP VISUALIZATION
        startVisualisation = false;
    }   
}

void initOpenGLWindow() {
    myWindow.Create(2048, 1024, "OpenGL Project Core");   
}

void setWindowCallbacks() {
	glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
    glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void initOpenGLState() {
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initModels() {
    universe.LoadModel("models/moon/univers6.obj");
    satelit.LoadModel("models/moon/satelitbun.obj");
    ozn.LoadModel("models/moon/ozn3.obj");
    solar.LoadModel("models/moon/solar2.obj");
}

void initShaders() {
	myBasicShader.loadShader(
        "shaders/basic.vert",
        "shaders/basic.frag");
    myBasicShader.useShaderProgram();

    lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
    lightShader.useShaderProgram();

    faces.push_back("textures/starfield/starfield_rt.tga");
    faces.push_back("textures/starfield/starfield_lf.tga");
    faces.push_back("textures/starfield/starfield_up.tga");
    faces.push_back("textures/starfield/starfield_dn.tga");
    faces.push_back("textures/starfield/starfield_bk.tga");
    faces.push_back("textures/starfield/starfield_ft.tga");

    mySkyBox.Load(faces);
    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");

    skyboxShader.useShaderProgram();
}

void initUniforms() {
    
    myBasicShader.useShaderProgram();

    // create model matrix
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

	// get view matrix for current camera
	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
	// send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix
    normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

	// create projection matrix
	projection = glm::perspective(glm::radians(45.0f),
                               (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
                               0.1f, 5000.0f);
	projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
	// send projection matrix to shader
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));	

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 1.0f, 0.0f);
    lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(angleDirectionalLight), glm::vec3(0.0f, 0.0f, 1.0f));
	lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

    // color of directional light
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

    // point light, on the sun
    sunLight = glm::vec3(200.0, 200.0, 20.0);
    sunLightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(angleDirectionalLight), glm::vec3(0.0f, 0.0f, 1.0f));
    sunLightLocation = glGetUniformLocation(myBasicShader.shaderProgram, "lightPosEye");
    glUniform3fv(sunLightLocation, 1, glm::value_ptr(sunLight));	

    lightShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
}

void oznMovement() {

    if (moveOzn) {// ozn moves to left
        oznTranslate += 0.25;    
    }
    else { // ozn moves to right
        oznTranslate -= 0.25;
    }
}

void satelliteRotation() {
    if (!rotateSatellite) {
        angleSatellite -= 1.0f;
    }
    else {
        angleSatellite += 1.0f;
    }
}

void viewUniverse() {

    if (startVisualisation) {
        universeAngle += 0.2f;
        myCamera.startVisualization(universeAngle);
        
    }
}

void drawUniverse(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    // SATELLITE DRAW

    satelliteRotation();    
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angleSatellite), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    satelit.Draw(shader);

    // OZN DRAW
    
    oznMovement();   
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(oznTranslate, 0, 0));    
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    ozn.Draw(shader);

    // UNIVERSE DRAW

    model = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    universe.Draw(shader);   

    // SOLAR PANEL DRAW
    model = glm::mat4(1.0f);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    solar.Draw(shader);
}

void renderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    
    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);

    viewUniverse();

    myBasicShader.useShaderProgram();

    view = myCamera.getViewMatrix();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    sunLightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(anglePointLight), glm::vec3(0.0f, 0.0f, 1.0f));
    lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(angleDirectionalLight), glm::vec3(0.0f, 0.0f, 1.0f));
    glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));
    glUniform3fv(sunLightLocation, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * sunLightRotation)) * sunLight));

    drawUniverse(myBasicShader);

    mySkyBox.Draw(skyboxShader, view, projection);  
}

void cleanup() {
    myWindow.Delete();
}

int main(int argc, const char * argv[]) {

    try {
        initOpenGLWindow();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
	initModels();
	initShaders();
	initUniforms();
    setWindowCallbacks();

	glCheckError();
	while (!glfwWindowShouldClose(myWindow.getWindow())) {
        processMovement();
	    renderScene();

		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());

		glCheckError();
	}

	cleanup();

    return EXIT_SUCCESS;
}
