#include <iostream>
#include <string>
#include "raylib.h"
#include "raymath.h"

using namespace std;

const int screenWidth = 1024;
const int screenHeight = 600;
const int frameRate = 60;
int scorePlayer = 0;
int scoreCPU = 0;

struct Paddle
{
    Texture2D texture;
    Vector2 position;
    int speed_y;
    int width;
    int height;
};

struct Ball
{
    Texture2D texture;
    Vector2 position;
    Vector2 velocity;
    int speed;
    int width;
    int height;
};


Paddle initPlayer(const string& texturePath) {
    Paddle player{};
    player.texture = LoadTexture(texturePath.c_str());
    player.position = {15, screenHeight / 2.0f};
    player.speed_y = 400;
    player.width = (int)player.texture.width;
    player.height = (int)player.texture.height;
    return player;
}

void updatePlayer(Paddle& player, float deltaTime) {
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
        player.position.y -= player.speed_y * deltaTime;

    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
        player.position.y += player.speed_y * deltaTime;

    if (player.position.y < 0)
        player.position.y = 0;

    if (player.position.y + player.height > screenHeight)
        player.position.y = screenHeight - player.height;
}

void drawPlayer(const Paddle& player) {
    DrawTextureV(player.texture, player.position, WHITE);
}


void resetBall(Ball& ball) {
    ball.position = {screenWidth / 2.0f, screenHeight / 2.0f};

    int randomAngle = GetRandomValue(25, 85);
    float radAngle = randomAngle * DEG2RAD;

    // Randomly send ball left or right
    float dirX = (GetRandomValue(0, 1) == 0) ? 1.0f : -1.0f;
    float dirY = (GetRandomValue(0, 1) == 0) ? 1.0f : -1.0f;

    ball.velocity.x = dirX * 300 * cosf(radAngle);
    ball.velocity.y = dirY * 300 * sinf(radAngle);
}

Ball initBall(const string& texturePath) {
    Ball ball{};
    ball.texture = LoadTexture(texturePath.c_str());
    ball.speed = 300;
    ball.width = (int)ball.texture.width;
    ball.height = (int)ball.texture.height;
    resetBall(ball);
    return ball;
}


void updateBall(Ball& ball, float deltaTime) {

    ball.position.x += ball.velocity.x * deltaTime;
    ball.position.y += ball.velocity.y * deltaTime;

    // Top / bottom wall bounce
    if (ball.position.y < 0) {
        ball.position.y = 0;
        // ball.speed += 300;
        ball.velocity.y *= -1;
    }
    if (ball.position.y + ball.height > screenHeight) {
        ball.position.y = screenHeight - ball.height;
        ball.velocity.y *= -1;
    }
    cout<<ball.speed<<endl;
    // Ball passes player's side → CPU scores
    if (ball.position.x < 0) {
        scoreCPU += 10;
        resetBall(ball);
    }

    // Ball passes CPU's side → Player scores
    if (ball.position.x + ball.width > screenWidth) {
        scorePlayer += 10;
        resetBall(ball);
    }
}

void drawBall(const Ball& ball) {
    DrawTextureV(ball.texture, ball.position, WHITE);
}


Paddle initCPU(const string& texturePath) {
    Paddle cpu{};
    cpu.texture = LoadTexture(texturePath.c_str());
    cpu.position = {screenWidth - 30.0f, screenHeight / 2.0f};
    cpu.speed_y = 300;
    cpu.width = (int)cpu.texture.width;
    cpu.height = (int)cpu.texture.height;
    return cpu;
}

void updateCPU(Paddle& cpu, Ball& ball, float deltaTime) {
    float ballCenter = ball.position.y + ball.height / 2.0f;
    float cpuCenter = cpu.position.y + cpu.height / 2.0f;

    if (cpuCenter < ballCenter)
        cpu.position.y += cpu.speed_y * deltaTime;
    else if (cpuCenter > ballCenter)
        cpu.position.y -= cpu.speed_y * deltaTime;

    if (cpu.position.y < 0)
        cpu.position.y = 0;

    if (cpu.position.y + cpu.height > screenHeight)
        cpu.position.y = screenHeight - cpu.height;
}

void drawCPU(const Paddle& cpu) {
    DrawTextureV(cpu.texture, cpu.position, WHITE);
}


void collisionDetection(Ball& ball, Paddle& player, Paddle& cpu)
{
    Rectangle playerRect = {
        player.position.x, player.position.y,
        (float)player.width, (float)player.height
    };
    Rectangle cpuRect = {
        cpu.position.x, cpu.position.y,
        (float)cpu.width, (float)cpu.height
    };
    Rectangle ballRect = {
        ball.position.x, ball.position.y,
        (float)ball.width, (float)ball.height
    };

    if (CheckCollisionRecs(playerRect, ballRect))
    {
        ball.velocity.x = fabsf(ball.velocity.x); // always bounce right
        ball.position.x = player.position.x + player.width;
    }
    if (CheckCollisionRecs(cpuRect, ballRect))
    {
        ball.velocity.x = -fabsf(ball.velocity.x); // always bounce left
        ball.position.x = cpu.position.x - ball.width;
    }
}

int main()
{
    InitWindow(screenWidth, screenHeight, "Ping Pong Game");
    SetTargetFPS(frameRate);

    Paddle player = initPlayer("../assets/player.png");
    Ball ball = initBall("../assets/ball.png");
    Paddle cpu = initCPU("../assets/enemy.png");

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        
        updatePlayer(player, deltaTime);
        updateBall(ball, deltaTime);
        updateCPU(cpu, ball, deltaTime);
        collisionDetection(ball, player, cpu);

        BeginDrawing();
        ClearBackground(DARKGRAY);

        // Grid
        for (int i = 0; i < screenWidth; i += 32)
            DrawLine(i, 0, i, screenHeight, Fade(RAYWHITE, 0.5f));
        for (int i = 0; i < screenHeight; i += 32)
            DrawLine(0, i, screenWidth, i, Fade(RAYWHITE, 0.5f));

        // FIX 1: score variables now match the correct labels
        DrawText(TextFormat("Player Score: %d", scorePlayer), 10, 10, 20, GREEN);
        DrawText(TextFormat("CPU Score: %d", scoreCPU), screenWidth - 200, 10, 20, YELLOW);

        drawPlayer(player);
        drawBall(ball);
        drawCPU(cpu);

        EndDrawing();
    }

    UnloadTexture(player.texture);
    UnloadTexture(ball.texture);
    UnloadTexture(cpu.texture);
    CloseWindow();

    return 0;
}