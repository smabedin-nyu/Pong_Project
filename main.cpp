/**
 * Author: Shahzaib
 * Assignment: Pong Clone
 * Date due: 10/14/2026
 *
 * I pledge that I have completed this assignment without
 * collaborating with anyone else, in conformance with the
 * NYU School of Engineering Policies and Procedures on
 * Academic Misconduct.
 */

#include "CS3113/cs3113.h"

// Scenes
enum SceneState {PLAYING, GAME_OVER};
AppStatus gAppStatus = RUNNING;
SceneState gSceneState = PLAYING;

bool gSinglePlayer = false;

// Screen
constexpr int SCREEN_WIDTH  = 1200,
              SCREEN_HEIGHT = 800,
              FPS           = 60;

// Background
constexpr char BACKGROUND_FP[] = "Assets/field.png"; // modified from source: https://kenney.nl/assets/sports-pack
Texture2D gBackgroundTexture;

// Red Player
constexpr char RED_PLAYER_FP[] = "Assets/red_player.png"; // modified from source: https://kenney.nl/assets/sports-pack

constexpr Vector2 RED_ORIGIN  = {90.0f, 400.0f},
                  PLAYER_SIZE = {63.0f, 93.0f};

constexpr float PLAYER_SPEED = 500.0f;

Vector2 gRedPosition = RED_ORIGIN,
        gRedMovement = {0.0f, 0.0f};

Texture2D gRedTexture;

// Blue Player
constexpr char BLUE_PLAYER_FP[] = "Assets/blue_player.png"; // modified from source: https://kenney.nl/assets/sports-pack

constexpr Vector2 BLUE_ORIGIN = {1110.0f, 400.0f};

Vector2 gBluePosition = BLUE_ORIGIN,
        gBlueMovement = {0.0f, 0.0f};

float gBlueDirection = -1.0f;

Texture2D gBlueTexture;

// Soccer Balls
constexpr char BALL_FP[] = "Assets/soccer_ball.png"; // source:

constexpr Vector2 BALL_SIZE = {36.0f, 36.0f};

constexpr int MAX_BALLS = 3;

constexpr float BALL_SPEED_X = 600.0f,
                BALL_SPEED_Y = 360.0f;

Vector2 gBallPosition[MAX_BALLS] = {{600.0f, 400.0f}, {600.0f, 310.0f}, {600.0f, 490.0f}};

Vector2 gBallVelocity[MAX_BALLS] = {{BALL_SPEED_X, BALL_SPEED_Y}, {-BALL_SPEED_X, -BALL_SPEED_Y}, {BALL_SPEED_X, -BALL_SPEED_Y}};

int gActiveBalls = 1;

Texture2D gBallTexture;

// Score
constexpr char SCORE_FP[] = "Assets/score_values.png"; // Created this Sprite Sheet myself on Illustrator
Texture2D gScoreTexture;
constexpr int WINNING_SCORE = 5;
int gRedScore  = 0,
    gBlueScore = 0;

// Delta Time
float gPreviousTicks = 0.0f;

// Forward Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();
void resetGame();
void resetBall(int index);
void drawObject(Texture2D texture, Vector2 position, Vector2 scale, float angle);
void drawScore(int score, bool red, float x, float y);
bool checkCollision(Vector2 firstPosition, Vector2 firstSize,
                    Vector2 secondPosition, Vector2 secondSize);

// initialise
void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Pong Project");

    gBackgroundTexture = LoadTexture(BACKGROUND_FP);
    gRedTexture        = LoadTexture(RED_PLAYER_FP);
    gBlueTexture       = LoadTexture(BLUE_PLAYER_FP);
    gBallTexture       = LoadTexture(BALL_FP);
    gScoreTexture      = LoadTexture(SCORE_FP);

    SetTargetFPS(FPS);

    gPreviousTicks = static_cast<float>(GetTime());
}

// processInput
void processInput()
{
    if (WindowShouldClose() || IsKeyPressed(KEY_Q))
        gAppStatus = TERMINATED;

    gRedMovement.y  = 0.0f;
    gBlueMovement.y = 0.0f;

    if (IsKeyPressed(KEY_R)) resetGame();

    if (gSceneState != PLAYING) return;

    if (IsKeyPressed(KEY_T))
        gSinglePlayer = !gSinglePlayer;

    if (IsKeyPressed(KEY_ONE))
        gActiveBalls = 1;

    else if (IsKeyPressed(KEY_TWO))
    {
        if (gActiveBalls < 2) resetBall(1);
        gActiveBalls = 2;
    }

    else if (IsKeyPressed(KEY_THREE))
    {
        if (gActiveBalls < 2) resetBall(1);
        if (gActiveBalls < 3) resetBall(2);
        gActiveBalls = 3;
    }

    if (IsKeyDown(KEY_W))
        gRedMovement.y -= 1.0f;

    if (IsKeyDown(KEY_S))
        gRedMovement.y += 1.0f;

    if (!gSinglePlayer)
    {
        if (IsKeyDown(KEY_UP))
            gBlueMovement.y -= 1.0f;

        if (IsKeyDown(KEY_DOWN))
            gBlueMovement.y += 1.0f;
    }
}

// update
void update()
{
    float ticks = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks = ticks;

    if (gSceneState != PLAYING) return;

    gRedPosition.y += gRedMovement.y * PLAYER_SPEED * deltaTime;

    if (gSinglePlayer)
        gBluePosition.y += gBlueDirection * PLAYER_SPEED * deltaTime;

    else
        gBluePosition.y += gBlueMovement.y * PLAYER_SPEED * deltaTime;

    if (gRedPosition.y <= PLAYER_SIZE.y / 2.0f)
        gRedPosition.y = PLAYER_SIZE.y / 2.0f;

    if (gRedPosition.y >= SCREEN_HEIGHT - PLAYER_SIZE.y / 2.0f)
        gRedPosition.y = SCREEN_HEIGHT - PLAYER_SIZE.y / 2.0f;

    if (gBluePosition.y <= PLAYER_SIZE.y / 2.0f)
    {
        gBluePosition.y = PLAYER_SIZE.y / 2.0f;
        gBlueDirection = 1.0f;
    }

    else if (gBluePosition.y >= SCREEN_HEIGHT - PLAYER_SIZE.y / 2.0f)
    {
        gBluePosition.y = SCREEN_HEIGHT - PLAYER_SIZE.y / 2.0f;
        gBlueDirection = -1.0f;
    }

    for (int i = 0; i < gActiveBalls; i++)
    {
        gBallPosition[i].x += gBallVelocity[i].x * deltaTime;
        gBallPosition[i].y += gBallVelocity[i].y * deltaTime;

        if (gBallPosition[i].y <= BALL_SIZE.y / 2.0f)
        {
            gBallPosition[i].y = BALL_SIZE.y / 2.0f;
            gBallVelocity[i].y = -gBallVelocity[i].y;
        }

        else if (gBallPosition[i].y >= SCREEN_HEIGHT - BALL_SIZE.y / 2.0f)
        {
            gBallPosition[i].y = SCREEN_HEIGHT - BALL_SIZE.y / 2.0f;
            gBallVelocity[i].y = -gBallVelocity[i].y;
        }

        if (gBallVelocity[i].x < 0.0f &&
            checkCollision(gBallPosition[i], BALL_SIZE, gRedPosition, PLAYER_SIZE))
        {
            gBallPosition[i].x = gRedPosition.x + PLAYER_SIZE.x / 2.0f + BALL_SIZE.x / 2.0f;
            gBallVelocity[i].x = -gBallVelocity[i].x;
        }

        if (gBallVelocity[i].x > 0.0f &&
            checkCollision(gBallPosition[i], BALL_SIZE, gBluePosition, PLAYER_SIZE))
        {
            gBallPosition[i].x = gBluePosition.x - PLAYER_SIZE.x / 2.0f - BALL_SIZE.x / 2.0f;
            gBallVelocity[i].x = -gBallVelocity[i].x;
        }

        if (gBallPosition[i].x - BALL_SIZE.x / 2.0f > SCREEN_WIDTH)
        {
            gRedScore++;
            resetBall(i);
        }

        else if (gBallPosition[i].x + BALL_SIZE.x / 2.0f < 0.0f)
        {
            gBlueScore++;
            resetBall(i);
        }

        if (gRedScore >= WINNING_SCORE || gBlueScore >= WINNING_SCORE)
        {
            gSceneState = GAME_OVER;
            break;
        }
    }
}

// resetBall: Returns a soccer ball to its starting position
void resetBall(int index)
{
    gBallPosition[index].x = SCREEN_WIDTH / 2.0f;
    gBallPosition[index].y = SCREEN_HEIGHT / 2.0f + (index - 1) * 90.0f;

    if (index == 1)
    {
        gBallVelocity[index].x = -BALL_SPEED_X;
        gBallVelocity[index].y = -BALL_SPEED_Y;
    }

    else
    {
        gBallVelocity[index].x = BALL_SPEED_X;

        if (index == 0)
            gBallVelocity[index].y = BALL_SPEED_Y;

        else
            gBallVelocity[index].y = -BALL_SPEED_Y;
    }
}

// resetGame: Restarts the match
void resetGame()
{
    gRedPosition  = RED_ORIGIN;
    gBluePosition = BLUE_ORIGIN;

    gRedMovement  = {0.0f, 0.0f};
    gBlueMovement = {0.0f, 0.0f};

    gBlueDirection = -1.0f;
    gSinglePlayer  = false;
    gActiveBalls   = 1;

    gRedScore  = 0;
    gBlueScore = 0;

    gSceneState = PLAYING;

    for (int i = 0; i < MAX_BALLS; i++)
    {
        resetBall(i);
    }
}

// checkCollision
bool checkCollision(Vector2 firstPosition, Vector2 firstSize,
                    Vector2 secondPosition, Vector2 secondSize)
{
    return firstPosition.x - firstSize.x / 2.0f < secondPosition.x + secondSize.x / 2.0f &&
           firstPosition.x + firstSize.x / 2.0f > secondPosition.x - secondSize.x / 2.0f &&
           firstPosition.y - firstSize.y / 2.0f < secondPosition.y + secondSize.y / 2.0f &&
           firstPosition.y + firstSize.y / 2.0f > secondPosition.y - secondSize.y / 2.0f;
}

// drawObject
void drawObject(Texture2D texture, Vector2 position, Vector2 scale, float angle)
{
    Rectangle textureArea = {0.0f, 0.0f, static_cast<float>(texture.width), static_cast<float>(texture.height)};

    Rectangle destinationArea = {position.x, position.y, scale.x, scale.y};

    Vector2 objectOrigin = {scale.x / 2.0f, scale.y / 2.0f};

    DrawTexturePro(texture, textureArea, destinationArea, objectOrigin, angle, WHITE);
}

// drawScore: Draws the Scores of Red, Blue, and the - from the spritesheet
void drawScore(int score, bool red, float x, float y)
{
    float redPositions[6]  = {0.0f, 40.0f, 67.0f, 101.0f, 134.0f, 168.0f};
    float bluePositions[6] = {235.0f, 274.0f, 302.0f, 335.0f, 369.0f, 402.0f};

    float sourceX;

    if (red)
        sourceX = redPositions[score];
    else
        sourceX = bluePositions[score];

    Rectangle source = {sourceX, 5.0f, 33.0f, 44.0f};
    Rectangle destination = {x, y, 50.0f, 66.0f};
    Vector2 origin = {0.0f, 0.0f};

    DrawTexturePro(gScoreTexture, source, destination, origin, 0.0f, WHITE);
}

// render
void render()
{
    BeginDrawing();

    ClearBackground(DARKGREEN);

    Rectangle backgroundArea = {0.0f, 0.0f, static_cast<float>(gBackgroundTexture.width), static_cast<float>(gBackgroundTexture.height)};

    Rectangle backgroundDestination = {0.0f, 0.0f, static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT)};

    Vector2 backgroundOrigin = {0.0f, 0.0f};

    DrawTexturePro(gBackgroundTexture, backgroundArea, backgroundDestination, backgroundOrigin, 0.0f, WHITE);

    drawObject(gRedTexture, gRedPosition, PLAYER_SIZE, 0.0f);
    drawObject(gBlueTexture, gBluePosition, PLAYER_SIZE, 0.0f);

    for (int i = 0; i < gActiveBalls; i++)
        drawObject(gBallTexture, gBallPosition[i], BALL_SIZE, 0.0f);

    drawScore(gRedScore, true, 505.0f, 20.0f);
    drawScore(gBlueScore, false, 645.0f, 20.0f);

    Rectangle dashSource = {439.0f, 5.0f, 28.0f, 44.0f};
    Rectangle dashDestination = {585.0f, 20.0f, 30.0f, 66.0f};
    Vector2 scoreOrigin = {0.0f, 0.0f};

    DrawTexturePro(gScoreTexture, dashSource, dashDestination, scoreOrigin, 0.0f, WHITE);

    if (gSceneState == GAME_OVER)
    {
        DrawRectangle(270, 295, 660, 195, BLACK);

        Rectangle winnerSource;

        if (gRedScore >= WINNING_SCORE)
            winnerSource = {0.0f, 72.0f, 436.0f, 48.0f};

        else
            winnerSource = {0.0f, 142.0f, 436.0f, 48.0f};

        Rectangle winnerDestination = {325.0f, 360.0f, 550.0f, 60.0f};
        Vector2 winnerOrigin = {0.0f, 0.0f};

        DrawTexturePro(gScoreTexture, winnerSource, winnerDestination, winnerOrigin, 0.0f, WHITE);
    }

    EndDrawing();
}

// shutdown
void shutdown()
{
    UnloadTexture(gBackgroundTexture);
    UnloadTexture(gRedTexture);
    UnloadTexture(gBlueTexture);
    UnloadTexture(gBallTexture);
    UnloadTexture(gScoreTexture);

    CloseWindow();
}

// main
int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}