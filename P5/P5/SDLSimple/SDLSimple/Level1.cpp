#include "Level1.h"

#define LEVEL1_WIDTH 14
#define LEVEL1_HEIGHT 8

#define LEVEL1_ENEMY_COUNT 1

unsigned int level1_data[] =
{
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 6, 6, 6, 6, 6, 6, 6,
    3, 0, 0, 3, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6
};

void Level1::Initialize() {

    state.nextScene = -1;
    //initialize tileset
    GLuint mapTextureID = Util::LoadTexture("Terrain (32x32).png");
    state.map = new Map(LEVEL1_WIDTH, LEVEL1_HEIGHT, level1_data, mapTextureID, 1.0f, 6, 2);
    
    //initialize player
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->lives = state.lives;

    state.player->position = glm::vec3(1, -3, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);
    state.player->speed = 3.5f;
    state.player->textureID = Util::LoadTexture("alien.png");
    
    //player positions
    state.player->animRight = new int[4]{ 5, 6};
    state.player->animLeft = new int[4]{ 7, 8 };
    state.player->animUp = new int[4]{ 0,1,2};
    state.player->animDown = new int[4]{ 0,1,2 };

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 15;
    state.player->animRows = 1;

    state.player->height = 0.8f;
    state.player->width = 0.8f;

    state.player->jumpPower = 9.0f;
    //initialize enemies
    state.enemies = new Entity[LEVEL1_ENEMY_COUNT];

    GLuint frogTextureID = Util::LoadTexture("frog.png");
    state.enemies[0].textureID = frogTextureID;

    for (int i = 0; i < LEVEL1_ENEMY_COUNT; i++) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].speed = 2;
    }
    state.enemies[0].position = glm::vec3(8.0f, -4.5f, 0);
    state.enemies[0].aiType = JUMPER;
    state.enemies[0].acceleration = glm::vec3(0, -17.81f, 0);
    state.enemies[0].aiState = IDLE;
}

void Level1::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL1_ENEMY_COUNT, state.map);
    state.lives = state.player->lives;
    //check if out of lives, if so jump to losing screen
    if (state.lives <= 0) {
        state.nextScene = 5;
    }
    
    if (state.player->position.x >= 12.5) {
        state.nextScene = 2;
    }
}

void Level1::Render(ShaderProgram* program) {
    state.map->Render(program);
    state.player->Render(program);
}
