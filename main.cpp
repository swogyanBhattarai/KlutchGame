#include "raylib.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

typedef enum GameScreen
{
    MENU = 0,
    LEVELS,
    EASY,
    HARD,
    OPTIONS
} GameScreen;

typedef struct Sprite
{
    Texture2D tex;
    Vector2 pos;
} Sprite;

bool paused;
bool playMusic = true;
bool spawnTree = false;
bool showEnemy = false;
bool vulnerable = false;
bool gameOver = false;
bool showHitbox = false;

Color hitBox = {0, 0, 0, 0};

double vulTime = 0.0f;

float speed = 200.0f;

int j = 0, sp = 0, spd = 0, livesEasy = 3, livesHard = 1;
int scoreEasy = 0, highScoreEasy = 0, scoreHard = 0, highScoreHard;

class rectangles
{
public:
    rectangles() {};
    int height, width, posX, posY;
};

Rectangle carRecs[9], player;

int carSpeed[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};

void updateCarPos();

int convertToInt(string num)
{
    int ans = 0;
    for (int i = 0; i < num.length(); i++)
    {
        if (num[i] - '0' >= 0 && num[i] - '0' <= 9)
            ans = ans * 10 + num[i] - '0';

        else
        {
            return -1;
        }
    }
    return ans;
}

int main()
{
    InitWindow(1920, 1080, "Klutch");

    fstream easyScore, hardScore;
    string temp1, temp2;

    easyScore.open("highscoreEasy.txt", ios::in);
    if (easyScore.is_open())
    {
        getline(easyScore, temp1);
        easyScore.close();
    }

    hardScore.open("highscoreHard.txt", ios::in);
    if (hardScore.is_open())
    {
        getline(hardScore, temp2);
        hardScore.close();
    }

    highScoreEasy = convertToInt(temp1);

    highScoreHard = convertToInt(temp2);

    // audio
    InitAudioDevice();
    Music menu = LoadMusicStream("Music/menu music.mp3");
    Music easyStart = LoadMusicStream("Music/easy mode music.mp3");
    Sound easyEnd = LoadSound("Music/easy mode end.mp3");
    Music hardStart = LoadMusicStream("Music/hard mode music.mp3");
    Sound hardEnd = LoadSound("Music/hard mode end.mp3");

    SetTargetFPS(144);

    // settings value
    const char *vol[] = {"On", "Off"};
    int volSetting = 0;

    const char *frames[] = {"144", "60", "30"};
    int fps = 0;

    const char *toggle[] = {"Off", "On"};
    int toggleCount = 0;

    const char *boxToggle[] = {"Off", "On"};
    int boxCount = 0;

    GameScreen currentScreen = MENU;

    Vector2 titlepos;
    Vector2 audiopos;

    rectangles Area, leftLane, rightLane;
    Area.height = 1080;
    Area.width = 1920;
    Area.posX = 0;
    Area.posY = 0;

    // textures
    Sprite car, carVul, logo, roadHalf, bg, tree1, tree2, tree3, tree4, tree5, tree6, tree7, tree8, tree9, tree10, enemy[3], bgHard, roadHard;

    tree1.tex = LoadTexture("Sprites/actualTree.png");
    tree1.pos.x = 50;
    tree1.pos.y = -330;
    tree2.tex = LoadTexture("Sprites/actualTree.png");
    tree2.pos.x = 163;
    tree2.pos.y = -480;
    tree3.tex = LoadTexture("Sprites/actualTree.png");
    tree3.pos.x = 256;
    tree3.pos.y = -622;
    tree4.tex = LoadTexture("Sprites/actualTree.png");
    tree4.pos.x = 480;
    tree4.pos.y = -885;
    tree5.tex = LoadTexture("Sprites/actualTree.png");
    tree5.pos.x = 350;
    tree5.pos.y = -330;
    tree6.tex = LoadTexture("Sprites/actualTree.png");
    tree6.pos.x = 1330;
    tree6.pos.y = -480;
    tree7.tex = LoadTexture("Sprites/actualTree.png");
    tree7.pos.x = 1486;
    tree7.pos.y = -622;
    tree8.tex = LoadTexture("Sprites/actualTree.png");
    tree8.pos.x = 1750;
    tree8.pos.y = -885;
    tree9.tex = LoadTexture("Sprites/actualTree.png");
    tree9.pos.x = 1400;
    tree9.pos.y = -885;
    tree10.tex = LoadTexture("Sprites/actualTree.png");
    tree10.pos.x = 1850;
    tree10.pos.y = -885;

    enemy[0].tex = LoadTexture("Sprites/Enemy1.png");
    enemy[0].pos.x = 0;
    enemy[0].pos.y = 0;

    enemy[1].tex = LoadTexture("Sprites/Enemy2.png");
    enemy[1].pos.x = 0;
    enemy[1].pos.y = 0;

    enemy[2].tex = LoadTexture("Sprites/Enemy3.png");
    enemy[2].pos.x = 0;
    enemy[2].pos.y = 0;

    bg.tex = LoadTexture("Sprites/background.png");

    logo.tex = LoadTexture("Sprites/Logo.png");
    logo.pos.x = (GetScreenWidth() / 2) - 200;
    logo.pos.y = 100;

    roadHalf.tex = LoadTexture("Sprites/roadEasy.png");
    roadHalf.pos.x = (Area.posX + (Area.width / 3));
    roadHalf.pos.y = 0;

    bgHard.tex = LoadTexture("Sprites/backgroundHard.png");

    roadHard.tex = LoadTexture("Sprites/roadHard.png");
    roadHard.pos.x = (Area.posX + (Area.width / 3));
    roadHard.pos.y = 0;

    car.tex = LoadTexture("Sprites/Car.png");
    car.pos.x = roadHalf.pos.x + (roadHalf.pos.x / 2) - 26.5;
    car.pos.y = 1000;

    carVul.tex = LoadTexture("Sprites/carVul.png");

    // menu options
    const char *menuOptions[] = {
        "Start Game",
        "Settings",
        "Exit"};

    const char *levelOptions[] = {
        "Easy",
        "Hard"};

    const char *settingOptions[] = {
        "Music:",
        "FPS:",
        "Toggle Fullscreen:",
        "Show Hitbox:"};

    SetExitKey(KEY_NULL);

    // closing game
    bool exitWindow = false;

    // menu values
    const int numLevelOptions = sizeof(levelOptions) / sizeof(char *);
    const int numOptions = sizeof(settingOptions) / sizeof(char *);
    const int numMenuOptions = sizeof(menuOptions) / sizeof(char *);
    const int optionHeight = 50;
    const int optionSpacing = 20;
    const int menuWidth = 400;
    const int menuHeight = (numMenuOptions * optionHeight) + ((numMenuOptions - 10) * optionSpacing);
    const int menuX = (GetScreenWidth() - menuWidth) / 2;
    const int menuY = ((GetScreenHeight() - menuHeight) - 100) / 2;

    // font and colors
    Font font = LoadFont("resources/arial.ttf");
    Color fontColor = WHITE;
    Color highlightColor = GOLD;

    // title
    const char *title = "Klutch";
    const int titleFontSize = 80;
    titlepos.x = (GetScreenWidth() - MeasureText(title, titleFontSize)) / 2;
    titlepos.y = (GetScreenHeight() / 2) - MeasureText(title, titleFontSize);
    const Color titleColor = RAYWHITE;
    const int titleBorderThickness = 3;

    int selectedOption = 0, levelOption = 0, settingOption = 0;

    audiopos.x = (GetScreenWidth() - menuWidth) / 2;
    audiopos.y = ((GetScreenHeight() - menuHeight) - 100) / 2;

    player.x = roadHalf.pos.x + (roadHalf.pos.x / 2) - 15.28;
    player.y = 1000;

    while (!WindowShouldClose())
    {
        if (playMusic == true)
        {
            PlayMusicStream(menu);
        }
        else
        {
            StopMusicStream(menu);
        }

        switch (currentScreen)
        {

        case (MENU):
        {
            updateCarPos();
            vulnerable = false;
            gameOver = false;
            scoreEasy = 0;
            scoreHard = 0;

            if (playMusic == true)
            {
                PlayMusicStream(menu);
                UpdateMusicStream(menu);
            }

            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
            {
                selectedOption = (selectedOption + numMenuOptions - 1) % numMenuOptions;
            }
            else if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
            {
                selectedOption = (selectedOption + 1) % numMenuOptions;
            }

            BeginDrawing();
            ClearBackground(BLACK);

            // DrawTextEx(font, title, titlepos, titleFontSize, titleBorderThickness, titleColor);

            DrawTextureEx(logo.tex, {logo.pos.x, logo.pos.y}, 0.0f, 5.0f, WHITE);

            for (int i = 0; i < numMenuOptions; i++)
            {
                Color optionColor = (i == selectedOption) ? highlightColor : fontColor;
                DrawText(menuOptions[i], menuX, menuY + (i * (optionHeight + optionSpacing)) + 80, optionHeight, optionColor);
            }

            carRecs[0].y = 20;
            carRecs[1].y = 20;
            carRecs[2].y = 20;
            carRecs[3].y = 20;
            carRecs[4].y = 20;
            carRecs[5].y = 20;
            carRecs[6].y = 20;
            carRecs[7].y = 20;
            carRecs[8].y = 20;

            /* enemy[1].pos.y = 20;*/

            if (IsKeyPressed(KEY_ENTER))
            {
                switch (selectedOption)
                {
                case (0):
                    currentScreen = LEVELS;
                    break;
                case (1):
                    currentScreen = OPTIONS;
                    break;
                case (2):
                    CloseWindow();
                    break;
                }
            }
            EndDrawing();
            StopMusicStream(hardStart);
            StopMusicStream(easyStart);
            break;
        }

        case (LEVELS):
        {
            if (playMusic == true)
            {
                UpdateMusicStream(menu);
                ResumeMusicStream(menu);
            }

            BeginDrawing();
            ClearBackground(BLACK);
            paused = false;
            const char *select = "Please select a level";
            DrawText(select, (GetScreenWidth() - MeasureText(select, 50)) / 2, 125, 50, WHITE);

            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
            {
                levelOption = (levelOption + numLevelOptions - 1) % numLevelOptions;
            }
            else if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
            {
                levelOption = (levelOption + 1) % numLevelOptions;
            }
            for (int j = 0; j < numLevelOptions; j++)
            {
                Color levelColor = (j == levelOption) ? highlightColor : fontColor;
                DrawText(levelOptions[j], menuX, menuY + (j * (optionHeight + optionSpacing + 50)), optionHeight, levelColor);
            }

            if (IsKeyPressed(KEY_ENTER))
            {
                switch (levelOption)
                {
                case (0):
                    currentScreen = EASY;
                    break;
                case (1):
                    currentScreen = HARD;
                    break;
                }
            }

            if (IsKeyPressed(KEY_ESCAPE))
            {
                currentScreen = MENU;
            }
            EndDrawing();
            break;
        }
        case (OPTIONS):
        {
            if (playMusic == true)
            {
                UpdateMusicStream(menu);
            }

            BeginDrawing();
            ClearBackground(BLACK);
            const char *setText = "Use up and down to select & <- and -> keys to change settings";
            DrawText(setText, ((GetScreenWidth() - MeasureText(setText, 50)) / 2) + 320, 200, 30, WHITE);

            if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
            {
                settingOption = (settingOption + numOptions - 1) % numOptions;
            }
            else if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
            {
                settingOption = (settingOption + 1) % numOptions;
            }
            for (int k = 0; k < numOptions; k++)
            {
                Color levelColor = (k == settingOption) ? highlightColor : fontColor;
                DrawText(settingOptions[k], menuX, menuY + (k * (optionHeight + optionSpacing + 50)), optionHeight, levelColor);
            }

            DrawTextEx(font, vol[volSetting], {audiopos.x + 300, audiopos.y}, 50, 10, WHITE);
            DrawTextEx(font, frames[fps], {audiopos.x + 300, audiopos.y + 120}, 50, 10, WHITE);
            DrawTextEx(font, toggle[toggleCount], {audiopos.x + 600, audiopos.y + 240}, 50, 10, WHITE);
            DrawTextEx(font, boxToggle[boxCount], {audiopos.x + 450, audiopos.y + 360}, 50, 10, WHITE);

            switch (settingOption)
            {
            case 0:
                if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))
                {
                    volSetting = (volSetting + 1) % (sizeof(vol) / sizeof(char *));
                    if (volSetting == 1)
                    {
                        playMusic = false;
                    }
                    else
                    {
                        playMusic = true;
                    }
                }
                else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))
                {
                    volSetting = (volSetting + 1) % (sizeof(vol) / sizeof(char *));
                    if (volSetting == 1)
                    {
                        playMusic = false;
                    }
                    else
                    {
                        playMusic = true;
                    }
                }
                break;
            case 1:
                if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))
                {
                    fps = (fps - 1) % 3;
                    if (fps < 0)
                    {
                        fps = 2;
                    }
                }
                else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))
                {
                    fps = (fps + 1) % (sizeof(frames) / sizeof(char *));
                }
                break;
            case 2:
                if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))
                {
                    toggleCount = (toggleCount + 1) % (sizeof(toggle) / sizeof(char *));
                    ToggleFullscreen();
                }
                else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))
                {
                    toggleCount = (toggleCount + 1) % (sizeof(toggle) / sizeof(char *));
                    ToggleFullscreen();
                }
                break;
            case 3:
                if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))
                {
                    boxCount = (boxCount + 1) % (sizeof(boxToggle) / sizeof(char *));
                }
                else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))
                {
                    boxCount = (boxCount + 1) % (sizeof(boxToggle) / sizeof(char *));
                }
                break;
            }

            if (IsKeyPressed(KEY_ESCAPE))
            {
                currentScreen = MENU;
            }
            EndDrawing();
            break;
        }
        case (EASY):
        {
            // DrawRectangleLines(Area.posX, Area.posY, Area.width, Area.height, GREEN); //main rect
            // DrawRectangleLines(Area.posX, Area.posY, Area.width / 3, Area.height, RED); //left third
            // DrawRectangleLines(Area.posX + (Area.width / 3) * 2, Area.posY, Area.width / 3, Area.height, BLUE); //right third
            // DrawRectangleLines(Area.posX + (Area.width / 3), Area.posY, Area.width / 3, Area.height, hitBox); // middle third

            if (boxCount == 0)
            {
                hitBox = {0, 0, 0, 0};
            }
            else if (boxCount == 1)
            {
                hitBox = {253, 249, 0, 255};
            }

            speed = 200.0f;

            const char *pauseText = "The game is paused. Return to menu? [Y/N]";

            if (tree1.pos.y <= -150)
            {
                tree1.pos.x = GetRandomValue(20, 520);
            }
            if (tree2.pos.y <= -150)
            {
                tree2.pos.x = GetRandomValue(20, 520);
            }
            if (tree3.pos.y <= -150)
            {
                tree3.pos.x = GetRandomValue(20, 520);
            }
            if (tree4.pos.y <= -150)
            {
                tree4.pos.x = GetRandomValue(20, 520);
            }
            if (tree5.pos.y <= -150)
            {
                tree5.pos.x = GetRandomValue(20, 520);
            }

            if (tree1.pos.y > GetScreenHeight())
            {
                tree1.pos.y = -150;
            }
            if (tree2.pos.y > GetScreenHeight())
            {
                tree2.pos.y = -350;
            }
            if (tree3.pos.y > GetScreenHeight())
            {
                tree3.pos.y = -550;
            }
            if (tree4.pos.y > GetScreenHeight())
            {
                tree4.pos.y = -750;
            }
            if (tree5.pos.y > GetScreenHeight())
            {
                tree5.pos.y = -850;
            }

            if (tree6.pos.y <= -150)
            {
                tree6.pos.x = GetRandomValue(1330, 1750);
            }
            if (tree7.pos.y <= -150)
            {
                tree7.pos.x = GetRandomValue(1330, 1750);
            }
            if (tree8.pos.y <= -150)
            {
                tree8.pos.x = GetRandomValue(1330, 1750);
            }
            if (tree9.pos.y <= -150)
            {
                tree9.pos.x = GetRandomValue(1330, 1750);
            }
            if (tree10.pos.y <= -150)
            {
                tree10.pos.x = GetRandomValue(1330, 1750);
            }

            if (tree6.pos.y > GetScreenHeight())
            {
                tree6.pos.y = -150;
            }
            if (tree7.pos.y > GetScreenHeight())
            {
                tree7.pos.y = -350;
            }
            if (tree8.pos.y > GetScreenHeight())
            {
                tree8.pos.y = -550;
            }
            if (tree9.pos.y > GetScreenHeight())
            {
                tree9.pos.y = -750;
            }
            if (tree10.pos.y > GetScreenHeight())
            {
                tree10.pos.y = -850;
            }

            if (fps == 0)
            {
                SetTargetFPS(144);
            }
            else if (fps == 1)
            {
                SetTargetFPS(60);
            }
            else if (fps == 2)
            {
                SetTargetFPS(30);
            }

            if (paused == false)
            {
                if (gameOver == false)
                {

                    if (playMusic == true)
                    {
                        PlayMusicStream(easyStart);
                        UpdateMusicStream(easyStart);
                    }

                    // up movement
                    if (IsKeyDown(KEY_W))
                    {
                        player.y -= 100 * GetFrameTime();
                        car.pos.y -= 100 * GetFrameTime();
                        speed = 200.0f * 2;
                        if (showEnemy == true)
                        {
                            carSpeed[0] = 2.5;
                            carSpeed[1] = 2.5;
                            carSpeed[2] = 2.5;
                            carSpeed[3] = 2.5;
                            carSpeed[4] = 2.5;
                            carSpeed[5] = 2.5;
                            carSpeed[6] = 2.5;
                            carSpeed[7] = 2.5;
                            carSpeed[8] = 2.5;
                        }
                    }

                    // down movement
                    else if (IsKeyDown(KEY_S))
                    {
                        player.y += 300 * GetFrameTime();
                        car.pos.y += 300 * GetFrameTime();

                        if (showEnemy == true)
                        {
                            carSpeed[0] = 1;
                            carSpeed[1] = 1;
                            carSpeed[2] = 1;
                            carSpeed[3] = 1;
                            carSpeed[4] = 1;
                            carSpeed[5] = 1;
                            carSpeed[6] = 1;
                            carSpeed[7] = 1;
                            carSpeed[8] = 1;
                        }
                    }
                    else
                    {
                        speed = 200.0f;
                        carSpeed[0] = 1;
                        carSpeed[1] = 1;
                        carSpeed[2] = 1;
                        carSpeed[3] = 1;
                        carSpeed[4] = 1;
                        carSpeed[5] = 1;
                        carSpeed[6] = 1;
                        carSpeed[7] = 1;
                        carSpeed[8] = 1;
                    }

                    // left movement
                    if (IsKeyDown(KEY_A))
                    {
                        player.x -= 400 * GetFrameTime();
                        car.pos.x -= 400 * GetFrameTime();
                    }

                    // right movement
                    if (IsKeyDown(KEY_D))
                    {
                        player.x += 400 * GetFrameTime();
                        car.pos.x += 400 * GetFrameTime();
                    }

                    // check if out of boundary
                    if (player.y <= 0)
                    {
                        player.y = 0;
                        car.pos.y = 0;
                    }

                    if (player.x <= 720)
                    {
                        player.x = 720;
                        car.pos.x = 720 - 10.7;
                    }

                    if ((player.x + 40) >= 1200)
                    {
                        player.x = 1200 - 40;
                        car.pos.x = 1200 - 51.5;
                    }

                    if ((player.y + 64) >= 1079)
                    {
                        player.y = 1079 - 64;
                        car.pos.y = 1079 - 64;
                    }

                    // tree speed
                    tree1.pos.y += speed * GetFrameTime();
                    tree2.pos.y += speed * GetFrameTime();
                    tree3.pos.y += speed * GetFrameTime();
                    tree4.pos.y += speed * GetFrameTime();
                    tree5.pos.y += speed * GetFrameTime();
                    tree6.pos.y += speed * GetFrameTime();
                    tree7.pos.y += speed * GetFrameTime();
                    tree8.pos.y += speed * GetFrameTime();
                    tree9.pos.y += speed * GetFrameTime();
                    tree10.pos.y += speed * GetFrameTime();

                    BeginDrawing();
                    ClearBackground(BLACK);
                    DrawTexture(bg.tex, 0, 0, WHITE);

                    /* for (int s = 0; s < sizeof(carSpeed) / sizeof(carSpeed[0]); s++) {
                        carSpeed[s] = GetRandomValue(0, 9);
                    } */

                    DrawTextureEx(roadHalf.tex, {roadHalf.pos.x, roadHalf.pos.y}, 0.0f, 1.0f, WHITE);
                    DrawTextureEx(tree1.tex, {tree1.pos.x, tree1.pos.y}, 0.0f, 2.5f, WHITE);
                    DrawTextureEx(tree2.tex, {tree2.pos.x, tree2.pos.y}, 0.0f, 2.5f, WHITE);
                    DrawTextureEx(tree3.tex, {tree3.pos.x, tree3.pos.y}, 0.0f, 2.5f, WHITE);
                    DrawTextureEx(tree4.tex, {tree4.pos.x, tree4.pos.y}, 0.0f, 2.5f, WHITE);
                    DrawTextureEx(tree5.tex, {tree5.pos.x, tree5.pos.y}, 0.0f, 2.5f, WHITE);

                    DrawTextureEx(tree6.tex, {tree6.pos.x, tree6.pos.y}, 0.0f, 2.5f, WHITE);
                    DrawTextureEx(tree7.tex, {tree7.pos.x, tree7.pos.y}, 0.0f, 2.5f, WHITE);
                    DrawTextureEx(tree8.tex, {tree8.pos.x, tree8.pos.y}, 0.0f, 2.5f, WHITE);
                    DrawTextureEx(tree9.tex, {tree9.pos.x, tree9.pos.y}, 0.0f, 2.5f, WHITE);
                    DrawTextureEx(tree10.tex, {tree10.pos.x, tree10.pos.y}, 0.0f, 2.5f, WHITE);

                    // car.pos.y -= 100 * GetFrameTime();

                    if (vulnerable == false)
                    {
                        DrawTextureEx(car.tex, {car.pos.x, car.pos.y}, 0.0f, 2.0f, WHITE);
                    }

                    else
                    {
                        DrawTextureEx(carVul.tex, {car.pos.x, car.pos.y}, 0.0f, 2.0f, WHITE);
                    }

                    for (int a = 0; a < sizeof(carRecs) / sizeof(carRecs[0]); a++)
                    {
                        /* DrawRectangleLines(carRecs[a].x, carRecs[a].y, carRecs[a].width, carRecs[a].height, hitBox);
                        DrawTextureEx(enemy1.tex, { carRecs[a].x - 14, carRecs[a].y }, 0.0f, 2.0f, WHITE);
                        carRecs[a].y += GetRandomValue(50, 300) * GetFrameTime() * carSpeed[a];
                        enemy1.pos.y += GetRandomValue(50, 300) * GetFrameTime() * carSpeed[a]; */
                        if (carRecs[a].y > GetScreenHeight() + 10)
                        {
                            carRecs[a].y = enemy[0].pos.y = GetRandomValue(-100, -1000);
                            showEnemy = true;
                        }
                    }

                    // drawing enemy and it's hitbox
                    if (showEnemy != false)
                    {
                        DrawRectangleLines(carRecs[0].x, carRecs[0].y, carRecs[0].width, carRecs[0].height, hitBox);
                        DrawTextureEx(enemy[0].tex, {carRecs[0].x - 14, carRecs[0].y}, 0.0f, 2.0f, WHITE);

                        DrawRectangleLines(carRecs[1].x, carRecs[1].y, carRecs[1].width, carRecs[1].height, hitBox);
                        DrawTextureEx(enemy[0].tex, {carRecs[1].x - 14, carRecs[1].y}, 0.0f, 2.0f, WHITE);

                        DrawRectangleLines(carRecs[2].x, carRecs[2].y, carRecs[2].width, carRecs[2].height, hitBox);
                        DrawTextureEx(enemy[0].tex, {carRecs[2].x - 14, carRecs[2].y}, 0.0f, 2.0f, WHITE);

                        DrawRectangleLines(carRecs[3].x, carRecs[3].y, carRecs[3].width, carRecs[3].height, hitBox);
                        DrawTextureEx(enemy[0].tex, {carRecs[3].x - 14, carRecs[3].y}, 0.0f, 2.0f, WHITE);

                        DrawRectangleLines(carRecs[4].x, carRecs[4].y, carRecs[4].width, carRecs[4].height, hitBox);
                        DrawTextureEx(enemy[0].tex, {carRecs[4].x - 14, carRecs[4].y}, 0.0f, 2.0f, WHITE);

                        DrawRectangleLines(carRecs[5].x, carRecs[5].y, carRecs[5].width, carRecs[5].height, hitBox);
                        DrawTextureEx(enemy[0].tex, {carRecs[5].x - 14, carRecs[5].y}, 0.0f, 2.0f, WHITE);

                        DrawRectangleLines(carRecs[6].x, carRecs[6].y, carRecs[6].width, carRecs[6].height, hitBox);
                        DrawTextureEx(enemy[0].tex, {carRecs[6].x - 14, carRecs[6].y}, 0.0f, 2.0f, WHITE);

                        DrawRectangleLines(carRecs[7].x, carRecs[7].y, carRecs[7].width, carRecs[7].height, hitBox);
                        DrawTextureEx(enemy[0].tex, {carRecs[7].x - 14, carRecs[7].y}, 0.0f, 2.0f, WHITE);

                        DrawRectangleLines(carRecs[8].x, carRecs[8].y, carRecs[8].width, carRecs[8].height, hitBox);
                        DrawTextureEx(enemy[0].tex, {carRecs[8].x - 14, carRecs[8].y}, 0.0f, 2.0f, WHITE);

                        // giving speed to hitbox and enemy
                        carRecs[0].y += GetRandomValue(50, 300) * GetFrameTime() * (carSpeed[0] + GetRandomValue(1, 1.5));
                        enemy[0].pos.y += GetRandomValue(50, 300) * GetFrameTime() * (carSpeed[0] + GetRandomValue(1, 1.5));

                        carRecs[1].y += GetRandomValue(50, 300) * GetFrameTime() * (carSpeed[1] + GetRandomValue(1.5, 2));
                        enemy[0].pos.y += GetRandomValue(50, 300) * GetFrameTime() * (carSpeed[1] + GetRandomValue(1.5, 2));

                        carRecs[2].y += GetRandomValue(50, 300) * GetFrameTime() * (carSpeed[2] + GetRandomValue(1.5, 2));
                        enemy[0].pos.y += GetRandomValue(50, 300) * GetFrameTime() * (carSpeed[2] + GetRandomValue(1.5, 2));

                        carRecs[3].y += GetRandomValue(50, 300) * GetFrameTime() * (carSpeed[3] + GetRandomValue(2, 1.5));
                        enemy[0].pos.y += GetRandomValue(50, 300) * GetFrameTime() * (carSpeed[3] + GetRandomValue(2, 1.5));

                        carRecs[4].y += GetRandomValue(50, 300) * GetFrameTime() * (carSpeed[4] + GetRandomValue(2.5, 1));
                        enemy[0].pos.y += GetRandomValue(50, 300) * GetFrameTime() * (carSpeed[4] + GetRandomValue(2.5, 1));

                        carRecs[5].y += GetRandomValue(50, 300) * GetFrameTime() * (carSpeed[5] + GetRandomValue(1, 2.5));
                        enemy[0].pos.y += GetRandomValue(50, 300) * GetFrameTime() * (carSpeed[5] + GetRandomValue(1, 2.5));

                        carRecs[6].y += GetRandomValue(50, 300) * GetFrameTime() * (carSpeed[6] + GetRandomValue(1.5, 2));
                        enemy[0].pos.y += GetRandomValue(50, 300) * GetFrameTime() * (carSpeed[6] + GetRandomValue(1.5, 2));

                        carRecs[7].y += GetRandomValue(50, 300) * GetFrameTime() * (carSpeed[7] + GetRandomValue(2, 1.5));
                        enemy[0].pos.y += GetRandomValue(50, 300) * GetFrameTime() * (carSpeed[7] + GetRandomValue(2, 1.5));

                        carRecs[8].y += GetRandomValue(50, 300) * GetFrameTime() * (carSpeed[8] + GetRandomValue(2.5, 2));
                        enemy[0].pos.y += GetRandomValue(50, 300) * GetFrameTime() * (carSpeed[8] + GetRandomValue(2.5, 2));

                        if (vulnerable == false)
                        {
                            for (int num = 0; num < sizeof(carRecs) / sizeof(carRecs[0]); num++)
                            {
                                if (CheckCollisionPointRec({player.x, player.y}, carRecs[num]) || CheckCollisionPointRec({player.x + 40, player.y}, carRecs[num]))
                                {
                                    vulnerable = true;
                                    if (vulnerable == true)
                                    {
                                        livesEasy--;
                                    }
                                }
                                if (CheckCollisionPointRec({player.x, player.y + 64}, carRecs[num]) || CheckCollisionPointRec({player.x + 40, player.y + 64}, carRecs[num]))
                                {
                                    vulnerable = true;
                                    if (vulnerable == true)
                                    {
                                        livesEasy--;
                                    }
                                }
                            }
                        }

                        if (vulnerable == true)
                        {
                            vulTime += GetFrameTime();
                            if (vulTime > 1)
                            {
                                vulnerable = false;
                                vulTime = 0;
                            }
                        }
                    }
                    else
                    {
                        // giving speed to hitbox and enemy before they become visible
                        carRecs[0].y += 300 * GetFrameTime();
                        enemy[0].pos.y += 300 * GetFrameTime();

                        carRecs[1].y += 300 * GetFrameTime() * carSpeed[1];
                        enemy[0].pos.y += 300 * GetFrameTime() * carSpeed[1];

                        carRecs[2].y += 300 * GetFrameTime() * carSpeed[2];
                        enemy[0].pos.y += 300 * GetFrameTime() * carSpeed[2];

                        carRecs[3].y += 300 * GetFrameTime() * carSpeed[3];
                        enemy[0].pos.y += 300 * GetFrameTime() * carSpeed[3];

                        carRecs[4].y += 300 * GetFrameTime() * carSpeed[4];
                        enemy[0].pos.y += 300 * GetFrameTime() * carSpeed[4];

                        carRecs[5].y += 300 * GetFrameTime() * carSpeed[5];
                        enemy[0].pos.y += 300 * GetFrameTime() * carSpeed[5];

                        carRecs[6].y += 300 * GetFrameTime() * carSpeed[6];
                        enemy[0].pos.y += 300 * GetFrameTime() * carSpeed[6];

                        carRecs[7].y += 300 * GetFrameTime() * carSpeed[7];
                        enemy[0].pos.y += 300 * GetFrameTime() * carSpeed[7];

                        carRecs[8].y += 300 * GetFrameTime() * carSpeed[8];
                        enemy[0].pos.y += 300 * GetFrameTime() * carSpeed[8];
                    }

                    DrawRectangleLines(player.x, player.y, 40, 64, hitBox);

                    if (IsKeyPressed(KEY_ESCAPE))
                    {
                        paused = true;
                        PauseMusicStream(easyStart);
                    }

                    if (livesEasy == 0)
                    {
                        gameOver = true;
                    }

                    scoreEasy += 150 * GetFrameTime();

                    DrawText(TextFormat("Lives: %i", livesEasy), 50, 50, 30, WHITE);
                    DrawText(TextFormat("Score: %i", scoreEasy), 50, 100, 30, WHITE);
                    DrawText(TextFormat("High Score: %i", highScoreEasy), 50, 150, 30, WHITE);

                    if ((highScoreEasy > 0) && (scoreEasy > highScoreEasy))
                    {
                        highScoreEasy = scoreEasy;
                        DrawText(TextFormat("New High Score!!!"), 50, 200, 30, WHITE);
                    }

                    EndDrawing();
                }

                else
                {
                    ClearBackground(BLACK);
                    BeginDrawing();
                    DrawText("Game Over, press ESC to go back to menu", 650, (GetScreenHeight() / 2) - 58, 30, WHITE);
                    DrawText(TextFormat("Lives: %i", livesEasy), 50, 50, 30, WHITE);
                    DrawText(TextFormat("Score: %i", scoreEasy), 50, 100, 30, WHITE);
                    DrawText(TextFormat("High Score: %i", highScoreEasy), 50, 150, 30, WHITE);

                    if (scoreEasy > highScoreEasy)
                    {
                        highScoreEasy = scoreEasy;
                    }

                    if (IsKeyPressed(KEY_ESCAPE))
                    {
                        easyScore.open("highscoreEasy.txt", ios::out);
                        easyScore << highScoreEasy;
                        easyScore.close();
                        car.pos.x = roadHalf.pos.x + (roadHalf.pos.x / 2) - 26.5;
                        car.pos.y = 1000;
                        player.x = roadHalf.pos.x + (roadHalf.pos.x / 2) - 15.28;
                        player.y = 1000;
                        tree1.pos.x = 50;
                        tree1.pos.y = -150;
                        tree2.pos.x = 163;
                        tree2.pos.y = -350;
                        tree3.pos.x = 256;
                        tree3.pos.y = -550;
                        tree4.pos.x = 300;
                        tree4.pos.y = -750;
                        tree4.pos.x = 300;
                        tree4.pos.y = -850;
                        tree6.pos.x = 1330;
                        tree6.pos.y = -480;
                        tree7.pos.x = 1486;
                        tree7.pos.y = -622;
                        tree8.pos.x = 1750;
                        tree8.pos.y = -885;
                        tree9.pos.x = 1400;
                        tree9.pos.y = -885;
                        tree10.pos.x = 1850;
                        tree10.pos.y = -885;
                        showEnemy = false;
                        livesEasy = 3;
                        livesHard = 1;
                        currentScreen = MENU;
                    }
                    EndDrawing();
                }
            }
            else
            {
                BeginDrawing();
                speed = 0.0f;
                DrawRectangle(0, (GetScreenHeight() / 2) - 100, GetScreenWidth(), 100, WHITE);
                DrawText(pauseText, 650, (GetScreenHeight() / 2) - 60, 30, BLACK);

                if (IsKeyPressed(KEY_Y))
                {
                    currentScreen = MENU;
                    car.pos.x = roadHalf.pos.x + (roadHalf.pos.x / 2) - 26.5;
                    car.pos.y = 1000;
                    player.x = roadHalf.pos.x + (roadHalf.pos.x / 2) - 15.28;
                    player.y = 1000;
                    tree1.pos.x = 50;
                    tree1.pos.y = -150;
                    tree2.pos.x = 163;
                    tree2.pos.y = -350;
                    tree3.pos.x = 256;
                    tree3.pos.y = -550;
                    tree4.pos.x = 300;
                    tree4.pos.y = -750;
                    tree4.pos.x = 300;
                    tree4.pos.y = -850;
                    tree6.pos.x = 1330;
                    tree6.pos.y = -480;
                    tree7.pos.x = 1486;
                    tree7.pos.y = -622;
                    tree8.pos.x = 1750;
                    tree8.pos.y = -885;
                    tree9.pos.x = 1400;
                    tree9.pos.y = -885;
                    tree10.pos.x = 1850;
                    tree10.pos.y = -885;
                    showEnemy = false;
                    livesEasy = 3;
                    livesHard = 1;
                    updateCarPos();
                }
                else if (IsKeyPressed(KEY_N))
                {
                    paused = false;
                }
                if (IsKeyPressed(KEY_ESCAPE))
                {
                    paused = false;
                }
                EndDrawing();
            }
            StopMusicStream(menu);
            break;
        }
        case (HARD):
        {

            if (boxCount == 0)
            {
                hitBox = {0, 0, 0, 0};
            }
            else if (boxCount == 1)
            {
                hitBox = {253, 249, 0, 255};
            }

            speed = 200.0f;

            const char *pauseText = "The game is paused. Return to menu? [Y/N]";

            if (fps == 0)
            {
                SetTargetFPS(144);
            }
            else if (fps == 1)
            {
                SetTargetFPS(60);
            }
            else if (fps == 2)
            {
                SetTargetFPS(30);
            }

            if (paused == false)
            {
                if (gameOver == false)
                {

                    if (playMusic == true)
                    {
                        PlayMusicStream(hardStart);
                        UpdateMusicStream(hardStart);
                    }

                    if (tree1.pos.y <= -150)
                    {
                        tree1.pos.x = GetRandomValue(20, 520);
                    }
                    if (tree2.pos.y <= -150)
                    {
                        tree2.pos.x = GetRandomValue(20, 520);
                    }
                    if (tree3.pos.y <= -150)
                    {
                        tree3.pos.x = GetRandomValue(20, 520);
                    }
                    if (tree4.pos.y <= -150)
                    {
                        tree4.pos.x = GetRandomValue(20, 520);
                    }
                    if (tree5.pos.y <= -150)
                    {
                        tree5.pos.x = GetRandomValue(20, 520);
                    }

                    if (tree1.pos.y > GetScreenHeight())
                    {
                        tree1.pos.y = -150;
                    }
                    if (tree2.pos.y > GetScreenHeight())
                    {
                        tree2.pos.y = -350;
                    }
                    if (tree3.pos.y > GetScreenHeight())
                    {
                        tree3.pos.y = -550;
                    }
                    if (tree4.pos.y > GetScreenHeight())
                    {
                        tree4.pos.y = -750;
                    }
                    if (tree5.pos.y > GetScreenHeight())
                    {
                        tree5.pos.y = -850;
                    }

                    if (tree6.pos.y <= -150)
                    {
                        tree6.pos.x = GetRandomValue(1330, 1750);
                    }
                    if (tree7.pos.y <= -150)
                    {
                        tree7.pos.x = GetRandomValue(1330, 1750);
                    }
                    if (tree8.pos.y <= -150)
                    {
                        tree8.pos.x = GetRandomValue(1330, 1750);
                    }
                    if (tree9.pos.y <= -150)
                    {
                        tree9.pos.x = GetRandomValue(1330, 1750);
                    }
                    if (tree10.pos.y <= -150)
                    {
                        tree10.pos.x = GetRandomValue(1330, 1750);
                    }

                    if (tree6.pos.y > GetScreenHeight())
                    {
                        tree6.pos.y = -150;
                    }
                    if (tree7.pos.y > GetScreenHeight())
                    {
                        tree7.pos.y = -350;
                    }
                    if (tree8.pos.y > GetScreenHeight())
                    {
                        tree8.pos.y = -550;
                    }
                    if (tree9.pos.y > GetScreenHeight())
                    {
                        tree9.pos.y = -750;
                    }
                    if (tree10.pos.y > GetScreenHeight())
                    {
                        tree10.pos.y = -850;
                    }

                    // up movement
                    if (IsKeyDown(KEY_W))
                    {
                        player.y -= 100 * GetFrameTime();
                        car.pos.y -= 100 * GetFrameTime();
                        speed = 200.0f * 2;
                        if (showEnemy == true)
                        {
                            carSpeed[0] = 4.5;
                            carSpeed[1] = 4.5;
                            carSpeed[2] = 4.5;
                            carSpeed[3] = 4.5;
                            carSpeed[4] = 4.5;
                            carSpeed[5] = 4.5;
                            carSpeed[6] = 4.5;
                            carSpeed[7] = 4.5;
                            carSpeed[8] = 4.5;
                        }
                    }

                    // down movement
                    else if (IsKeyDown(KEY_S))
                    {
                        player.y += 50 * GetFrameTime();
                        car.pos.y += 50 * GetFrameTime();

                        if (showEnemy == true)
                        {
                            carSpeed[0] = 1;
                            carSpeed[1] = 1;
                            carSpeed[2] = 1;
                            carSpeed[3] = 1;
                            carSpeed[4] = 1;
                            carSpeed[5] = 1;
                            carSpeed[6] = 1;
                            carSpeed[7] = 1;
                            carSpeed[8] = 1;
                        }
                    }
                    else
                    {
                        speed = 200.0f;
                        carSpeed[0] = 2.5;
                        carSpeed[1] = 2.5;
                        carSpeed[2] = 2.5;
                        carSpeed[3] = 2.5;
                        carSpeed[4] = 2.5;
                        carSpeed[5] = 2.5;
                        carSpeed[6] = 2.5;
                        carSpeed[7] = 2.5;
                        carSpeed[8] = 2.5;
                    }

                    // left movement
                    if (IsKeyDown(KEY_A))
                    {
                        player.x -= 200 * GetFrameTime();
                        car.pos.x -= 200 * GetFrameTime();
                    }

                    // right movement
                    if (IsKeyDown(KEY_D))
                    {
                        player.x += 200 * GetFrameTime();
                        car.pos.x += 200 * GetFrameTime();
                    }

                    // check if out of boundary
                    if (player.y <= 0)
                    {
                        player.y = 0;
                        car.pos.y = 0;
                    }

                    if (player.x <= 720)
                    {
                        player.x = 720;
                        car.pos.x = 720 - 10.7;
                    }

                    if ((player.x + 40) >= 1200)
                    {
                        player.x = 1200 - 40;
                        car.pos.x = 1200 - 51.5;
                    }

                    if ((player.y + 64) >= 1079)
                    {
                        player.y = 1079 - 64;
                        car.pos.y = 1079 - 64;
                    }

                    tree1.pos.y += speed * GetFrameTime();
                    tree2.pos.y += speed * GetFrameTime();
                    tree3.pos.y += speed * GetFrameTime();
                    tree4.pos.y += speed * GetFrameTime();
                    tree5.pos.y += speed * GetFrameTime();
                    tree6.pos.y += speed * GetFrameTime();
                    tree7.pos.y += speed * GetFrameTime();
                    tree8.pos.y += speed * GetFrameTime();
                    tree9.pos.y += speed * GetFrameTime();
                    tree10.pos.y += speed * GetFrameTime();

                    BeginDrawing();
                    ClearBackground(BLACK);
                    DrawTexture(bgHard.tex, 0, 0, WHITE);
                    DrawTextureEx(roadHard.tex, {roadHard.pos.x, roadHard.pos.y}, 0.0f, 1.0f, WHITE);

                    DrawTextureEx(tree1.tex, {tree1.pos.x, tree1.pos.y}, 0.0f, 2.5f, WHITE);
                    DrawTextureEx(tree2.tex, {tree2.pos.x, tree2.pos.y}, 0.0f, 2.5f, WHITE);
                    DrawTextureEx(tree3.tex, {tree3.pos.x, tree3.pos.y}, 0.0f, 2.5f, WHITE);
                    DrawTextureEx(tree4.tex, {tree4.pos.x, tree4.pos.y}, 0.0f, 2.5f, WHITE);
                    DrawTextureEx(tree5.tex, {tree5.pos.x, tree5.pos.y}, 0.0f, 2.5f, WHITE);

                    DrawTextureEx(tree6.tex, {tree6.pos.x, tree6.pos.y}, 0.0f, 2.5f, WHITE);
                    DrawTextureEx(tree7.tex, {tree7.pos.x, tree7.pos.y}, 0.0f, 2.5f, WHITE);
                    DrawTextureEx(tree8.tex, {tree8.pos.x, tree8.pos.y}, 0.0f, 2.5f, WHITE);
                    DrawTextureEx(tree9.tex, {tree9.pos.x, tree9.pos.y}, 0.0f, 2.5f, WHITE);
                    DrawTextureEx(tree10.tex, {tree10.pos.x, tree10.pos.y}, 0.0f, 2.5f, WHITE);

                    /* for (int s = 0; s < sizeof(carSpeed) / sizeof(carSpeed[0]); s++) {
                        carSpeed[s] = GetRandomValue(0, 9);
                    } */

                    if (vulnerable == false)
                    {
                        DrawTextureEx(car.tex, {car.pos.x, car.pos.y}, 0.0f, 2.0f, WHITE);
                    }

                    else
                    {
                        DrawTextureEx(carVul.tex, {car.pos.x, car.pos.y}, 0.0f, 2.0f, WHITE);
                    }

                    for (int a = 0; a < sizeof(carRecs) / sizeof(carRecs[0]); a++)
                    {
                        /* DrawRectangleLines(carRecs[a].x, carRecs[a].y, carRecs[a].width, carRecs[a].height, hitBox);
                        DrawTextureEx(enemy1.tex, { carRecs[a].x - 14, carRecs[a].y }, 0.0f, 2.0f, WHITE);
                        carRecs[a].y += GetRandomValue(50, 300) * GetFrameTime() * carSpeed[a];
                        enemy1.pos.y += GetRandomValue(50, 300) * GetFrameTime() * carSpeed[a]; */
                        if (carRecs[a].y > GetScreenHeight() + 10)
                        {
                            carRecs[a].y = enemy[1].pos.y = GetRandomValue(-100, -1000);
                            showEnemy = true;
                        }
                    }

                    // drawing enemy and it's hitbox
                    if (showEnemy != false)
                    {
                        {
                            DrawRectangleLines(carRecs[0].x, carRecs[0].y, carRecs[0].width, carRecs[0].height, hitBox);
                            DrawTextureEx(enemy[1].tex, {carRecs[0].x - 14, carRecs[0].y}, 0.0f, 2.0f, WHITE);

                            DrawRectangleLines(carRecs[1].x, carRecs[1].y, carRecs[1].width, carRecs[1].height, hitBox);
                            DrawTextureEx(enemy[1].tex, {carRecs[1].x - 14, carRecs[1].y}, 0.0f, 2.0f, WHITE);

                            DrawRectangleLines(carRecs[2].x, carRecs[2].y, carRecs[2].width, carRecs[2].height, hitBox);
                            DrawTextureEx(enemy[1].tex, {carRecs[2].x - 14, carRecs[2].y}, 0.0f, 2.0f, WHITE);

                            DrawRectangleLines(carRecs[3].x, carRecs[3].y, carRecs[3].width, carRecs[3].height, hitBox);
                            DrawTextureEx(enemy[1].tex, {carRecs[3].x - 14, carRecs[3].y}, 0.0f, 2.0f, WHITE);

                            DrawRectangleLines(carRecs[4].x, carRecs[4].y, carRecs[4].width, carRecs[4].height, hitBox);
                            DrawTextureEx(enemy[1].tex, {carRecs[4].x - 14, carRecs[4].y}, 0.0f, 2.0f, WHITE);

                            DrawRectangleLines(carRecs[5].x, carRecs[5].y, carRecs[5].width, carRecs[5].height, hitBox);
                            DrawTextureEx(enemy[1].tex, {carRecs[5].x - 14, carRecs[5].y}, 0.0f, 2.0f, WHITE);

                            DrawRectangleLines(carRecs[6].x, carRecs[6].y, carRecs[6].width, carRecs[6].height, hitBox);
                            DrawTextureEx(enemy[1].tex, {carRecs[6].x - 14, carRecs[6].y}, 0.0f, 2.0f, WHITE);

                            DrawRectangleLines(carRecs[7].x, carRecs[7].y, carRecs[7].width, carRecs[7].height, hitBox);
                            DrawTextureEx(enemy[1].tex, {carRecs[7].x - 14, carRecs[7].y}, 0.0f, 2.0f, WHITE);

                            DrawRectangleLines(carRecs[8].x, carRecs[8].y, carRecs[8].width, carRecs[8].height, hitBox);
                            DrawTextureEx(enemy[1].tex, {carRecs[8].x - 14, carRecs[8].y}, 0.0f, 2.0f, WHITE);
                        }

                        // giving speed to hitbox and enemy
                        {
                            carRecs[0].y += GetRandomValue(50, 300) * GetFrameTime() * (carSpeed[0] + GetRandomValue(1, 1.5));
                            enemy[1].pos.y += GetRandomValue(50, 300) * GetFrameTime() * (carSpeed[0] + GetRandomValue(1, 1.5));

                            carRecs[1].y += GetRandomValue(50, 300) * GetFrameTime() * (carSpeed[1] + GetRandomValue(1.5, 2));
                            enemy[1].pos.y += GetRandomValue(50, 300) * GetFrameTime() * (carSpeed[1] + GetRandomValue(1.5, 2));

                            carRecs[2].y += GetRandomValue(50, 300) * GetFrameTime() * (carSpeed[2] + GetRandomValue(1.5, 2));
                            enemy[1].pos.y += GetRandomValue(50, 300) * GetFrameTime() * (carSpeed[2] + GetRandomValue(1.5, 2));

                            carRecs[3].y += GetRandomValue(50, 300) * GetFrameTime() * (carSpeed[3] + GetRandomValue(2, 1.5));
                            enemy[1].pos.y += GetRandomValue(50, 300) * GetFrameTime() * (carSpeed[3] + GetRandomValue(2, 1.5));

                            carRecs[4].y += GetRandomValue(50, 300) * GetFrameTime() * (carSpeed[4] + GetRandomValue(2.5, 1));
                            enemy[1].pos.y += GetRandomValue(50, 300) * GetFrameTime() * (carSpeed[4] + GetRandomValue(2.5, 1));

                            carRecs[5].y += GetRandomValue(50, 300) * GetFrameTime() * (carSpeed[5] + GetRandomValue(1, 2.5));
                            enemy[1].pos.y += GetRandomValue(50, 300) * GetFrameTime() * (carSpeed[5] + GetRandomValue(1, 2.5));

                            carRecs[6].y += GetRandomValue(50, 300) * GetFrameTime() * (carSpeed[6] + GetRandomValue(1.5, 2));
                            enemy[1].pos.y += GetRandomValue(50, 300) * GetFrameTime() * (carSpeed[6] + GetRandomValue(1.5, 2));

                            carRecs[7].y += GetRandomValue(50, 300) * GetFrameTime() * (carSpeed[7] + GetRandomValue(2, 1.5));
                            enemy[1].pos.y += GetRandomValue(50, 300) * GetFrameTime() * (carSpeed[7] + GetRandomValue(2, 1.5));

                            carRecs[8].y += GetRandomValue(50, 300) * GetFrameTime() * (carSpeed[8] + GetRandomValue(2.5, 2));
                            enemy[1].pos.y += GetRandomValue(50, 300) * GetFrameTime() * (carSpeed[8] + GetRandomValue(2.5, 2));
                        }

                        if (vulnerable == false)
                        {
                            for (int num = 0; num < sizeof(carRecs) / sizeof(carRecs[0]); num++)
                            {
                                if (CheckCollisionPointRec({player.x, player.y}, carRecs[num]) || CheckCollisionPointRec({player.x + 40, player.y}, carRecs[num]))
                                {
                                    vulnerable = true;
                                    if (vulnerable == true)
                                    {
                                        livesHard--;
                                    }
                                }
                                if (CheckCollisionPointRec({player.x, player.y + 64}, carRecs[num]) || CheckCollisionPointRec({player.x + 40, player.y + 64}, carRecs[num]))
                                {
                                    vulnerable = true;
                                    vulnerable = true;
                                    if (vulnerable == true)
                                    {
                                        livesHard--;
                                    }
                                }
                            }
                        }

                        if (vulnerable == true)
                        {
                            vulTime += GetFrameTime();
                            if (vulTime > 1)
                            {
                                vulnerable = false;
                                vulTime = 0;
                            }
                        }
                    }
                    else
                    {
                        // giving speed to hitbox and enemy before they become visible
                        {
                            carRecs[0].y += 300 * GetFrameTime() * carSpeed[0];
                            enemy[1].pos.y += 300 * GetFrameTime() * carSpeed[0];

                            carRecs[1].y += 300 * GetFrameTime() * carSpeed[1];
                            enemy[1].pos.y += 300 * GetFrameTime() * carSpeed[1];

                            carRecs[2].y += 300 * GetFrameTime() * carSpeed[2];
                            enemy[1].pos.y += 300 * GetFrameTime() * carSpeed[2];

                            carRecs[3].y += 300 * GetFrameTime() * carSpeed[3];
                            enemy[1].pos.y += 300 * GetFrameTime() * carSpeed[3];

                            carRecs[4].y += 300 * GetFrameTime() * carSpeed[4];
                            enemy[1].pos.y += 300 * GetFrameTime() * carSpeed[4];

                            carRecs[5].y += 300 * GetFrameTime() * carSpeed[5];
                            enemy[1].pos.y += 300 * GetFrameTime() * carSpeed[5];

                            carRecs[6].y += 300 * GetFrameTime() * carSpeed[6];
                            enemy[1].pos.y += 300 * GetFrameTime() * carSpeed[6];

                            carRecs[7].y += 300 * GetFrameTime() * carSpeed[7];
                            enemy[1].pos.y += 300 * GetFrameTime() * carSpeed[7];

                            carRecs[8].y += 300 * GetFrameTime() * carSpeed[8];
                            enemy[1].pos.y += 300 * GetFrameTime() * carSpeed[8];
                        }
                    }

                    DrawRectangleLines(player.x, player.y, 40, 64, hitBox);

                    if (IsKeyPressed(KEY_ESCAPE))
                    {
                        paused = true;
                        PauseMusicStream(hardStart);
                    }

                    if (livesHard == 0)
                    {
                        gameOver = true;
                        StopMusicStream(hardStart);
                    }

                    scoreHard += 250 * GetFrameTime();

                    DrawText(TextFormat("Lives: %i", livesHard), 50, 50, 30, WHITE);
                    DrawText(TextFormat("Score: %i", scoreHard), 50, 100, 30, WHITE);
                    DrawText(TextFormat("High Score: %i", highScoreHard), 50, 150, 30, WHITE);

                    if ((highScoreHard > 0) && (scoreHard > highScoreHard))
                    {
                        highScoreHard = scoreHard;
                        DrawText(TextFormat("New High Score!!!"), 50, 200, 30, WHITE);
                    }

                    EndDrawing();
                }

                else
                {
                    ClearBackground(BLACK);
                    BeginDrawing();
                    DrawText("Game Over, press ESC to go back to menu", 650, (GetScreenHeight() / 2) - 58, 30, WHITE);
                    DrawText(TextFormat("Lives: %i", livesHard), 50, 50, 30, WHITE);
                    DrawText(TextFormat("Score: %i", scoreHard), 50, 100, 30, WHITE);
                    DrawText(TextFormat("High Score: %i", highScoreHard), 50, 150, 30, WHITE);

                    tree1.pos.x = 50;
                    tree1.pos.y = -150;
                    tree2.pos.x = 163;
                    tree2.pos.y = -350;
                    tree3.pos.x = 256;
                    tree3.pos.y = -550;
                    tree4.pos.x = 300;
                    tree4.pos.y = -750;
                    tree4.pos.x = 300;
                    tree4.pos.y = -850;
                    tree6.pos.x = 1330;
                    tree6.pos.y = -480;
                    tree7.pos.x = 1486;
                    tree7.pos.y = -622;
                    tree8.pos.x = 1750;
                    tree8.pos.y = -885;
                    tree9.pos.x = 1400;
                    tree9.pos.y = -885;
                    tree10.pos.x = 1850;
                    tree10.pos.y = -885;

                    if (scoreHard > highScoreHard)
                    {
                        highScoreHard = scoreHard;
                    }

                    if (IsKeyPressed(KEY_ESCAPE))
                    {
                        {
                            hardScore.open("highscoreHard.txt", ios::out);
                            hardScore << highScoreHard;
                            hardScore.close();
                            car.pos.x = roadHalf.pos.x + (roadHalf.pos.x / 2) - 26.5;
                            car.pos.y = 1000;
                            player.x = roadHalf.pos.x + (roadHalf.pos.x / 2) - 15.28;
                            player.y = 1000;
                            showEnemy = false;
                            livesEasy = 3;
                            livesHard = 1;
                        }
                        currentScreen = MENU;
                    }
                    EndDrawing();
                }
            }
            else
            {
                BeginDrawing();
                speed = 0.0f;
                DrawRectangle(0, (GetScreenHeight() / 2) - 100, GetScreenWidth(), 100, WHITE);
                DrawText(pauseText, 650, (GetScreenHeight() / 2) - 60, 30, BLACK);

                if (IsKeyPressed(KEY_Y))
                {
                    {
                        currentScreen = MENU;
                        car.pos.x = roadHalf.pos.x + (roadHalf.pos.x / 2) - 26.5;
                        car.pos.y = 1000;
                        player.x = roadHalf.pos.x + (roadHalf.pos.x / 2) - 15.28;
                        player.y = 1000;
                        showEnemy = false;
                        livesEasy = 3;
                        livesHard = 1;
                    }
                }
                else if (IsKeyPressed(KEY_N))
                {
                    paused = false;
                }
                if (IsKeyPressed(KEY_ESCAPE))
                {
                    paused = false;
                }
                EndDrawing();
            }
            StopMusicStream(menu);
            break;
        }
        }
    }
    UnloadFont(font);

    // unload the road

    UnloadTexture(logo.tex);
    UnloadTexture(car.tex);
    UnloadTexture(carVul.tex);
    UnloadTexture(bg.tex);
    UnloadTexture(roadHalf.tex);

    UnloadTexture(tree1.tex);
    UnloadTexture(tree2.tex);
    UnloadTexture(tree3.tex);
    UnloadTexture(tree4.tex);
    UnloadTexture(tree5.tex);
    UnloadTexture(tree6.tex);
    UnloadTexture(tree7.tex);
    UnloadTexture(tree8.tex);
    UnloadTexture(tree9.tex);
    UnloadTexture(tree10.tex);

    UnloadMusicStream(menu);
    UnloadMusicStream(easyStart);
    UnloadSound(easyEnd);
    UnloadMusicStream(hardStart);
    UnloadSound(hardEnd);

    CloseWindow();
    CloseAudioDevice();

    return 0;
}

void updateCarPos()
{
    for (int re = 0; re < sizeof(carRecs) / sizeof(carRecs[0]); re++)
    {
        carRecs[re].width = 36;
        carRecs[re].height = 64;
        if (re != 0)
        {
            carRecs[re].x = 732 + ((carRecs[re].width * re) * 1.5);
        }
        else
        {
            carRecs[re].x = 732;
        }
        carRecs[re].y = 20;
    }
}
