#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <vector>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Entity.h"

#define PLATFORM_COUNT 28
struct GameState {
    Entity *player;
    Entity *platforms;
};

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

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
    displayWindow = SDL_CreateWindow("Ellie Haber's Project 3: Adventures of a Traveling Robot", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
   
    // Initialize Game Objects
    
    // Initialize Player
    /*state.player = new Entity();
    state.player->position = glm::vec3(0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);
    state.player->speed = 2.0f;
    state.player->textureID = LoadTexture("george_0.png");
    
    state.player->animRight = new int[4] {3, 7, 11, 15};
    state.player->animLeft = new int[4] {1, 5, 9, 13};
    state.player->animUp = new int[4] {2, 6, 10, 14};
    state.player->animDown = new int[4] {0, 4, 8, 12};

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;
    
    state.player->height = 0.8f;
    state.player->width = 0.8f;

    state.player->jumpPower = 5.0f;*/
    
    //Initialize robot
    state.player = new Entity();
    state.player->position = glm::vec3(0,4.5,0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0,-.82f, 0);
    state.player->speed = 1.3;
    state.player->textureID = LoadTexture("platformChar_idle.png");
    
    state.player->height = 0.55f;
    state.player->width = 0.55f;
    
    //Initialize tiles
    state.platforms = new Entity[PLATFORM_COUNT];
    GLuint platformTextureID = LoadTexture("platformPack_tile007.png");
    GLuint platformBaseTextureID = LoadTexture("platformPack_tile001.png");


    //adds wall barrier on left side of screen
    float shift = 0;
    for (int i = 0; i <= 7; i++) {
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3(-4.5, -3.5 - shift, 0);
        shift -= 1;
    }
     
    //adds wall barrier on right side of screen
    shift = 0;
    for (int i = 8; i <= 15; i++) {
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3(4.5, -3.5 - shift, 0);
        shift -= 1;
    }

    //adds ground level blocks
    shift = 0;
    for (int i = 16; i <= 21; i++) {
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3(-3.5 + shift, -3.5, 0);
        shift += 1;
    }

    //adds obstacles side by side on far left of screen
    shift = 0;
    for (int i = 22; i <= 23; i++) {
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3(-2.5 + shift, -0.25, 0);
        shift += 1;
    }
    //adds two separate blocks at different heights on right of screen
    shift = 1.5;
    for (int i = 24; i <= 25; i++) {
        state.platforms[i].textureID = platformTextureID;
        state.platforms[i].position = glm::vec3(-2.5 + shift, 1.75, 0);
        shift += 3.8;
    }

    //adds base platform
    state.platforms[26].textureID = platformBaseTextureID;
    state.platforms[26].position = glm::vec3(2.5, -3.5, 0);
    
    state.platforms[27].textureID = platformBaseTextureID;
    state.platforms[27].position = glm::vec3(3.5 , -3.5, 0);
    

    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].Update(0, NULL, 0);
    }
    
}

void DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text,
          float size, float spacing, glm::vec3 position){
float width = 1.0f / 16.0f;
float height = 1.0f / 16.0f;

std::vector<float> vertices;
std::vector<float> texCoords;

for (int i = 0; i < text.size(); i++) {
    int index = (int)text[i];
    float offset = (size + spacing) * i;
    float u = (float)(index % 16) / 16.0f;
    float v = (float)(index / 16) / 16.0f;

    vertices.insert(vertices.end(), {
        offset + (-0.5f * size), 0.5f * size,
        offset + (-0.5f * size), -0.5f * size,
        offset + (0.5f * size), 0.5f * size,
        offset + (0.5f * size), -0.5f * size,
        offset + (0.5f * size), 0.5f * size,
        offset + (-0.5f * size), -0.5f * size,
        });

    texCoords.insert(texCoords.end(), {
        u, v,
        u, v + height,
        u + width, v,
        u + width, v + height,
        u + width, v,
        u, v + height,
        });
}
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);

    program->SetModelMatrix(modelMatrix);

    glUseProgram(program->programID);

    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);

    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);

    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));

    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}


void ProcessInput() {
    
    state.player->movement = glm::vec3(0);
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        // Move the player left
                        break;
                        
                    case SDLK_RIGHT:
                        // Move the player right
                        break;
                }
                break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    //flag to check if player collides with base platform
    bool baseCollStatus = state.player->reachBase(&state.platforms[26])|| state.player->reachBase(&state.platforms[27]);
    if (baseCollStatus){
        //sets flag to true if player reaches base platform
        state.player->collidedBase = true;
    }
    else if (state.player->collidedBottom == false) {
            //changes acceleration depending on if player moves left or right
            if (keys[SDL_SCANCODE_LEFT]) {
                state.player->acceleration.x = -60.0f;
            }
            else if (keys[SDL_SCANCODE_RIGHT]) {
                state.player->acceleration.x = 60.0f;
            }
            
            if (glm::length(state.player->movement) > 1.0f) {
                state.player->movement = glm::normalize(state.player->movement);
            }
        }
    }


#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;

void Update(){
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }
    
    while (deltaTime >= FIXED_TIMESTEP) {
        // update. notice its fixed_timestep not deltaTime
        state.player->Update(FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT);
        
        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;
}



void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].Render(&program);
    }
    
    state.player->Render(&program);
    
    if (state.player->collidedBase==true) {
        DrawText(&program, LoadTexture("font1.png"), "MISSION SUCCESSFUL!!!", 0.5f,-.22f, glm::vec3(0,0,0));
    }
    else if (state.player->collidedLeft || state.player->collidedRight || state.player->collidedBottom) {
        DrawText(&program, LoadTexture("font1.png"), "MISSION FAILED.", 0.5f, -.22f, glm::vec3(0,0,0));
    }
    
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
