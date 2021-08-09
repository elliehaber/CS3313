#include "Level3.h"

#define LEVEL3_WIDTH 14
#define LEVEL3_HEIGHT 8

#define LEVEL3_ENEMY_COUNT 3

unsigned int level3_data[] =
{
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 0, 0, 0, 6,
    6, 0, 6, 6, 6, 0, 6, 0, 6, 0, 0, 6, 0, 6,
    6, 6, 6, 0, 0, 0, 6, 0, 0, 0, 0, 6, 0, 6,
    6, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6,
    6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 6,
    6, 0, 0, 0, 0, 6, 0, 6, 0, 6, 6, 0, 6, 6,
    0, 0, 0, 6, 0, 0, 0, 6, 6, 6, 0, 0, 6, 6,
    6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 6, 6, 6
};

void Level3::Initialize() {

        state.nextScene = -1;
        //initialize tileset
        GLuint mapTextureID = Util::LoadTexture("brick_tiles.v1.png");
        state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, level3_data, mapTextureID, 1.0f, 2, 4);
        
        //initialize player
        state.player = new Entity();
        state.player->entityType = PLAYER;
        state.player->lives = state.lives;
        state.player->newPos = glm::vec3(2, -3, 0);

        state.player->position = glm::vec3(1.0f, -6.0f, 0);
        state.player->movement = glm::vec3(0,0,0);
        //state.player->acceleration = glm::vec3(0, 0, 0);
        state.player->speed = 1.0f;
        state.player->textureID = Util::LoadTexture("knight.png");
        
        //player positions
    //    state.player->animRight = new int[4]{ 10,11,12,13};
    //    state.player->animLeft = new int[4]{ 7, 8 };
    //    state.player->animUp = new int[4]{ 0,1,2};
    //    state.player->animDown = new int[4]{ 0,1,2 };

        state.player->animIndices = state.player->animRight;
        state.player->animFrames = 4;
        state.player->animIndex = 0;
        state.player->animTime = 0;
        state.player->animCols = 15;
        state.player->animRows = 1;

        state.player->height = 0.9f;
        state.player->width = 0.9f;

        state.player->jumpPower = 9.0f;
        //initialize enemies
        state.enemies = new Entity[LEVEL3_ENEMY_COUNT];

        GLuint eyeTextureID = Util::LoadTexture("enemy.png");
        state.enemies[0].textureID = eyeTextureID;
        state.enemies[1].textureID = eyeTextureID;
        state.enemies[2].textureID = eyeTextureID;


        for (int i = 0; i < LEVEL3_ENEMY_COUNT; i++) {
            state.enemies[i].entityType = ENEMY;
            state.enemies[i].speed = 0.2f;
        }
        state.enemies[0].position = glm::vec3(9.0f, -2.5f, 0);
        state.enemies[0].aiType = WAITANDGO;
        //state.enemies[0].acceleration = glm::vec3(0, -17.81f, 0);
        state.enemies[0].aiState = IDLE;
    
        state.enemies[1].position = glm::vec3(7.0f, -3.5f, 0);
        state.enemies[1].newPos = glm::vec3(7.0f, -3.5f, 0);

        state.enemies[1].aiType = WAITANDGO;
        //state.enemies[0].acceleration = glm::vec3(0, -17.81f, 0);
        state.enemies[1].aiState = IDLE;
    
        state.enemies[2].position = glm::vec3(5.0f, -4.5f, 0);
        state.enemies[2].newPos = glm::vec3(5.0f, -4.5f, 0);

        state.enemies[2].aiType = WAITANDGO;
        //state.enemies[0].acceleration = glm::vec3(0, -17.81f, 0);
        state.enemies[2].aiState = IDLE;
    }

void Level3::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map);
    state.lives = state.player->lives;
    //check if out of lives, if so jump to losing screen
    if (state.lives <= 0) {
        state.nextScene = 5;
    }
    if (state.player->position.y >= 12.5) {
        state.nextScene = 4;
    }
    for (int i = 0; i < LEVEL3_ENEMY_COUNT; i++) {
        state.enemies[i].Update(deltaTime, state.player, state.enemies, LEVEL3_ENEMY_COUNT, state.map);
    }
}

void Level3::Render(ShaderProgram* program) {
    state.map->Render(program);
    for (int i = 0; i < LEVEL3_ENEMY_COUNT; i++) state.enemies[i].Render(program);
    state.player->Render(program);
}
