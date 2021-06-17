#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;
bool notStarted = true;

ShaderProgram program;
glm::mat4 viewMatrix, projectionMatrix, modelMatrix, ballMatrix, leftMatrix, rightMatrix;

//initialize left player
glm::vec3 left_position = glm::vec3(0, 0, 0);
glm::vec3 left_movement = glm::vec3(0, 0, 0);

//initialize right player
glm::vec3 right_position = glm::vec3(0, 0, 0);
glm::vec3 right_movement = glm::vec3(0, 0, 0);


// initialize ball
glm::vec3 ball_position = glm::vec3(0, 0, 0);
glm::vec3 ball_movement = glm::vec3(0, 0, 0);
float ball_speed = 5.0f;

glm::vec3 ball_size = glm::vec3(0.30f, 0.30f, 1.0f);
float ball_h =  ball_size.y;
float ball_w = ball_size.x;

//initialize paddle info
glm::vec3 paddle_size = glm::vec3(0.30f, 1.2f, 1.0f);
float paddle_h = paddle_size.y;
float paddle_w = paddle_size.x;
float paddle_speed = 6.5f;


GLuint ballTextureID;
GLuint leftTextureID;
GLuint rightTextureID;


GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    stbi_image_free(image);
    return textureID;
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Ellie Haber Project 2: Pong!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    

    glUseProgram(program.programID);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);
    // Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //textures
    ballTextureID = LoadTexture("jump.png");
    leftTextureID = LoadTexture("rect.png");
    rightTextureID = LoadTexture("rect.png");

    
    }

bool reachTop(float y, float height) {
    return !((y + (height / 2.0f)) <= 3.75f);
}

bool reachBottom(float y, float height) {
    return !((y - (height / 2.0f)) >= -3.75f);
}



void ProcessInput() {

    left_movement= right_movement = glm::vec3(0);

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            gameIsRunning = false;
            break;
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);
    //start game by moving ball
    if (keys[SDL_SCANCODE_SPACE]) {
            ball_movement = glm::vec3(1, 0.5, 0);
    }
    //right paddle
    if (keys[SDL_SCANCODE_UP] && !reachTop(right_position.y, paddle_h)) {
        right_movement.y = 1.0f;
    }
    else if (keys[SDL_SCANCODE_DOWN] && !reachBottom(right_position.y, paddle_h)) {
        right_movement.y = -1.0f;
    }
    //left paddle
    if (keys[SDL_SCANCODE_W] && !reachTop(left_position.y, paddle_h)) {
        left_movement.y = 1.0f;
    }
    else if (keys[SDL_SCANCODE_S] && !reachBottom(left_position.y, paddle_h)) {
        left_movement.y = -1.0f;
    }

    if (glm::length(ball_movement) > 1.0f) ball_movement = glm::normalize(ball_movement);
}


void updateLeft(float deltaTime) {
    left_position += left_movement * paddle_speed * deltaTime;
    leftMatrix = glm::translate(modelMatrix, glm::vec3(-4.8f, 0.0f, 0.0f));
    leftMatrix = glm::translate(leftMatrix, left_position);
    leftMatrix = glm::scale(leftMatrix, paddle_size);
}

void updateRight(float deltaTime) {
    right_position += right_movement * paddle_speed * deltaTime;
    rightMatrix = glm::translate(modelMatrix, glm::vec3(4.8f, 0.0f, 0.0f));
    rightMatrix = glm::translate(rightMatrix, right_position);
    rightMatrix = glm::scale(rightMatrix, paddle_size);
}

    

void updateBall(float deltaTime){
    //right paddle- x distance
    float ballX = ((ball_w + 0.2f + paddle_w) / 2.0f);
    float rightX = fabs(ball_position.x - right_position.x - 4.8f);
    float right_x = rightX - ballX;
    //right paddle- y distance
    float ballY = ((ball_h + 0.2f + paddle_h) / 2.0f);
    float rightY = fabs(ball_position.y - right_position.y);
    float right_y = rightY - ballY;
    //left paddle- x distance
    float leftX = fabs(ball_position.x - left_position.x + 4.8f);
    float left_x = leftX - ballX;
    //left paddle- y distance
    float leftY = fabs(ball_position.y - left_position.y);
    float left_y = leftY - ballY;
    
    if (glm::length(ball_movement) > 1.0f) ball_movement = glm::normalize(ball_movement);

    if (reachTop(ball_position.y, ball_h)){
        ball_movement.y = -1.0f * fabs(ball_movement.y);
    }
    else if (reachBottom(ball_position.y, ball_h)) {
        ball_movement.y = fabs(ball_movement.y);
    }

    if (left_x < 0 && left_y < 0) {
        if (ball_position.x - ball_w / 2.0f < left_position.x + paddle_w / 2.0f - 4.8f) {
            ball_movement.y = -1.0f * ball_movement.y;
        }
        if (ball_position.x < -4.80f) ball_movement.x = -1.0f * fabs(ball_movement.x);
        else ball_movement.x = fabs(ball_movement.x);
    }
    else if ((right_x < 0 && right_y < 0)) {
        if (ball_position.x + ball_w / 2.0f > right_position.x - paddle_w / 2.0f + 4.8f) {
           ball_movement.y = -1.0f * ball_movement.y;
        }
        if (ball_position.x > 4.80f) ball_movement.x = fabs(ball_movement.x);
        else ball_movement.x = -1.0f * fabs(ball_movement.x);
    }
    
    
    if (glm::length(ball_movement) > 1.0f) ball_movement = glm::normalize(ball_movement);
    ball_position += ball_movement * ball_speed * deltaTime;
    ballMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
    ballMatrix = glm::translate(ballMatrix, ball_position);
    ballMatrix = glm::scale(ballMatrix, ball_size);
}

float lastTicks = 0.0f;

void Update() {
    if (ball_position.x < -4.8f || ball_position.x > 4.8f) {
        left_movement = glm::vec3(0.0f, 0.0f, 0.0f);
        right_movement = glm::vec3(0.0f, 0.0f, 0.0f);
        ball_movement = glm::vec3(0.0f, 0.0f, 0.0f);
    }
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;

    updateLeft(deltaTime);
    updateRight(deltaTime);
    updateBall(deltaTime);
  
    }

void drawBall() {
    program.SetModelMatrix(ballMatrix);
    glBindTexture(GL_TEXTURE_2D, ballTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void drawLeft() {
    program.SetModelMatrix(leftMatrix);
    glBindTexture(GL_TEXTURE_2D, leftTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

}

void drawRight() {
    program.SetModelMatrix(rightMatrix);
    glBindTexture(GL_TEXTURE_2D, rightTextureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);

}


void Render() {
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

    glClear(GL_COLOR_BUFFER_BIT);
    
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);

    drawBall();
    drawLeft();
    drawRight();

    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);

    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();

    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }

    Shutdown();
    return 0;
}
