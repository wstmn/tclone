#ifndef GLOBALS_H
#define GLOBALS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>

// Screen dimension constants
const int NATIVE_WIDTH = 320;
const int NATIVE_HEIGHT = 180;
const int SCALE_FACTOR = 4;
const int SCREEN_WIDTH = NATIVE_WIDTH * SCALE_FACTOR;
const int SCREEN_HEIGHT = NATIVE_HEIGHT * SCALE_FACTOR;
const int GRAVITY = 1;

const int JUMP_HEIGHT = 10;

// Declare global instances of SDL_Window and SDL_Renderer
extern SDL_Window* gWindow;
extern SDL_Renderer* gRenderer;

// Declare global sprite clips array
extern SDL_Rect gSpriteClips[12];

// Forward declarations of utility functions
extern bool init();
extern bool loadMedia();
extern void close();
extern SDL_Texture* loadTexture(std::string path);
extern bool checkCollision(SDL_Rect a, SDL_Rect b);

#endif  // GLOBALS_H


