#define GL_SILENCE_DEPRECATION
#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#include "Util.h"
#include "Entity.h"
#include "Map.h"
#include "Scene.h"
#include "MainMenu.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"
#include "WinScreen.h"
#include "LoseScreen.h"


SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;


Scene* currentScene;
Scene* sceneList[6];

Mix_Music* music;
Mix_Chunk* jump;

void SwitchToScene(Scene* scene) {
    currentScene = scene;
    currentScene->Initialize();
}


void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Ellie Haber's Project 5- Alien Platformer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(0, 0, 640, 480);

    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

    //add in audio tracks for background and jumping effect
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("magic cliffs.wav");
    Mix_PlayMusic(music, -1);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 8);

    jump = Mix_LoadWAV("jump.wav");
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    glUseProgram(program.programID);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //create scenes for game environment
    sceneList[0] = new MainMenu();
    sceneList[1] = new Level1();
    sceneList[2] = new Level2();
    sceneList[3] = new Level3();
    sceneList[4] = new WinMenu();
    sceneList[5] = new LoseMenu();

    sceneList[1]->state.lives = 3;
    
    SwitchToScene(sceneList[0]);

    
}

void ProcessInput() {
    currentScene->state.player->movement = glm::vec3(0);
    
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
            //allows jumps and plays music
            case SDLK_SPACE:
                if (currentScene->state.player->collidedBottom) {
                    currentScene->state.player->jump = true;
                    Mix_PlayChannel(-1, jump, 0);
                }
                break;
            //allows user to start/restart game
            case SDLK_RETURN:
                if (currentScene == sceneList[0] || currentScene == sceneList[4] || currentScene == sceneList[5]) {
                    sceneList[1]->state.lives = 3;
                    SwitchToScene(sceneList[1]);
                }
                break;
            }
            break; // SDL_KEYDOWN
        }
    }

    const Uint8* keys = SDL_GetKeyboardState(NULL);
    //player movements
    if (keys[SDL_SCANCODE_LEFT]) {
            currentScene->state.player->movement.x = -1.0f;
            currentScene->state.player->animIndices = currentScene->state.player->animLeft;
        }
        else if (keys[SDL_SCANCODE_RIGHT]) {
            currentScene->state.player->movement.x = 1.0f;
            currentScene->state.player->animIndices = currentScene->state.player->animRight;
        }

        if (glm::length(currentScene->state.player->movement) > 1.0f) {
            currentScene->state.player->movement = glm::normalize(currentScene->state.player->movement);
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
            currentScene->Update(FIXED_TIMESTEP);
            currentScene->state.enemies[0].Update(FIXED_TIMESTEP, currentScene->state.player,
                                                    currentScene->state.enemies, 1, currentScene->state.map);
            deltaTime -= FIXED_TIMESTEP;
        }
        accumulator = deltaTime;

        viewMatrix = glm::mat4(1.0f);
        if (currentScene->state.player->position.x > 5 && currentScene->state.player->position.x < 8) {
            viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, 3.75, 0));
        }
        else if (currentScene->state.player->position.x <= 5){
            viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
        }
        else if (currentScene->state.player->position.x >= -5) {
            viewMatrix = glm::translate(viewMatrix, glm::vec3(-8, 3.75, 0));
        }
}



void Render() {
    glClear(GL_COLOR_BUFFER_BIT);


    program.SetViewMatrix(viewMatrix);
    
    currentScene->Render(&program);

    currentScene->state.enemies[0].Render(&program);

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
    if (currentScene->state.nextScene >= 0) {
        if (currentScene->state.nextScene == 2 || currentScene->state.nextScene == 3) {
            sceneList[currentScene->state.nextScene]->state.lives = currentScene->state.lives;
        }
        SwitchToScene(sceneList[currentScene->state.nextScene]);
    }

    Render();
}

    Shutdown();
    return 0;
}
