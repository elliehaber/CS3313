#include "Level2.h"

#define LEVEL2_WIDTH 14
#define LEVEL2_HEIGHT 8

#define LEVEL2_ENEMY_COUNT 1

unsigned int level2_data[] =
{
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 5, 0, 0, 0, 5, 0, 0, 5, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 6, 3,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 3,
    3, 0, 0, 5, 0, 0, 0, 0, 5, 0, 6, 6, 6, 3,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6
};

void Level2::Initialize() {

    state.nextScene = -1;
    //initialize tile blocks
    GLuint mapTextureID = Util::LoadTexture("Terrain (32x32).png");
    state.map = new Map(LEVEL2_WIDTH, LEVEL2_HEIGHT, level2_data, mapTextureID, 1.0f, 6, 2);
    
    //initialize player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->lives = state.lives;
    state.player->newPos = glm::vec3(2, -3, 0);

    state.player->position = glm::vec3(1.5, -6.5, 0);
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
    
    //initialize enemies
    state.enemies = new Entity[LEVEL2_ENEMY_COUNT];

    GLuint frogTextureID = Util::LoadTexture("frog.png");
    state.enemies[0].textureID = frogTextureID;

    for (int i = 0; i < LEVEL2_ENEMY_COUNT; i++) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].speed = 1.5;
    }

    state.enemies[0].position = glm::vec3(6.0f, -4.5f, 0);
    state.enemies[0].aiType = WAITANDGO;
    state.enemies[0].acceleration = glm::vec3(0, -17.81f, 0);
    state.enemies[0].aiState = IDLE;
}








void Level2::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL2_ENEMY_COUNT, state.map);
    state.lives = state.player->lives;
    //check if out of lives, if so jump to losing screen
    if (state.lives <= 0) {
        state.nextScene = 5;
    }
    else if (state.player->position.x >= 12.5 && state.player->position.y <= -2) {
        state.nextScene = 3;
    }
}

void Level2::Render(ShaderProgram* program) {
    state.map->Render(program);
    state.player->Render(program);
}
