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
#include <vector>
#include "Entity.h"

#define PLATFORM_COUNT 24
#define ENEMY_COUNT 3


struct GameState {
    Entity* player;
    Entity* platforms;
    Entity* enemies;
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
    displayWindow = SDL_CreateWindow("Ellie Haber's Project 4- Rise of the AI", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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

    glClearColor(0.2f, 0.5f, 0.8f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // Initialize Game Objects

    // Initialize Player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(-4, -1, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);
    state.player->speed = 3.0f;
    state.player->textureID = LoadTexture("marshmallow_earphone-01.png");

    


    state.player->height = 0.5f;
    state.player->width = 0.5f;

    state.player->jumpPower = 7.2f;
    
    state.platforms = new Entity[PLATFORM_COUNT];
    GLuint platformTextureID = LoadTexture("jungle_Tile.png");

    
        
    int x = 0;
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].entityType = PLATFORM;
        state.platforms[i].textureID = platformTextureID;
        //set up base floor tiles
        if (i < 10) {
            state.platforms[i].position = glm::vec3(-4.5 + i, -3.25, 0);
        }
        //set up second tier platform
        else if (i < 14) {
            x = i - 4;
            state.platforms[i].position = glm::vec3(-4.5 + x, -2.25, 0);
        }
        //set up third tier platform
        else if (i < 17) {
            x = i - 7;
            state.platforms[i].position = glm::vec3(-4.5 + x, -1.25, 0);
        }
        //set up floating tier
        else if (i < 19) {
            x = i - 9;
            state.platforms[i].position = glm::vec3(-4.5 + x, 1.25, 0);
        }
        //set up middle blocks
        else if (i < 20){
            x = i - 14;
            state.platforms[i].position = glm::vec3(-4.5 + x+.5, 0.9f, 0);
            
        }

            
        state.platforms[i].Update(0, NULL, NULL, 0, NULL, NULL);
    }
    
    state.enemies = new Entity[ENEMY_COUNT];
    GLuint iceCreamTextureID = LoadTexture("02.png");
    GLuint blobTextureID = LoadTexture("skeleton-animation_00.png");
    state.enemies[0].textureID = iceCreamTextureID;
    state.enemies[1].textureID = blobTextureID;
    state.enemies[2].textureID = blobTextureID;

    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].speed = 2;
    }

    state.enemies[0].position = glm::vec3(-2.5, 1.75f, 0);
    state.enemies[0].aiType = PATROL;
    state.enemies[0].aiState = WALKING;

    state.enemies[1].position = glm::vec3(4.5f, 2.2f, 0);
    state.enemies[2].acceleration = glm::vec3(0, -2.81f, 0);
    state.enemies[1].aiType = WAITANDGO;
    state.enemies[1].aiState = IDLE;

    state.enemies[2].position = glm::vec3(-1, 1.25f, 0);
    state.enemies[2].aiType = JUMPER;
    state.enemies[2].acceleration = glm::vec3(0, -4.81f, 0);
    state.enemies[2].aiState = IDLE;
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
                break;
            case SDLK_RIGHT:
                break;
            case SDLK_SPACE:
                if(state.player->collidedBottom) state.player->jump = true;
                break;
            }
            break;
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        state.player->movement.x = -1.0f;
        state.player->animIndices = state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        state.player->movement.x = 1.0f;
        state.player->animIndices = state.player->animRight;
    }


    if (glm::length(state.player->movement) > 1.0f) {
        state.player->movement = glm::normalize(state.player->movement);
    }

}

#define FIXED_TIMESTEP 0.0166666f
float lastTicks = 0;
float accumulator = 0.0f;
void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }
    while (deltaTime >= FIXED_TIMESTEP) {
        // Update. Notice it's FIXED_TIMESTEP. Not deltaTime
        state.player->Update(FIXED_TIMESTEP, state.player, state.platforms, PLATFORM_COUNT, state.enemies, ENEMY_COUNT);

        for (int i = 0; i < ENEMY_COUNT; i++) {
            state.enemies[i].Update(FIXED_TIMESTEP, state.player, state.platforms, PLATFORM_COUNT, NULL, NULL);
        }
        
        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;
}

void DrawText(ShaderProgram* program, GLuint fontTextureID, std::string text,
    float size, float spacing, glm::vec3 position) {
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

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].Render(&program);
    }

    for (int i = 0; i < ENEMY_COUNT; i++) {
        state.enemies[i].Render(&program);
    }

    state.player->Render(&program);

    bool allEnemiesDefeated = true;
    for (int i = 0; i < ENEMY_COUNT; i++) {
        if (state.enemies[i].alive) allEnemiesDefeated = false;
    }

    if (allEnemiesDefeated) {
        DrawText(&program, LoadTexture("font1.png"), "You Win :)", 0.5f, -0.2f, glm::vec3(-1.75f, 0.5f, 0));
    }

    else if (state.player->alive == false) {
        DrawText(&program, LoadTexture("font1.png"), "You Lose :(", 0.5f, -0.2f, glm::vec3(-1.75f, 0.5f, 0));
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
