#include <SDL2/SDL.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <SDL2/SDL_image.h>

// Screen dimensions
const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

// Texture dimensions
const int TEXTURE_WIDTH = 64;
const int TEXTURE_HEIGHT = 64;

// Map dimensions
const int MAP_WIDTH = 24;
const int MAP_HEIGHT = 24;

// Define the map
std::vector<std::vector<int>> map = {
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
  {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
  {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

// Player position and direction
double posX = 1.0, posY = 1.2; // Player start position
double dirX = 1.0, dirY = 0.40; // Initial direction vector
double planeX = 0.0, planeY = 0.60; // Camera plane

// Rotation speed
const double rotSpeed = 0.55;
// Movement speed
const double moveSpeed = 0.30;

SDL_Texture* wallTextures[5];
SDL_Texture* floorTexture;
SDL_Texture* ceilingTexture;

bool loadTextures(SDL_Renderer* renderer) {
    wallTextures[0] = IMG_LoadTexture(renderer, "pics/wall5.png");
    wallTextures[1] = IMG_LoadTexture(renderer, "pics/wall5.png");
    wallTextures[2] = IMG_LoadTexture(renderer, "pics/wall5.png");
    wallTextures[3] = IMG_LoadTexture(renderer, "pics/wall5.png");
    wallTextures[4] = IMG_LoadTexture(renderer, "pics/wall5.png");
    floorTexture = IMG_LoadTexture(renderer, "pics/wall6.png");
    ceilingTexture = IMG_LoadTexture(renderer, "pics/wall4.png");

    for (int i = 0; i < 5; ++i) {
        if (!wallTextures[i]) {
            std::cerr << "Failed to load texture: " << IMG_GetError() << std::endl;
            return false;
        }
    }
    if (!floorTexture || !ceilingTexture) {
        std::cerr << "Failed to load floor or ceiling texture: " << IMG_GetError() << std::endl;
        return false;
    }
    return true;
}

void drawFloorAndCeiling(SDL_Renderer* renderer) {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        // Ray direction for the leftmost and rightmost rays
        float rayDirX0 = dirX - planeX;
        float rayDirY0 = dirY - planeY;
        float rayDirX1 = dirX + planeX;
        float rayDirY1 = dirY + planeY;

        // Current y position compared to the center of the screen (the horizon)
        int p = y - SCREEN_HEIGHT / 2;

        // Vertical position of the camera.
        float posZ = 0.5 * SCREEN_HEIGHT;

        // Horizontal distance from the camera to the floor for the current row.
        float rowDistance = posZ / p;

        // Calculate the real world step vector we have to add for each x (parallel to camera plane)
        float floorStepX = rowDistance * (rayDirX1 - rayDirX0) / SCREEN_WIDTH;
        float floorStepY = rowDistance * (rayDirY1 - rayDirY0) / SCREEN_WIDTH;

        // Real world coordinates of the leftmost column. This will be updated as we step to the right.
        float floorX = posX + rowDistance * rayDirX0;
        float floorY = posY + rowDistance * rayDirY0;

        for (int x = 0; x < SCREEN_WIDTH; x++) {
            // The cell coord is simply got from the integer parts of floorX and floorY
            int cellX = (int)(floorX);
            int cellY = (int)(floorY);

            // Get the texture coordinate from the fractional part
            int tx = (int)(TEXTURE_WIDTH * (floorX - cellX)) & (TEXTURE_WIDTH - 1);
            int ty = (int)(TEXTURE_HEIGHT * (floorY - cellY)) & (TEXTURE_HEIGHT - 1);

            floorX += floorStepX;
            floorY += floorStepY;

            // Draw the floor pixel
            SDL_Rect sourceRect = { tx, ty, 1, 1 };
            SDL_Rect destRect = { x, y, 1, 1 };
            SDL_RenderCopy(renderer, floorTexture, &sourceRect, &destRect);

            // Draw the ceiling pixel
            destRect.y = SCREEN_HEIGHT - y - 1;
            SDL_RenderCopy(renderer, ceilingTexture, &sourceRect, &destRect);
        }
    }
}

void drawWalls(SDL_Renderer* renderer) {
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        double cameraX = 2 * x / double(SCREEN_WIDTH) - 1;
        double rayDirX = dirX + planeX * cameraX;
        double rayDirY = dirY + planeY * cameraX;

        int mapX = int(posX);
        int mapY = int(posY);

        double sideDistX;
        double sideDistY;

        double deltaDistX = std::abs(1 / rayDirX);
        double deltaDistY = std::abs(1 / rayDirY);
        double perpWallDist;

        int stepX;
        int stepY;

        bool hit = false;
        int side;

        if (rayDirX < 0) {
            stepX = -1;
            sideDistX = (posX - mapX) * deltaDistX;
        } else {
            stepX = 1;
            sideDistX = (mapX + 1.0 - posX) * deltaDistX;
        }
        if (rayDirY < 0) {
            stepY = -1;
            sideDistY = (posY - mapY) * deltaDistY;
        } else {
            stepY = 1;
            sideDistY = (mapY + 1.0 - posY) * deltaDistY;
        }

        while (!hit) {
            if (sideDistX < sideDistY) {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            } else {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }
            if (map[mapX][mapY] > 0) hit = true;
        }

        if (side == 0)
            perpWallDist = (mapX - posX + (1 - stepX) / 2) / rayDirX;
        else
            perpWallDist = (mapY - posY + (1 - stepY) / 2) / rayDirY;

        int lineHeight = (int)(SCREEN_HEIGHT / perpWallDist);

        int drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2;
        if (drawStart < 0) drawStart = 0;
        int drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2;
        if (drawEnd >= SCREEN_HEIGHT) drawEnd = SCREEN_HEIGHT - 1;

        int textureIndex = map[mapX][mapY] - 1;
        SDL_Texture* texture = wallTextures[textureIndex];

        SDL_Rect sourceRect;
        sourceRect.y = 0;
        sourceRect.w = TEXTURE_WIDTH;
        sourceRect.h = TEXTURE_HEIGHT;

        SDL_Rect destRect;
        destRect.x = x;
        destRect.w = 1;
        destRect.h = drawEnd - drawStart;
        destRect.y = drawStart;

        if (side == 0) {
            sourceRect.x = int((posY + perpWallDist * rayDirY) * TEXTURE_WIDTH) % TEXTURE_WIDTH;
        } else {
            sourceRect.x = int((posX + perpWallDist * rayDirX) * TEXTURE_WIDTH) % TEXTURE_WIDTH;
        }

        SDL_RenderCopy(renderer, texture, &sourceRect, &destRect);
    }
}

void moveForward() {
    if (map[int(posX + dirX * moveSpeed)][int(posY)] == false) posX += dirX * moveSpeed;
    if (map[int(posX)][int(posY + dirY * moveSpeed)] == false) posY += dirY * moveSpeed;
}

void moveBackward() {
    if (map[int(posX - dirX * moveSpeed)][int(posY)] == false) posX -= dirX * moveSpeed;
    if (map[int(posX)][int(posY - dirY * moveSpeed)] == false) posY -= dirY * moveSpeed;
}

void rotateLeft() {
    double oldDirX = dirX;
    dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
    dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
    double oldPlaneX = planeX;
    planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
    planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
}

void rotateRight() {
    double oldDirX = dirX;
    dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
    dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
    double oldPlaneX = planeX;
    planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
    planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
}

int main(int argc, char* args[]) {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    SDL_Window* window = SDL_CreateWindow("Eli's Maze Demo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (!loadTextures(renderer)) {
        return 1;
    }

    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_UP: moveForward(); break;
                    case SDLK_DOWN: moveBackward(); break;
                    case SDLK_LEFT: rotateLeft(); break;
                    case SDLK_RIGHT: rotateRight(); break;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        drawFloorAndCeiling(renderer);
        drawWalls(renderer);

        SDL_RenderPresent(renderer);
    }

    for (int i = 0; i < 5; ++i) {
        SDL_DestroyTexture(wallTextures[i]);
    }
    SDL_DestroyTexture(floorTexture);
    SDL_DestroyTexture(ceilingTexture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    IMG_Quit();
    SDL_Quit();

    return 0;
}
