#include "MainMenu.h"

#define MENU_WIDTH 14
#define MENU_HEIGHT 8

#define MENU_ENEMY_COUNT 1


//initialize map of main menu tiles
//ground blocks 2 levels on floor
unsigned int menu_data[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
    3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6
};

void MainMenu::Initialize() {

    state.nextScene = -1;

    GLuint mapTextureID = Util::LoadTexture("Terrain (32x32).png");
    state.map = new Map(MENU_WIDTH, MENU_HEIGHT, menu_data, mapTextureID, 1.0f, 6, 2);
    
    //initialize main player (protagonist)
    state.player = new Entity();
    state.player->entityType = PLAYER;
    state.player->position = glm::vec3(6.4, -2, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0, -9.81f, 0);
    state.player->speed = 3.7f;
    state.player->textureID = Util::LoadTexture("alien.png");

    //player appearance from different angles
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

    state.player->jumpPower = 7.0f;

    //initialize enemies
    state.enemies = new Entity[MENU_ENEMY_COUNT];

    GLuint frogTextureID = Util::LoadTexture("frog.png");
    state.enemies[0].textureID = frogTextureID;

    for (int i = 0; i < MENU_ENEMY_COUNT; i++) {
        state.enemies[i].entityType = ENEMY;
        state.enemies[i].speed = 2;
    }

    state.enemies[0].position = glm::vec3(8.0f, -4.75f, 0);
    state.enemies[0].aiType = JUMPER;
    state.enemies[0].acceleration = glm::vec3(0, -9.81f, 0);
    state.enemies[0].aiState = IDLE;
    state.enemies[0].isActive = false;
}

void MainMenu::Update(float deltaTime) {
    state.player->Update(deltaTime, state.player, state.enemies, MENU_ENEMY_COUNT, state.map);
}

void MainMenu::Render(ShaderProgram* program) {
    
    GLuint fontTextureID = Util::LoadTexture("font1.png");

    Util::DrawText(program, fontTextureID, "Alien Adventurer", 0.51f, 0.01f, glm::vec3(2.6, -1.0, 0));
    Util::DrawText(program, fontTextureID, "Press Enter to Begin", 0.4f, 0.01f, glm::vec3(2.6, -2.0, 0));
    Util::DrawText(program, fontTextureID, "Jump Using Spacebar", 0.35f, 0.01f, glm::vec3(2.7, -3.2, 0));
    Util::DrawText(program, fontTextureID, "Travel With Left/Right Arrows", 0.32f, 0.01f, glm::vec3(1.8, -4.2, 0));

    state.map->Render(program);
    state.player->Render(program);
}
