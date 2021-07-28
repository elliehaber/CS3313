#include "Level3.h"

#define LEVEL3_WIDTH 14
#define LEVEL3_HEIGHT 8

#define LEVEL3_ENEMY_COUNT 1

unsigned int level3_data[] =
{
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 5, 0, 0, 5, 0, 0, 5, 0,
    3, 6, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 5, 0, 5, 0, 0, 5,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6
};

void Level3::Initialize() {

    state.nextScene = -1;
    //initialize tiles for background
    GLuint mapTextureID = Util::LoadTexture("Terrain (32x32).png");
    state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, level3_data, mapTextureID, 1.0f, 6, 2);
    
    //initialize player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->lives = state.lives;

    state.player->position = glm::vec3(1.5, -0.5, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);
    state.player->speed = 3.5f;
    state.player->textureID = Util::LoadTexture("alien.png");

    state.player->animRight = new int[4]{ 3, 7, 11, 15 };
    state.player->animLeft = new int[4]{ 1, 5, 9, 13 };
    state.player->animUp = new int[4]{ 2, 6, 10, 14 };
    state.player->animDown = new int[4]{ 0, 4, 8, 12 };

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 15;
    state.player->animRows = 1;

    state.player->height = 0.8f;
    state.player->width = 0.8f;

    state.player->jumpPower = 6.0f;
    //initialize enemy
    state.enemies = new Entity[LEVEL3_ENEMY_COUNT];

    GLuint frogTextureID = Util::LoadTexture("frog.png");

    state.enemies[0].textureID = frogTextureID;
    
    for (int i = 0; i < LEVEL3_ENEMY_COUNT; i++) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].speed = 2;
    }

    state.enemies[0].position = glm::vec3(4.5, -4.5f, 0);
    state.enemies[0].aiType = WAITANDGO;
    state.enemies[0].acceleration = glm::vec3(0, -17.81f, 0);
    state.enemies[0].aiState = IDLE;
}

void Level3::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map);
    state.lives = state.player->lives;
    //check if out of lives, if so jump to losing screen
    if (state.lives <= 0) {
        state.nextScene = 5;
    }
    if (state.player->position.x >= 12.5) {
        state.nextScene = 4;
    }
}

void Level3::Render(ShaderProgram* program) {
    state.map->Render(program);
    state.player->Render(program);
}
